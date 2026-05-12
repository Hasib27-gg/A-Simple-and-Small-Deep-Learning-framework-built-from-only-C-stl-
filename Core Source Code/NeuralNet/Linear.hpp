#ifndef LINEAR_HPP
#define LINEAR_HPP

#include "LinearAlgebra/Linalg.hpp"
namespace NeuralNet {
using GenericPtr = void *;
enum struct Mode { Training, Evaluation };

template <typename InputType = float, typename InputGradType = float,
          typename OutputType = float, typename OutputGradType = float,
          typename WeightType = float, typename BiasType = float,
          typename WeightGradType = float, typename BiasGradType = float>
class Linear {
private:
  // all params:
  Linalg::Matrix<WeightType> _weight;
  Linalg::Matrix<BiasType> _bias;
  Linalg::Matrix<WeightGradType> _weight_grad;
  Linalg::Matrix<BiasGradType> _bias_grad;

  // buffers:
  Linalg::Matrix<InputType> _inp;
  Linalg::Matrix<OutputGradType> _out_grad_buff;
  Linalg::Matrix<OutputType> _matmul_res_buff;

  // pointers to additional bufferes:
  GenericPtr _weight_additionals = nullptr;
  GenericPtr _bias_additionals = nullptr;

  // flags:
  bool _use_bias = true;
  Mode _curr_mode = Mode::Training;

  // functors:
  Linalg::Reductions::Sum<InputGradType, BiasGradType> _func_sum;
  Linalg::Zipables::Add<OutputType, BiasType, OutputType> _func_add;
  Linalg::Indexers::RowRepeater _bias_indexer;

  // helper default value generators:
  // _get0 -> returns 0
  template <typename NumTy> inline static NumTy _get0(const std::size_t) {
    return 0;
  }
  // _getr -> returns a pseudo random [range : (0 - 1)]:
  template <typename NumTy> inline static NumTy _getr(const std::size_t idx) {
    return (std::sin(((idx << 2) ^ idx) * 10.954) + 1) / 2;
  } // @TODO: Maybe change into kamming ?

public:
  template <typename ValueGen1 = decltype(_getr<WeightType>),
            typename ValueGen2 = decltype(_get0<BiasType>),
            typename ValueGen3 = decltype(_get0<WeightGradType>),
            typename ValueGen4 = decltype(_get0<BiasGradType>)>
  inline void init(const Linalg::NumPair &inp_shape, const std::size_t out_size,
                   const bool bias = true,
                   ValueGen1 &weight_init_func = _getr<WeightType>,
                   ValueGen2 &bias_init_func = _get0<BiasType>,
                   ValueGen3 &weight_grad_init_func = _get0<WeightGradType>,
                   ValueGen4 &bias_grad_init_func = _get0<BiasGradType>,
                   const Mode init_mode = Mode::Training) {

    // ----- CORE ALLOCATION AND FILLING ------
    self._weight =
        Linalg::createMatrix<WeightType>(out_size, inp_shape.second, 1);
    self._weight_grad =
        Linalg::createMatrix<WeightGradType>(out_size, inp_shape.second, 2);

    Linalg::fill(_weight, weight_init_func);
    Linalg::fill(_weight_grad, weight_grad_init_func);

    if (bias) {
      self._bias = Linalg::createMatrix<BiasType>(1, out_size, 6);
      self._bias_grad = Linalg::createMatrix<BiasGradType>(1, out_size, 7);
      self._bias_indexer.setColumnSize(out_size);
      Linalg::fill(self._bias, bias_init_func);
      Linalg::fill(self._bias_grad, bias_grad_init_func);
    }

    // ---- STORING FLAGS ----
    self._use_bias = bias;
    self._curr_mode = init_mode;

    // ---- ALLOCATION FOR BUFFERS ----
    self._inp =
        Linalg::createMatrix<InputType>(inp_shape.first, inp_shape.second, 3);
    self._matmul_res_buff =
        Linalg::createMatrix<OutputType>(inp_shape.first, out_size, 4);
    self._out_grad_buff = Linalg::createMatrix<OutputGradType>(
        inp_shape.first, inp_shape.second, 5);
  }

