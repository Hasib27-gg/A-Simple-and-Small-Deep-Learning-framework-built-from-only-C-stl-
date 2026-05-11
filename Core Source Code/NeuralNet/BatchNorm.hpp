#ifndef BATCH_NORM_HPP
#define BATCH_NORM_HPP
#include "Linear.hpp"
#include "LinearAlgebra/LinalgAppliabels.hpp"
#include "LinearAlgebra/LinalgIndexers.hpp"
#include "LinearAlgebra/LinalgReductions.hpp"
#include "LinearAlgebra/LinalgZippables.hpp"
#include "LinearAlgebra/Matrix.hpp"

namespace NeuralNet {

template <typename GammaType = float, typename BetaType = float,
          typename GammaGradType = float, typename BetaGradType = float,
          typename InputType = float, typename OutputType = float,
          typename InputGradType = float, typename OutputGradType = float>
class BatchNorm {
private:
  Linalg::Matrix<InputType> _mean_res_buff;
  Linalg::Matrix<InputType> _std_res_buff;
  Linalg::Matrix<InputType> _norm_buff;
  Linalg::Matrix<OutputType> _out_buff;
  Linalg::Matrix<InputGradType> _grad_buff_1;
  Linalg::Matrix<InputGradType> _grad_buff_2;
  Linalg::Matrix<OutputGradType> _grad_buff_3;

  Linalg::Matrix<GammaType> _gamma;
  Linalg::Matrix<GammaGradType> _gamma_grad;
  Linalg::Matrix<BetaType> _beta;
  Linalg::Matrix<BetaGradType> _beta_grad;

  Linalg::Reductions::Stdev<InputType, InputType> _func_std;
  Linalg::Reductions::Mean<InputType, InputType> _func_mean;

  Linalg::Reductions::Sum<InputGradType, GammaGradType> _func_sum_1;
  Linalg::Reductions::Sum<InputGradType, BetaGradType> _func_sum_2;

  Linalg::Zipables::Add<OutputType, BetaType, OutputType> _func_add;
  Linalg::Zipables::Add<OutputGradType, GammaGradType, OutputGradType>
      _func_add_2;
  Linalg::Zipables::Subtract<InputGradType, OutputGradType, OutputGradType>
      _func_sub_2;

  Linalg::Zipables::Multiply<InputType, GammaType, OutputType> _func_mul;
  Linalg::Zipables::Multiply<InputGradType, InputType, InputGradType>
      _func_mul_2;
  Linalg::Zipables::Multiply<InputType, GammaGradType, OutputGradType>
      _func_mul_3;
  Linalg::Zipables::Multiply<OutputGradType, GammaType, OutputGradType>
      _func_mul_4;
  Linalg::Zipables::Subtract<InputType, InputType, InputType> _func_sub;
  Linalg::Zipables::Divide<InputType, InputType, InputType> _func_div;

  Linalg::Appliables::Multiply<InputGradType> _grad_scaler;

  Linalg::Indexers::RowRepeater _idxer;

  Mode _curr_mode = Mode::Training;
  GenericPtr _gamma_additionals = nullptr;
  GenericPtr _beta_additionals = nullptr;
  template <typename NumTy> inline static NumTy _get0(const std::size_t) {
    return 0;
  }

  template <typename NumTy> inline static NumTy _get1(const std::size_t idx) {
    return 1;
  }

  template <typename NumTy1, typename NumTy2, typename NumTy3>
  class _SCALED_DIV {
  public:
    std::size_t _batch_size;
    inline NumTy3 operator()(const NumTy1 a, const NumTy2 b) {
      return static_cast<NumTy3>(a) /
             (static_cast<NumTy3>(b * self._batch_size));
    }
  };
  _SCALED_DIV<OutputGradType, InputType, OutputGradType> _scaled_div;

public:
  template <typename GammaInitTy = decltype(_get1<GammaType>),
            typename GammaGradInitTy = decltype(_get0<GammaGradType>),
            typename BetaInitTy = decltype(_get0<BetaType>),
            typename BetaGradInitTy = decltype(_get0<BetaGradType>)>
  inline void
  init(const Linalg::NumPair &inp_shape, GammaInitTy &gamma_init = _get1,
       GammaGradInitTy &gamma_grad_init = _get0, BetaInitTy &beta_init = _get0,
       BetaGradInitTy &beta_grad_init = _get0,
       const Mode init_mode = Mode::Training) {
    self._curr_mode = init_mode;

    self._mean_res_buff =
        Linalg::createMatrix<InputType>(1, inp_shape.second, 1);
    self._std_res_buff =
        Linalg::createMatrix<InputType>(1, inp_shape.second, 1);

    self._norm_buff =
        Linalg::createMatrix<InputType>(inp_shape.first, inp_shape.second, 1);
    self._out_buff =
        Linalg::createMatrix<OutputType>(inp_shape.first, inp_shape.second, 1);

    self._gamma = Linalg::createMatrix<GammaType>(1, inp_shape.second, 1);
    self._gamma_grad =
        Linalg::createMatrix<GammaGradType>(1, inp_shape.second, 1);

    self._beta = Linalg::createMatrix<BetaType>(1, inp_shape.second, 1);
    self._beta_grad =
        Linalg::createMatrix<BetaGradType>(1, inp_shape.second, 1);

    self._grad_buff_1 = Linalg::createMatrix<InputGradType>(
        inp_shape.first, inp_shape.second, 1);
    self._grad_buff_2 = Linalg::createMatrix<InputGradType>(
        inp_shape.first, inp_shape.second, 1);
    self._grad_buff_3 = Linalg::createMatrix<OutputGradType>(
        inp_shape.first, inp_shape.second, 1);

    Linalg::fill(self._gamma, gamma_init);
    Linalg::fill(self._gamma_grad, gamma_grad_init);

    Linalg::fill(self._beta, beta_init);
    Linalg::fill(self._beta_grad, beta_grad_init);

    _idxer.setColumnSize(inp_shape.second);
    _grad_scaler.setValue(inp_shape.first 
    );

    self._scaled_div._batch_size = inp_shape.first;
  }

