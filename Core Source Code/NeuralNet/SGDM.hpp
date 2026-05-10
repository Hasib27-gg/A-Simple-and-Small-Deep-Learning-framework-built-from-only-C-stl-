#ifndef SGD_WITH_MOMENTUM_HPP
#define SGD_WITH_MOMENTUM_HPP

#include "Linear.hpp"
#include "LinearAlgebra/Linalg.hpp"
#include "LinearAlgebra/LinalgIndexers.hpp"
#include "LinearAlgebra/Matrix.hpp"
#include <cstddef>

namespace NeuralNet {

template <typename LearningRateType = float, typename MomentumType = float>
class SGDM {

  template <typename NumberType>
  using MatrixPointer = Linalg::Matrix<NumberType> *;

  template <typename NumTy1, typename NumTy2, typename NumTy3>
  class _MomentumUpdate {
  private:
    MomentumType momentum = 0.9;

  public:
    inline void init(const MomentumType momentum = 0.9) {
      self.momentum = momentum;
    }

    inline NumTy3 operator()(const NumTy1 num1, const NumTy2 num2) {
      return self.momentum * num1 + (1 - self.momentum) * num2;
    }
  };

  template <typename NumTy1, typename NumTy2, typename NumTy3>
  class _WeightUpdate {
  private:
    LearningRateType lr = 1e-2;

  public:
    inline void init(const LearningRateType lr = 1e-2,
                     const MomentumType momentum = 0.9) {
      self.lr = lr;
    }

    inline NumTy3 operator()(const NumTy1 num1, const NumTy2 num2) {
      return num1 - self.lr * num2;
    }
  };

  LearningRateType lr = 1e-2;
  MomentumType momentum = 0.9;

public:
  inline void init(const LearningRateType lr = 1e-2,
                   const MomentumType momentum = 0.9) {
    self.lr = lr;
    self.momentum = momentum;
  }
  template <typename MatTy1, typename MatTy2>
  inline void
  update(std::pair<Linalg::Matrix<MatTy1> &, Linalg::Matrix<MatTy2> &> mats,
         GenericPtr &buff) {
    if (buff == nullptr) {
      MatrixPointer<MomentumType> ptr;
      ptr = new Linalg::Matrix<MomentumType>;

      ptr->setRowSize(mats.first.getRowSize());
      ptr->setColumnSize(mats.first.getColumnSize());
      ptr->getData().resize(mats.first.getData().size(), 0);

      buff = static_cast<GenericPtr>(ptr);
    }

    _MomentumUpdate<MatTy2, MomentumType, MomentumType> momentum_updater;

    momentum_updater.init(self.momentum);
    Linalg::Matrix<MomentumType> &buff_ref =
        *static_cast<MatrixPointer<MomentumType>>(buff);

    Linalg::zip_no_accum(mats.second,buff_ref, buff_ref, momentum_updater,
                         Linalg::Indexers::defaultIndexer,
                         Linalg::Indexers::defaultIndexer);

    _WeightUpdate<MatTy1, MomentumType, MatTy1> weight_updater;

    Linalg::zip_no_accum(mats.first, buff_ref, mats.first, weight_updater,
                         Linalg::Indexers::defaultIndexer,
                         Linalg::Indexers::defaultIndexer);
  }

  inline void clear_buffer(GenericPtr &ptr) {
    MatrixPointer<MomentumType> buff_ptr =
        static_cast<MatrixPointer<MomentumType>>(ptr);
    delete buff_ptr;
    ptr = nullptr;
  }

  inline Linalg::Matrix<MomentumType> get_buffer(GenericPtr &ptr) {
    MatrixPointer<MomentumType> buff_ptr =
        static_cast<MatrixPointer<MomentumType>>(ptr);
    return *buff_ptr;
  }
};
} // namespace NeuralNet

#endif