  Linalg::Matrix<OutputType> forward(Linalg::Matrix<InputType> &x) {

    if (self._curr_mode == Mode::Training)
      Linalg::copy_if_allocated(x, self._inp, 3);

    // ---- CORE MATMUL ----
    Linalg::transpose(self._weight); // w_t
    Linalg::matmul_no_accum(x, self._weight, self._matmul_res_buff);
    Linalg::transpose(self._weight); // w

    // the bias addition part
    if (self._use_bias)
      Linalg::zip_no_accum(self._matmul_res_buff, self._bias,
                           self._matmul_res_buff, self._func_add,
                           Linalg::Indexers::defaultIndexer,
                           self._bias_indexer // becuase bias is a row vector!
      );

    return self._matmul_res_buff;
  }
  Linalg::Matrix<OutputGradType> backward(Linalg::Matrix<InputGradType> &grad) {

    if (self._curr_mode == Mode::Evaluation)
      throw std::runtime_error(
          "Calling .backward() is ambigious in elvaluation");
    /*
       formulas:
       w_grad = matmul(g_t, inp_grad) + w_grad
       b_grad = sum_along_rows(g_t, inp_grad) + b_grad
       out_grad = matmul(g, w)
    */

    // grad -> grad_t
    Linalg::transpose(grad);
    // w_grad = matmul(g_t, inp_grad) + w_grad
    Linalg::matmul_with_accum(grad, self._inp, self._weight_grad);
    // b_grad = sum_along_rows(g_t, inp_grad) + b_grad
    if (self._use_bias)
      Linalg::reduce_with_accum(grad, self._bias_grad, self._func_sum, 1);
    // grad_t -> grad
    Linalg::transpose(grad);
    // out_grad = matmul(g, w)
    Linalg::matmul_no_accum(grad, self._weight, self._out_grad_buff);
    return self._out_grad_buff;
  }
  template <typename OptimizerTy> inline void step(OptimizerTy &optim) {

    /*
     Optimizer must have the following template for .update():

     template<typename Ty1, typename Ty2>
     [#maybe inline#] void update(std::pair<Ty1&, Ty2&> param_pair,
                                  GenericPtr& additionals)
    */
    optim.update(std::pair<Linalg::Matrix<WeightType> &, Linalg::Matrix<WeightGradType> &>(
                     self._weight, self._weight_grad),
                 self._weight_additionals);
    optim.update(std::pair<Linalg::Matrix<BiasType> &, Linalg::Matrix<BiasGradType> &>(
                     self._bias, self._bias_grad),
                 self._bias_additionals);
  }

  inline std::pair<Linalg::Matrix<WeightType> &,
                   Linalg::Matrix<WeightGradType> &>
  weight_pair() {
    return {self._weight, self._weight_grad};
  }

  inline std::pair<Linalg::Matrix<BiasType> &, Linalg::Matrix<BiasGradType> &>
  bias_pair() {
    return {self._bias, self._bias_grad};
  }

  inline Linalg::Matrix<InputType> &recent_inp() { return self._inp; }

  inline void clear_grad() {
    Linalg::fill(self._weight_grad, _get0<WeightGradType>);
    Linalg::fill(self._bias_grad, _get0<BiasType>);
  }

  template <typename NumberType>
  inline static NumberType get_rand(const std::size_t idx) {

    return _getr<NumberType>(idx) + std::cos(idx ^ ((idx >> 2) - 3));
  }

  inline GenericPtr &weight_additionals() {
    return self._weight_additionals;
  }
  inline GenericPtr &bias_additionals() { return self._bias_additionals; }
};

} // namespace NeuralNet

#endif