  Linalg::Matrix<InputType> forward(Linalg::Matrix<InputType> &x) {
    Linalg::transpose(x);
    Linalg::reduce_no_accum(x, self._std_res_buff, self._func_std, 1);
    Linalg::reduce_no_accum(x, self._mean_res_buff, self._func_mean, 1);
    Linalg::transpose(x);

    Linalg::copy_if_allocated(x, self._norm_buff, 0x004);
    Linalg::zip_no_accum(self._norm_buff, self._mean_res_buff, self._norm_buff,
                         self._func_sub, Linalg::Indexers::defaultIndexer,
                         self._idxer);

    Linalg::zip_no_accum(self._norm_buff, self._std_res_buff, self._norm_buff,
                         self._func_div, Linalg::Indexers::defaultIndexer,
                         self._idxer);

    Linalg::zip_no_accum(self._norm_buff, self._gamma, self._out_buff,
                         self._func_mul, Linalg::Indexers::defaultIndexer,
                         self._idxer);

    Linalg::zip_no_accum(self._out_buff, self._beta, self._out_buff,
                         self._func_add, Linalg::Indexers::defaultIndexer,
                         self._idxer);

    return self._out_buff;
  }

  Linalg::Matrix<OutputGradType> backward(Linalg::Matrix<InputGradType> &grad) {
    if (self._curr_mode == Mode::Evaluation)
      throw std::runtime_error("In [Mode::Evaluation] phase!");

    Linalg::zip_no_accum(grad, self._norm_buff, self._grad_buff_1,
                         self._func_mul_2, Linalg::Indexers::defaultIndexer,
                         Linalg::Indexers::defaultIndexer);

    Linalg::reduce_with_accum(self._grad_buff_1, self._gamma_grad,
                              self._func_sum_1, 0);
            
    Linalg::reduce_with_accum(grad, self._beta_grad, self._func_sum_2, 0);


    Linalg::copy_if_allocated(grad, self._grad_buff_2, 0x003);

    Linalg::map(self._grad_buff_2, self._grad_scaler);

    Linalg::zip_no_accum(self._norm_buff, self._gamma_grad, self._grad_buff_3,
                         self._func_mul_3, Linalg::Indexers::defaultIndexer,
                         self._idxer);

    Linalg::zip_with_accum(self._grad_buff_3, self._beta_grad,
                           self._grad_buff_3, self._func_add_2,
                           Linalg::Indexers::defaultIndexer, self._idxer);

    Linalg::zip_no_accum(self._grad_buff_2, self._grad_buff_3,
                         self._grad_buff_3, self._func_sub_2,
                         Linalg::Indexers::defaultIndexer,
                         Linalg::Indexers::defaultIndexer);

    Linalg::zip_no_accum(self._grad_buff_3, self._gamma, self._grad_buff_3,
                         self._func_mul_4, Linalg::Indexers::defaultIndexer,
                         self._idxer);
    std::size_t batch_size = grad.getRowSize();
    Linalg::zip_no_accum(self._grad_buff_3, self._std_res_buff,
                         self._grad_buff_3, self._scaled_div,
                         Linalg::Indexers::defaultIndexer, self._idxer);
    return self._grad_buff_3;
  }

  template <typename OptimizerType> inline void step(OptimizerType &optimizer) {
    optimizer.update(
        std::pair<Linalg::Matrix<GammaType> &, Linalg::Matrix<GammaGradType> &>(
            self._gamma, self._gamma_grad),
        self._gamma_additionals);
    optimizer.update(
        std::pair<Linalg::Matrix<BetaType> &, Linalg::Matrix<BetaGradType> &>(
            self._beta, self._beta_grad),
        self._beta_additionals);
  }

  inline std::pair<Linalg::Matrix<GammaType> &, Linalg::Matrix<GammaGradType> &>
  gamma_pair() {
    return {self._gamma, self._gamma_grad};
  }
  inline std::pair<Linalg::Matrix<BetaType> &, Linalg::Matrix<BetaGradType> &>
  beta_pair() {
    return {self._beta, self._beta_grad};
  }

  inline Linalg::Matrix<InputType> last_stdev() { return self._std_res_buff; }
  inline Linalg::Matrix<InputType> last_mean() { return self._mean_res_buff; }

  inline void set_mode(const Mode new_mode = Mode::Training) {
    self._curr_mode = new_mode;
  }

  inline GenericPtr &beta_additionals() { return self._beta_additionals; }
  inline GenericPtr &gamma_additionals() { return self._gamma_additionals; }

  inline void clear_grad() {
    Linalg::fill(self._gamma_grad, _get0<GammaGradType>);
    Linalg::fill(self._beta_grad, _get0<BetaGradType>);
  }
};
} // namespace NeuralNet

#endif
