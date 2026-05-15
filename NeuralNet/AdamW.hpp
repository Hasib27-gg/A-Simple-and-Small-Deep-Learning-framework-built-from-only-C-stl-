#ifndef ADAMW_MOMENTUM_HPP
#define ADAMW_MOMENTUM_HPP

#include "Linear.hpp"
#include "LinearAlgebra/Linalg.hpp"
#include "LinearAlgebra/LinalgIndexers.hpp"
#include "LinearAlgebra/Matrix.hpp"
#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace NeuralNet {

template <
typename LearningRateType = float, 
typename MomentumType = float,
typename VelocityType = float,
typename WeightDecayType = float>
class AdamW {


  constexpr static std::size_t ADAMW_STATE_BUFF = 0x120380;
  struct _AdamWStateBuff{
    Linalg::Matrix<MomentumType> momentum_buff;
    Linalg::Matrix<VelocityType> velocity_buff;
    std::size_t time_step;
  };


  template <typename NumTy1, typename NumTy2, typename NumTy3>
  class _MomentumUpdate {
    public:
    MomentumType rate= 0.9;

    inline NumTy3 operator()(const NumTy1 num1, const NumTy2 num2) const noexcept {
      return self.rate * num1 + (1 - self.rate) * num2;
    }
  };


  template <typename NumTy1, typename NumTy2, typename NumTy3>
  class _VelocityUpdate {
  public:
    VelocityType rate = 0.9;

  
    inline void init(const MomentumType rate = 0.9)  {
      self.rate = rate;
    }

    inline NumTy3 operator()(const NumTy1 num1, const NumTy2 num2) const noexcept {
      return self.rate * num1 + (1 - self.rate) * num2 * num2;
    }
  };



  template <typename NumTy1, typename NumTy2, typename NumTy3>
  class _MOMENTUM_DIV_SQRT_VL{


    public:

    LearningRateType lr = 1e-3;
    

    inline NumTy3 operator() (
      const NumTy1 v1,
      const NumTy2 v2
    )
    const noexcept{
      return -self.lr * v1/(std::sqrt(v2) + 1e-6);
    }
  };

  template<typename NumTy>
  class _WEIGHT_DECAY_UPDATE{
    public:
    LearningRateType lr = 1e-3;
    LearningRateType wd = 1e-3;
    public:
    inline void operator() (NumTy& val){
      val = (1 - lr * wd)*val;
    }
  }; 
  
public:
  LearningRateType lr = 1e-3;
  MomentumType beta1 = 0.9;
  MomentumType beta2 = 0.999;
  WeightDecayType wdecay = 1e-3;

  inline void init(const LearningRateType lr = 1e-2,
                   const MomentumType beta1 = 0.9, 
                   const MomentumType beta2 = 0.999,
                   const WeightDecayType wdecay = 1e-3) {
    self.lr = lr;
    self.beta1 = beta1;
    self.beta2 = beta2;
    self.wdecay = wdecay;
  }


  template <typename MatTy1, typename MatTy2>
  inline void
  update(std::pair<Linalg::Matrix<MatTy1> &, Linalg::Matrix<MatTy2> &> mats,
         GenericPtr &buff) {
    Linalg::NumPair shape_1 = {mats.first.getRowSize(), mats.first.getColumnSize()};

    
    if (buff == nullptr) {
  
      _AdamWStateBuff* ptr;
      ptr = new _AdamWStateBuff;
      
      ptr->momentum_buff.setRowSize(shape_1.first);
      ptr->momentum_buff.setColumnSize(shape_1.second);
      ptr->momentum_buff.setObjectId(ADAMW_STATE_BUFF | 0x00001);
      ptr->momentum_buff.getData().resize(mats.first.getData().size(), 0);

      ptr->velocity_buff.setRowSize(shape_1.first);
      ptr->velocity_buff.setColumnSize(shape_1.second);
      ptr->momentum_buff.setObjectId(ADAMW_STATE_BUFF | 0x00002);
      ptr->velocity_buff.getData().resize(mats.first.getData().size(), 0);

      buff = static_cast<GenericPtr>(ptr);
    }

    
    _MomentumUpdate<MomentumType, MatTy2, MomentumType> momentum_updater;
    _VelocityUpdate<VelocityType, MatTy2, VelocityType> velocity_updater;

    momentum_updater.rate = self.beta1;
    velocity_updater.rate = self.beta2;

    
    _AdamWStateBuff* buff_ref = static_cast<_AdamWStateBuff*>(buff);

    Linalg::zip_no_accum_static(buff_ref->momentum_buff, mats.second, buff_ref->momentum_buff, momentum_updater);
    Linalg::zip_no_accum_static(buff_ref->velocity_buff, mats.second, buff_ref->velocity_buff, velocity_updater);
   
    
    _MOMENTUM_DIV_SQRT_VL<MomentumType,VelocityType, MatTy1> comb_func;
    comb_func.lr = self.lr;

    Linalg::zip_with_accum_static(
      buff_ref->momentum_buff,
      buff_ref->velocity_buff,
      mats.first,
      comb_func
    );

    
    _WEIGHT_DECAY_UPDATE<MatTy1> w_decay_func;
    w_decay_func.lr = self.lr;
    w_decay_func.wd = self.wdecay;
    
    Linalg::map(mats.first, w_decay_func);
  }

  inline void clear_buffer(GenericPtr &ptr) {
     _AdamWStateBuff* buff_ptr =
        static_cast<_AdamWStateBuff*>(ptr);
    delete buff_ptr;
    ptr = nullptr;
  }

  inline _AdamWStateBuff& get_buffer(GenericPtr &ptr) {
    _AdamWStateBuff*  buff_ptr =
        static_cast<_AdamWStateBuff*>(ptr);
    return *buff_ptr;
  }


};
} // namespace NeuralNet

#endif
