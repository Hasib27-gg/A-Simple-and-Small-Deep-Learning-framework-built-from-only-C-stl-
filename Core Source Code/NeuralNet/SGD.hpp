#include "LinearAlgebra/Matrix.hpp"
#ifndef SGD_HPP
#define SGD_HPP
#include "Linear.hpp"
#include "LinearAlgebra/Linalg.hpp"

namespace NeuralNet {

template <typename LearningRateType = float> class SGD {

  template <typename NumTy1, typename NumTy2, typename NumTy3> class SGDUpdate {
  private:
    LearningRateType lr = 1e-2;

  public:
    inline void init(const LearningRateType lr = 1e-2) { self.lr = lr; }

    inline NumTy3 operator()(const NumTy1 num1, const NumTy2 num2) {
      return num1 - self.lr * num2;
    }
  };
  LearningRateType lr = 1e-2;

public:
  inline void init(const LearningRateType lr = 1e-2) { self.lr = lr; }
  template <typename MatTy1, typename MatTy2>
  inline void
  update(std::pair<Linalg::Matrix<MatTy1> &, Linalg::Matrix<MatTy2> &> mats,
         GenericPtr &buff) {

    SGDUpdate<MatTy1, MatTy2, MatTy1> func;
    func.init(self.lr);
    Linalg::zip_no_accum(mats.first, mats.second, mats.first, func,
                         Linalg::Indexers::defaultIndexer,
                         Linalg::Indexers::defaultIndexer);
  }


  inline void
  set_lr(const LearningRateType new_lr){
    self.lr = new_lr;
  }
};
} // namespace NeuralNet

#endif