#ifndef BCELWL_HPP
#define BCELWL_HPP

#include "Linear.hpp"
#include "LinearAlgebra/Linalg.hpp"
#include "LinearAlgebra/LinalgAppliabels.hpp"
#include "LinearAlgebra/LinalgIndexers.hpp"
#include "LinearAlgebra/Matrix.hpp"

namespace NeuralNet {

template <typename PredsType = float, typename TargetsType = float,
          typename OutputType = float, typename OutputGradType = float>

class BCELoss {
private:
  Linalg::Matrix<PredsType> _preds;
  Linalg::Matrix<TargetsType> _targets;
  Linalg::Matrix<OutputType> _buff_1;
  Linalg::Matrix<OutputGradType> _buff_2;

  Mode _curr_mode = Mode::Training;

  template <typename NumTy1, typename NumTy2, typename NumTy3>
  inline static NumTy3 _bce_loss_wl_kernel(const NumTy1 a, const NumTy2 b) {
    return static_cast<NumTy3>(std::max(a, 0.0f) - (a * b) +
                               std::log(1.0f + std::exp(-std::abs(a))));
  }

  template <typename NumTy1, typename NumTy2, typename NumTy3>
  inline static NumTy3 _bce_loss_wl_grad_kernel(const NumTy1 a,
                                                const NumTy2 b) {
    return static_cast<NumTy3>(1 / (1 + std::exp(-a)) - b);
  }

public:
  inline void init(const Linalg::NumPair &inp_shape,
                   const Mode init_mode = Mode::Training) {
    self._curr_mode = init_mode;

    self._preds = Linalg::createMatrix<PredsType>(inp_shape.first,
                                                  inp_shape.second, 0x001);
    self._buff_1 = Linalg::createMatrix<OutputType>(inp_shape.first,
                                                    inp_shape.second, 0x001);
    self._buff_2 = Linalg::createMatrix<OutputGradType>(
        inp_shape.first, inp_shape.second, 0x001);
    self._targets = Linalg::createMatrix<TargetsType>(inp_shape.first,
                                                      inp_shape.second, 0x001);
  }

  inline Linalg::Matrix<OutputType>
  forward(Linalg::Matrix<PredsType> &preds,
          Linalg::Matrix<TargetsType> &targets) {

    if (self._curr_mode == Mode::Training) {
      Linalg::copy_if_allocated(preds, self._preds, 0x02);
      Linalg::copy_if_allocated(targets, self._targets, 0x02);
    }

    Linalg::zip_no_accum(
        preds, targets, self._buff_1,
        self._bce_loss_wl_kernel<PredsType, TargetsType, OutputType>,
        Linalg::Indexers::defaultIndexer, Linalg::Indexers::defaultIndexer);

    return self._buff_1;
  }

  inline Linalg::Matrix<OutputGradType> backward() {
    if (self._curr_mode == Mode::Evaluation)
      throw std::runtime_error("In [Mode::Evaluation] phase!");

    Linalg::zip_no_accum(
        self._preds, self._targets, self._buff_2,
        self._bce_loss_wl_grad_kernel<PredsType, TargetsType, OutputGradType>,
        Linalg::Indexers::defaultIndexer, Linalg::Indexers::defaultIndexer);

    return self._buff_2;
  }

  inline Linalg::Matrix<PredsType> last_preds() { return self._preds; }
  inline Linalg::Matrix<PredsType> last_targets() { return self._targets; }
  inline void set_mode(const Mode m = Mode::Training) { self._curr_mode = m; }
};
} // namespace NeuralNet
#endif