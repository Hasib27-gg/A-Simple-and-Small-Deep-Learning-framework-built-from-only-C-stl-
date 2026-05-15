#ifndef ELU_HPP
#define ELU_HPP
#include "Linear.hpp"
#include "LinearAlgebra/LinalgIndexers.hpp"
#include "LinearAlgebra/LinalgZippables.hpp"
#include "LinearAlgebra/Matrix.hpp"

namespace NeuralNet {

template <typename InputType = float> class ELU {
private:
  Linalg::Matrix<InputType> _inp_buff_1;
  Linalg::Matrix<InputType> _inp_buff_2;
  Linalg::Appliables::Min<InputType> _func_max;
  Linalg::Appliables::Exp<InputType> _func_exp;
  Linalg::Zipables::Multiply<InputType, InputType, InputType>
      _func_handman_prod;
  Mode _curr_mode = Mode::Training;

  struct _ELU_FORWARD {
  public:
    InputType _alpha = 1;
    inline void operator()(InputType &val) const noexcept {
      val = val > 0 ? val : self._alpha * (std::exp(val) - 1);
    }
  };

  struct _ELU_GRAD {
  public:
    InputType _alpha = 1;
    inline void operator()(InputType &val)  const noexcept {
      val = val > 0 ? 1 : self._alpha * std::exp(val);
    }
  };

  _ELU_FORWARD _scaler_func;
  _ELU_GRAD _grad_func;

public:
  inline void init(const Linalg::NumPair &inp_shape,
                   const InputType alpha = 1.0,
                   const Mode init_mode = Mode::Training) {
    self._inp_buff_1 = Linalg::createMatrix<InputType>(inp_shape.first,
                                                       inp_shape.second, 0x001);
    self._inp_buff_2 = Linalg::createMatrix<InputType>(inp_shape.first,
                                                       inp_shape.second, 0x001);
    self._curr_mode = init_mode;
    self._scaler_func._alpha = alpha;
  }

  inline Linalg::Matrix<InputType> forward(Linalg::Matrix<InputType> &x) {
    if (self._curr_mode == Mode::Evaluation)
      Linalg::copy_if_allocated(x, self._inp_buff_2, 0x003);
    Linalg::copy_if_allocated(x, self._inp_buff_1, 0x004);

    Linalg::map(self._inp_buff_1, self._scaler_func);
    return self._inp_buff_1;
  }

  inline Linalg::Matrix<InputType> backward(Linalg::Matrix<InputType> &grad) {
    // Eval Mode: ERROR!
    if (self._curr_mode == Mode::Evaluation)
      throw std::runtime_error("In [Mode::Evaluation] phase!");

    Linalg::map(self._inp_buff_2, self._grad_func);

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