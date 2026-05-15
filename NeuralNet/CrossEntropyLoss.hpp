
#ifndef CROSS_ENTROPY_LOSS
#define CROSS_ENTROPY_LOSS


#include "Linear.hpp"
#include "LinearAlgebra/Linalg.hpp"
#include "LinearAlgebra/LinalgAppliabels.hpp"
#include "LinearAlgebra/LinalgIndexers.hpp"
#include "LinearAlgebra/LinalgZippables.hpp"
#include "LinearAlgebra/Matrix.hpp"
#include <stdexcept>

namespace NeuralNet {

    template<
    typename PredsType = float, 
    typename TargetsType = float,
    typename OutputType = float,
    typename OutputGradType = float>
class CrossEntropyLoss{
    private:

    Linalg::Matrix<PredsType> _preds_buff;
    Linalg::Matrix<PredsType> _max_buff;
    Linalg::Matrix<PredsType> _preds_edit_buff;
    Linalg::Matrix<OutputGradType> _grad_buff_1; 
    Linalg::Matrix<OutputGradType> _grad_buff_2; 
    Linalg::Matrix<TargetsType> _targets_buff;
    Linalg::Matrix<TargetsType> _targets_edit_buff;
    Linalg::Matrix<OutputType> _out_buff;

    Linalg::Zipables::Subtract<PredsType, PredsType, PredsType> _sub_func;
    Linalg::Zipables::Subtract<OutputGradType, TargetsType, OutputGradType> _sub_func_2;

    Linalg::Zipables::Divide<OutputGradType, OutputGradType, OutputGradType> _func_div;
    Linalg::Reductions::Sum<PredsType, OutputType> _sum_reduc;
    Linalg::Reductions::Sum<TargetsType, OutputType> _sum_reduc_2;
    Linalg::Reductions::Max<PredsType, PredsType> _max_finder;

    Linalg::Appliables::Divide<OutputGradType> _func_div_2;
    Linalg::Appliables::Exp<PredsType> _exp_func;
    Linalg::Zipables::Multiply<PredsType, TargetsType, TargetsType> _handman_prod;
    Linalg::Indexers::ColumnRepeater _idxer;

    NeuralNet::Mode _curr_mode = Mode::Training;
    
    class _SMOOTHER{
        public:
        PredsType _smoothing = 0.1;
        std::size_t _num_classes;
        inline void operator()(TargetsType& val){
            val = val * (1 - self._smoothing) + (self._smoothing / static_cast<TargetsType>(self._num_classes));
        }
    };
    _SMOOTHER _sm_func;


    template<typename NumTy1, typename NumTy2, typename NumTy3>
    inline static NumTy3 _neg_mul(const NumTy1 v1, const NumTy2 v2){
        return static_cast<NumTy3>(v1) * static_cast<NumTy3>(v2) * (-1);
    }

    template<typename NumTy1, typename NumTy2, typename NumTy3>
    inline static NumTy3 _logFirst_add_kernel(const NumTy1 v1, const NumTy2 v2){
        return std::log(static_cast<NumTy3>(v1)) + static_cast<NumTy3>(v2);
    }
    public:

    inline void init(const Linalg::NumPair& inp_shape, 
                    const Mode init_mode = Mode::Training,
                   OutputType smoothing = 0.1){
        self._preds_buff = Linalg::createMatrix<PredsType>(inp_shape.first, inp_shape.second, 1);
        self._preds_edit_buff = Linalg::createMatrix<PredsType>(inp_shape.first, inp_shape.second, 1);


        self._targets_buff = Linalg::createMatrix<TargetsType>(inp_shape.first, inp_shape.second, 2);
        self._targets_edit_buff = Linalg::createMatrix<TargetsType>(inp_shape.first, inp_shape.second, 2);


        self._out_buff = Linalg::createMatrix<OutputType>(inp_shape.first, 1, 3);
        self._grad_buff_1= Linalg::createMatrix<OutputType>(inp_shape.first, inp_shape.second, 3);
        self._grad_buff_2 = Linalg::createMatrix<OutputType>(inp_shape.first, 1, 3);


        self._max_buff = Linalg::createMatrix<PredsType>(inp_shape.first, 1, 1);

        self._curr_mode = init_mode;
        self._sm_func._smoothing = smoothing;
        self._sm_func._num_classes = inp_shape.second;

        _idxer.setColumnSize(inp_shape.second);
    }


    Linalg::Matrix<OutputType> forward(
        Linalg::Matrix<PredsType>& preds, 
        Linalg::Matrix<TargetsType>& targets
     ){

        if(self._curr_mode == Mode::Training) 
        {
            Linalg::copy_if_allocated(preds , self._preds_buff, 1);
            Linalg::copy_if_allocated(targets , self._targets_buff,2 );
        }
        Linalg::copy_if_allocated(preds, self._preds_edit_buff, 0x002);

        Linalg::reduce_no_accum(
            self._preds_edit_buff,
            self._max_buff,
            self._max_finder,
            1
        );
        Linalg::zip_no_accum(
            self._preds_edit_buff,
            self._max_buff,
            self._preds_edit_buff,
            self._sub_func,
            Linalg::Indexers::defaultIndexer,
            self._idxer
        );

        Linalg::map(self._preds_edit_buff, self._exp_func);

        Linalg::copy_if_allocated(self._preds_edit_buff, self._grad_buff_1, 0x002);
        
        Linalg::reduce_no_accum(
            self._preds_edit_buff,
            self._out_buff,
            self._sum_reduc,
            1
        );

        Linalg::copy_if_allocated(self._out_buff, self._grad_buff_2, 0x003);

        Linalg::zip_no_accum(
            self._out_buff, 
            self._max_buff, 
            self._out_buff,
            self._logFirst_add_kernel<OutputType, PredsType, OutputType>
        );


        Linalg::copy_if_allocated(targets, self._targets_edit_buff, 2);
        Linalg::map(self._targets_edit_buff, self._sm_func);
        Linalg::zip_no_accum(
            preds,
            self._targets_edit_buff,
            self._targets_edit_buff,
            self._neg_mul<PredsType, TargetsType, TargetsType>
        );

        Linalg::reduce_with_accum(
            self._targets_edit_buff,
            self._out_buff,
            self._sum_reduc_2,
            1
        );

        return self._out_buff;
     }

     Linalg::Matrix<OutputGradType> backward(){
        if(self._curr_mode == Mode::Evaluation)
          throw std::runtime_error("In [Mode::Evaluation] phase!");

        Linalg::zip_no_accum(
            self._grad_buff_1,
            self._grad_buff_2,
            self._grad_buff_1,
            self._func_div,
            Linalg::Indexers::defaultIndexer,
            self._idxer
        );
    
        Linalg::map(self._targets_buff, self._sm_func);
        Linalg::zip_with_accum(
            self._grad_buff_1,
            self._targets_buff,
            self._grad_buff_1,
            self._sub_func_2
        );

        self._func_div_2.setValue(self._grad_buff_1.getRowSize());
        Linalg::map(self._grad_buff_1, self._func_div_2);
        return self._grad_buff_1;
     }



     inline void set_mode(const Mode m = Mode::Training){
        self._curr_mode = m;
     }
};
}


#endif