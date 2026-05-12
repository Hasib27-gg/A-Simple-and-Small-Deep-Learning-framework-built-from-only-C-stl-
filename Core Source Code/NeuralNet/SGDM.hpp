#ifndef SGD_WITH_MOMENTUM_HPP
#define SGD_WITH_MOMENTUM_HPP

#include "Linear.hpp"
#include "LinearAlgebra/Linalg.hpp"
#include "LinearAlgebra/LinalgIndexers.hpp"
#include "LinearAlgebra/Matrix.hpp"
#include <cstddef>
#include <stdexcept>

namespace NeuralNet {

template <typename LearningRateType = float, typename MomentumType = float>
class SGDM {
  public:
  template <typename NumberType>
  using MatrixPointer = Linalg::Matrix<NumberType> *;

  template <typename NumTy1, typename NumTy2, typename NumTy3>
  class _MomentumUpdate {
    public:
    MomentumType rate = 0.9;


    inline NumTy3 operator()(const NumTy1 num1, const NumTy2 num2) {
      return self.rate * num1 + (1 - self.rate) * num2;
    }
  };

  template <typename NumTy1, typename NumTy2, typename NumTy3>
  class _WeightUpdate {
  public:
    LearningRateType lr = 1e-2;


    inline NumTy3 operator()(const NumTy1 num1, const NumTy2 num2) {
      return num1 - self.lr * num2;
    }
  };

  LearningRateType lr = 1e-2;
  MomentumType momentum_rate = 0.9;

public:
  inline void init(const LearningRateType lr = 1e-2,
                   const MomentumType momentum_rate = 0.9) {
    self.lr = lr;
    self.momentum_rate = momentum_rate;
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

    _MomentumUpdate<MomentumType, MatTy2, MomentumType> momentum_updater;

    momentum_updater.rate = self.momentum_rate;
    Linalg::Matrix<MomentumType> &buff_ref =
        *static_cast<MatrixPointer<MomentumType>>(buff);

    Linalg::zip_no_accum(buff_ref,mats.second, buff_ref, momentum_updater,
                         Linalg::Indexers::defaultIndexer,
                         Linalg::Indexers::defaultIndexer);

    _WeightUpdate<MatTy1, MomentumType, MatTy1> weight_updater;
    weight_updater.lr = self.lr;
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

  inline void
  set_lr(const LearningRateType new_lr){
    self.lr = new_lr;
  }
  inline void
  set_momentum_rate(const LearningRateType new_rate){
    self.momentum_rate = new_rate;
  }
};
} // namespace NeuralNet

#endif
