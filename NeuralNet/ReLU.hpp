#ifndef RELU_HPP
#define RELU_HPP
#include "Linear.hpp"
#include "LinearAlgebra/LinalgIndexers.hpp"
#include "LinearAlgebra/LinalgZippables.hpp"
#include "LinearAlgebra/Matrix.hpp"

namespace NeuralNet {

template <typename InputType = float> class ReLU {
private:
  Linalg::Matrix<InputType> _inp_buff_1;
  Linalg::Matrix<InputType> _inp_buff_2;
  Linalg::Appliables::Max<InputType> _func_max;
  Linalg::Appliables::Greater<InputType> _func_greater;
  Linalg::Zipables::Multiply<InputType, InputType, InputType>
      _func_handman_prod;
  Mode _curr_mode = Mode::Training;

public:
  inline void init(const Linalg::NumPair &inp_shape,
                   const Mode init_mode = Mode::Training) {
    self._inp_buff_1 = Linalg::createMatrix<InputType>(inp_shape.first,
                                                       inp_shape.second, 0x001);
    self._inp_buff_2 = Linalg::createMatrix<InputType>(inp_shape.first,
                                                       inp_shape.second, 0x001);
    self._curr_mode = init_mode;

    self._func_max.setValue(0);
    self._func_greater.setValue(0);
  }

  inline Linalg::Matrix<InputType> forward(Linalg::Matrix<InputType> &x) {
    // if training mode, then store current input into an input buffer
    if (self._curr_mode == Mode::Training)
      Linalg::copy_if_allocated(x, self._inp_buff_2, 0x003);

    // Copy to an buffer where we will modify the values
    Linalg::copy_if_allocated(x, self._inp_buff_1, 0x004);

    // This is the formula part:
    // res[i, j] = max(0, inp[i, j])
    Linalg::map(self._inp_buff_1, self._func_max);
    return self._inp_buff_1;
  }

  inline Linalg::Matrix<InputType> backward(Linalg::Matrix<InputType> &grad) {
    // Eval Mode: ERROR!
    if (self._curr_mode == Mode::Evaluation)
      throw std::runtime_error("In [Mode::Evaluation] phase!");

    // Get the mask :
    // dOut[i,j]/dinp[i,j] = 1 if inp[i, j] > 0 else 0
    Linalg::map(self._inp_buff_2, self._func_greater);

    // Use handman product to get the result for next layer .
    // By chain rule : out = grad_in * mask
    Linalg::zip_no_accum(
        grad, self._inp_buff_2, self._inp_buff_2, self._func_handman_prod);

    return self._inp_buff_2;
  }

  inline Linalg::Matrix<InputType> last_input() { return self._inp_buff_2; }

  inline void set_mode(const Mode m = Mode::Training) { self._curr_mode = m; }
};
} // namespace NeuralNet

#endif