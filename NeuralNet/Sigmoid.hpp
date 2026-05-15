#ifndef SIGMOID_HPP
#define SIGMOID_HPP
#include "Linear.hpp"
#include "LinearAlgebra/LinalgAppliabels.hpp"
#include "LinearAlgebra/LinalgIndexers.hpp"
#include "LinearAlgebra/LinalgZippables.hpp"
#include "LinearAlgebra/Matrix.hpp"

namespace NeuralNet {

template <typename InputType = float> class Sigmoid {
private:
  Linalg::Matrix<InputType> _inp_buff_1;
  Linalg::Appliables::Multiply<InputType> _func_mul;
  Linalg::Appliables::Exp<InputType> _func_exp;
  Linalg::Zipables::Multiply<InputType, InputType, InputType>
      _func_handman_prod;
  Mode _curr_mode = Mode::Training;

  inline static void _sig_grad_func(InputType &value) {
    value = value * (1 - value);
  }

public:
  inline void init(const Linalg::NumPair &inp_shape,
                   const Mode init_mode = Mode::Training) {
    self._inp_buff_1 = Linalg::createMatrix<InputType>(inp_shape.first,
                                                       inp_shape.second, 0x001);
    self._curr_mode = init_mode;

    self._func_mul.setValue(-1);
  }

  inline Linalg::Matrix<InputType> forward(Linalg::Matrix<InputType> &x) {

    Linalg::copy_if_allocated(x, self._inp_buff_1, 0x004);

    // This is the formula part:
    // res[i, j] = max(0, inp[i, j])
    Linalg::map(self._inp_buff_1, self._func_mul);
    Linalg::map(self._inp_buff_1, self._func_exp);
    return self._inp_buff_1;
  }

  inline Linalg::Matrix<InputType> backward(Linalg::Matrix<InputType> &grad) {
    // Eval Mode: ERROR!
    if (self._curr_mode == Mode::Evaluation)
      throw std::runtime_error("In [Mode::Evaluation] phase!");

    // Get the mask :
    // dOut[i,j]/dinp[i,j] = 1 if inp[i, j] > 0 else 0
    Linalg::map(self._inp_buff_1, self._sig_grad_func);

    // Use handman product to get the result for next layer .
    // By chain rule : out = grad_in * mask
    Linalg::zip_no_accum(
        grad, self._inp_buff_1, self._inp_buff_1, self._func_handman_prod);

    return self._inp_buff_1;
  }
  inline void set_mode(const Mode m = Mode::Training) { self._curr_mode = m; }
};

} // namespace NeuralNet

#endif