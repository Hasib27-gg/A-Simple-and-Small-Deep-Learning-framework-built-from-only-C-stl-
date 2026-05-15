#ifndef MSE_HPP
#define MSE_HPP


#include "Linear.hpp"
#include "LinearAlgebra/Linalg.hpp"
#include "LinearAlgebra/LinalgAppliabels.hpp"
#include "LinearAlgebra/Matrix.hpp"
#include <stdexcept>

namespace NeuralNet {

    template<
    typename PredsType = float, 
    typename TargetsType = float,
    typename OutputType = float,
    typename OutputGradType = float>
class MSE{
    private:

    Linalg::Matrix<PredsType> _preds_buff;
    Linalg::Matrix<PredsType> _preds_edit_buff;
    Linalg::Matrix<TargetsType> _targets_buff;
    Linalg::Matrix<OutputType> _out_buff;
    Linalg::Matrix<OutputGradType> _out_grad_buff;
    Linalg::Zipables::Subtract<PredsType, TargetsType, PredsType> _sub_func;
    Linalg::Zipables::Subtract<PredsType, TargetsType, OutputGradType> _sub_func_2;
    Linalg::Appliables::Divide<OutputGradType> _sub_div;
    Linalg::Reductions::Mean<PredsType, OutputType> _mean_reduc;
    Linalg::Appliables::Square<OutputType> _sqr_func;
    NeuralNet::Mode _curr_mode = Mode::Training;
    public:


    inline void init(const Linalg::NumPair& inp_shape, 
                    const Mode init_mode = Mode::Training){
        self._preds_buff = Linalg::createMatrix<PredsType>(inp_shape.first, inp_shape.second, 1);
        self._preds_edit_buff = Linalg::createMatrix<PredsType>(inp_shape.first, inp_shape.second, 1);
        self._targets_buff = Linalg::createMatrix<TargetsType>(inp_shape.first, inp_shape.second, 2);
        self._out_buff = Linalg::createMatrix<OutputType>(1, inp_shape.second, 3);
        self._out_grad_buff = Linalg::createMatrix<OutputGradType>(inp_shape.first, inp_shape.second, 1);
        self._curr_mode = init_mode;
        self._sub_div.setValue(inp_shape.first/2);
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
        
        Linalg::zip_no_accum(
            preds,
            targets,
            self._preds_edit_buff, 
            self._sub_func
        );
        
        Linalg::map(
            self._preds_edit_buff,
            self._sqr_func
        );
        
        Linalg::transpose(self._preds_edit_buff);
    
        Linalg::reduce_no_accum(
            self._preds_edit_buff,
            self._out_buff,
            self._mean_reduc,
            1
        );
        Linalg::transpose(self._preds_edit_buff);
        return self._out_buff;
     }

     Linalg::Matrix<OutputGradType> backward(){
        if(self._curr_mode == Mode::Evaluation)
          throw std::runtime_error("In [Mode::Evaluation] phase!");
        Linalg::zip_no_accum(
            self._preds_buff,
            self._targets_buff,
            self._out_grad_buff, 
            self._sub_func_2
        );

        Linalg::map(self._out_grad_buff, self._sub_div);
        return self._out_grad_buff;
     }



     inline void set_mode(const Mode m = Mode::Training){
        self._curr_mode = m;
     }
};
}


#endif