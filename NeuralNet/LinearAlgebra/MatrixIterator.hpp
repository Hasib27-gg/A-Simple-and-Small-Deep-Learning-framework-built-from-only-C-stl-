#ifndef MAT_ITER_2D_HPP
#define MAT_ITER_2D_HPP
#include "Matrix.hpp"


namespace Linalg {
     class IteratorReachedEnd : public std::runtime_error{
        public:
        explicit IteratorReachedEnd(const std::string& message) : std::runtime_error(message){}
    };

    template<typename NumberType>
    class MatrixIterator{
        private:
        Matrix<NumberType>*  _Matrix = nullptr;
        std::size_t            _IdxPointer;
        std::size_t            _ObjId;        
        public:

        inline void initializeIterator(
            Matrix<NumberType>       & target,
            const NumPair              & start,
            const std::size_t            selfId
        ){
            self._Matrix = &target;
            self._IdxPointer = start.first * target.getColumnSize() + start.second;
            self._ObjId =  selfId;
        }

        inline NumberType& operator* (){
#ifndef    DISABLE_ITERATOR_VALIDAION

        if(self._IdxPointer >= self._Matrix->getRowSize() * self._Matrix->getColumnSize())
            throw IteratorReachedEnd("Out of range error!");
#endif
        return self._Matrix->getData().data()[_IdxPointer];
        }

        inline std::size_t getObjectId(){
            return self._ObjId;
        }

        inline NumPair getLocation(){
            NumPair  res;
            res.first = self._IdxPointer / self._Matrix->getColumnSize();
            res.second = self._IdxPointer % self._Matrix->getColumnSize();
            return res;
        }

        inline void moveToNext(){
            self._IdxPointer++;
        }
        inline void moveToPrevious(){
            self._IdxPointer--;
        }

        inline void jumpForward(const size_t jmp_size = 1) noexcept{self._IdxPointer += jmp_size;}
        inline void jumpBackward(const size_t jmp_size = 1) noexcept{self._IdxPointer -= jmp_size;}

        inline void jumpForward(const NumPair step = {0,1}) noexcept
        {self._IdxPointer += (step.first * self._Matrix->getColumnSize() + step.second);}
        inline void jumpBackward(const NumPair step = {0,1}) noexcept
        {self._IdxPointer -= (step.first * self._Matrix->getColumnSize() + step.second);}

        inline void jumpTo(const NumPair to = {0,0}) noexcept
         {_IdxPointer = to.first *self._Matrix->getColumnSize()  + to.second;}

         //Iterates through the matrix fast , excludes loop overhead for larger matricies.
template <typename ApplyFuncTy_>
inline void walk(const NumPair& stop,
                    const size_t step,
                    ApplyFuncTy_& func,
                    const bool returnBack = true)
    {
    std::size_t _RowSize = self._Matrix->getRowSize();
    std::size_t _ColSize = self._Matrix->getColumnSize();
    #ifndef NO_RANGE_VALIDITY_CHECK
        if (stop.first >= _RowSize || stop.second >= _ColSize)
            throw std::out_of_range("Out of range error!");
    #endif
    
    #ifndef NO_ITERATOREXCEPT_CHECK
        if (_IdxPointer >= _RowSize * _ColSize)
            throw IteratorReachedEnd("Iterator has reached the end of target!");
    #endif

    #ifndef NO_STEP_VALIDITY_CHECK
        if (step == 0)
            throw std::invalid_argument("Step cannot be 0!");
    #endif

    std::size_t flat_stop = stop.first * _ColSize + stop.second;
    NumberType* data_pointer = self._Matrix->getData().data();
    
    #ifndef NO_STEP_VALIDITY_CHECK
        if (flat_stop <= _IdxPointer)
            throw std::invalid_argument("Stop must be ahead of current iterator position.");
    #endif

    std::size_t bound = flat_stop & (~7ULL);
    #pragma omp parallel for
    for (std::size_t idx = _IdxPointer;  idx <= bound ;  idx += 8*step)
    {
        func(data_pointer[idx]);
        func(data_pointer[idx + step]);
        func(data_pointer[idx + 2 * step]);
        func(data_pointer[idx + 3 * step]);
        func(data_pointer[idx + 4 * step]);
        func(data_pointer[idx + 5 * step]);
        func(data_pointer[idx + 6 * step]);
        func(data_pointer[idx + 7 * step]);
    }

    for (std::size_t idx = bound; idx <= flat_stop; idx += step)
        func(data_pointer[idx]);
        
    if(!returnBack)
      _IdxPointer = flat_stop;
        
}



    template <typename ApplyFuncTy_>
    inline void walk(const NumPair& stop,
                    const NumPair& step,
                    ApplyFuncTy_& func,
                    const bool returnBack = true)
    {

    std::size_t _RowSize = self._Matrix->getRowSize();
    std::size_t _ColSize = self._Matrix->getColumnSize();
    std::size_t flat_step = step.first*_ColSize + step.second;
    #ifndef NO_RANGE_VALIDITY_CHECK
        if (stop.first >= _RowSize || stop.second >= _ColSize)
            throw std::out_of_range("Out of range error!");
    #endif

    #ifndef NO_ITERATOREXCEPT_CHECK
        if (_IdxPointer >= _RowSize * _ColSize)
            throw IteratorReachedEnd("Iterator has reached the end of target!");
    #endif

    #ifndef NO_STEP_VALIDITY_CHECK
        if (flat_step == 0)
            throw std::invalid_argument("Step cannot be 0!");
    #endif

        size_t flat_stop = stop.first * _ColSize + stop.second ;
        NumberType* data_pointer = self._Matrix->getData().data();

    std::size_t bound = flat_stop & (~7ULL);
    
    #pragma omp parallel for
    for (std::size_t idx = _IdxPointer;  idx <= bound ;  idx += 8*flat_step)
    {
        func(data_pointer[idx]);
        func(data_pointer[idx + flat_step]);
        func(data_pointer[idx + 2 * flat_step]);
        func(data_pointer[idx + 3 * flat_step]);
        func(data_pointer[idx + 4 * flat_step]);
        func(data_pointer[idx + 5 * flat_step]);
        func(data_pointer[idx + 6 * flat_step]);
        func(data_pointer[idx + 7 * flat_step]);
    }

    for (std::size_t idx = bound; idx <= flat_stop; idx += flat_step)
        func(data_pointer[idx]);
        
    if(!returnBack)
      _IdxPointer = flat_stop;
    }

    inline void showIterator(){
        std::cout << "(Dtype: Matrix2DIterator): {";
        std::cout << '\n';
        std::cout <<   "    Target Id        : "<< self._Matrix->getObjectId();
        std::cout << "\n    Self Id          : "<< self._ObjId;
        std::cout << "\n    Current Loc(flat): "<< self._IdxPointer;
        NumPair loc = self.getLocation();
        std::cout << "\n    Current Loc(2D)  : ("<< loc.first << "," << loc.second << ")";
        std::cout << "\n    Current Value    : "<< *self;
        std::cout  <<  "\n}\n";
    }
    };
}
#endif