#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "includes.hpp"
#include <cassert>
#include <stdexcept>


#define self (*this)
#define NULL_OBJECT_ID 0U

namespace Linalg{
template<typename Ty>
using FlatArray = std::vector<Ty>;
using NumPair   = std::pair<size_t, size_t>;
template <typename NumberType = float>
class Matrix{
    private:
    
    FlatArray<NumberType>   _Data;
    std::size_t             _RowSize;
    std::size_t             _ColumnSize;
    std::size_t             _ObjId = NULL_OBJECT_ID;

    public:

    inline NumberType& operator() (const std::size_t rIdx, const std::size_t cIdx){
#ifndef DISABLE_VALIDATION
        if ((rIdx * _ColumnSize + cIdx) >= _RowSize * _ColumnSize)
            throw std::out_of_range
        ("(Object Id: " + std::to_string(_ObjId) + ") index is out of range!");
#endif
        return _Data.data()[rIdx * _ColumnSize + cIdx];
    }
    inline std::size_t getRowSize() noexcept {
        return self._RowSize;
    }
    inline std::size_t getColumnSize() noexcept {
        return self._ColumnSize;
    }
    inline std::size_t getObjectId() noexcept{
        return self._ObjId;
    }
    inline FlatArray<NumberType>& getData() noexcept {
        return self._Data;
    }
    inline void setRowSize(const std::size_t newSize) noexcept {
        self._RowSize = newSize;
    }
    inline void setColumnSize(const std::size_t newSize) noexcept {
        self._ColumnSize = newSize;
    }
    inline void setObjectId(const std::size_t newId) noexcept{
        self._ObjId = newId;
    }
};
inline static float _getzero(const std::size_t idx){
    return 0;
}
template<typename NumberType>
inline Matrix<NumberType> createMatrix(
    const std::size_t rowSize, 
    const std::size_t columnSize, 
    const std::size_t objectId)
{
    Matrix<NumberType> result;
    result.setColumnSize(columnSize);
    result.setRowSize(rowSize);
    result.setObjectId(objectId);
    result.getData().resize(rowSize * columnSize);
    result.getData().shrink_to_fit();
    return result;
}

template<typename NumberType>
inline void copy(
    Matrix<NumberType>& from,
    Matrix<NumberType>& to,
    std::size_t           newId
){
    to.setColumnSize(from.getColumnSize());
    to.setRowSize(from.getRowSize());
    to.getData() = from.getData();
    to.setObjectId(newId);
}


template<typename NumberType>
inline void copy_if_allocated(
    Matrix<NumberType>& from,
    Matrix<NumberType>& to,
    std::size_t           newId
){
    to.setColumnSize(from.getColumnSize());
    to.setRowSize(from.getRowSize());
    to.setObjectId(newId);

    const NumberType* src = from.getData().data();
    NumberType* to_data = to.getData().data();
    const std::size_t size = to.getData().size();

    if(size != from.getData().size())
       throw std::runtime_error("Size mismatch!");
    
    std::size_t idx = 0;
    for(; idx + 7< size; idx += 8){
        to_data[idx] = src[idx];
        to_data[idx+1] = src[idx+1];
        to_data[idx+2] = src[idx+2];
        to_data[idx+3] = src[idx+3];
        to_data[idx+4] = src[idx+4];
        to_data[idx+5] = src[idx+5];
        to_data[idx+6] = src[idx+6];
        to_data[idx+7] = src[idx+7];
    }

    for(; idx < size; idx++)
    to_data[idx] = src[idx];

}

template<typename NumberType1, typename NumberType2>
inline void copy_if_allocated(
    Matrix<NumberType1>& from,
    Matrix<NumberType2>& to,
    std::size_t           newId
){
    to.setColumnSize(from.getColumnSize());
    to.setRowSize(from.getRowSize());
    to.setObjectId(newId);

    const NumberType1* src = from.getData().data();
    NumberType2* to_data = to.getData().data();
    const std::size_t size = to.getData().size();

    if(size != from.getData().size())
       throw std::runtime_error("Size mismatch!");
    
    std::size_t idx = 0;
    for(; idx + 7< size; idx += 8){
        to_data[idx]   = static_cast<NumberType2>(src[idx]);
        to_data[idx+1] = static_cast<NumberType2>(src[idx+1]);
        to_data[idx+2] = static_cast<NumberType2>(src[idx+2]);
        to_data[idx+3] = static_cast<NumberType2>(src[idx+3]);
        to_data[idx+4] = static_cast<NumberType2>(src[idx+4]);
        to_data[idx+5] = static_cast<NumberType2>(src[idx+5]);
        to_data[idx+6] = static_cast<NumberType2>(src[idx+6]);
        to_data[idx+7] = static_cast<NumberType2>(src[idx+7]);
    }

    for(; idx < size; idx++)
    to_data[idx] = src[idx];

}
template<typename NumberType, typename ValueGenerator>
void fill(
    Matrix<NumberType>& matrix,
    ValueGenerator& gen
){
    std::size_t size = matrix.getData().size();
    NumberType* data = matrix.getData().data();
    std::size_t idx = 0;

    for (; idx + 7 < size; idx += 8) {
        data[idx]     = gen(idx);
        data[idx + 1] = gen(idx + 1);
        data[idx + 2] = gen(idx + 2);
        data[idx + 3] = gen(idx + 3);
        data[idx + 4] = gen(idx + 4);
        data[idx + 5] = gen(idx + 5);
        data[idx + 6] = gen(idx + 6);
        data[idx + 7] = gen(idx + 7);
    }

    for (; idx < size; idx++)
        data[idx] = gen(idx);
}

template<typename NumberType>
void transpose(
    Matrix<NumberType>& m,
    const std::size_t rtnId = NULL_OBJECT_ID,       
    const std::size_t rowTile = 128,
    const std::size_t colTile = 128
){
    size_t rows = m.getRowSize();
    size_t cols = m.getColumnSize();
    std::vector<NumberType> temp(rows * cols);
    NumberType* temp_data = temp.data();
    const NumberType* raw_data = m.getData().data();

    #pragma omp parallel for 
    for (size_t i_tile = 0; i_tile < rows; i_tile += rowTile) {
        for (size_t j_tile = 0; j_tile < cols; j_tile += colTile) {
            size_t i_end = std::min(i_tile + rowTile, rows);
            size_t j_end = std::min(j_tile + colTile, cols);

            for (size_t i = i_tile; i < i_end; ++i) {
                size_t j = j_tile;
                for (; j + 7 < j_end; j += 8) {
                    temp_data[j * rows + i]        = raw_data[i * cols + j];
                    temp_data[(j + 1) * rows + i]  = raw_data[i * cols + j + 1];
                    temp_data[(j + 2) * rows + i]  = raw_data[i * cols + j + 2];
                    temp_data[(j + 3) * rows + i]  = raw_data[i * cols + j + 3];
                    temp_data[(j + 4) * rows + i]  = raw_data[i * cols + j + 4];
                    temp_data[(j + 5) * rows + i]  = raw_data[i * cols + j + 5];
                    temp_data[(j + 6) * rows + i]  = raw_data[i * cols + j + 6];
                    temp_data[(j + 7) * rows + i]  = raw_data[i * cols + j + 7];
                    }

                for (; j < j_end; ++j)
                    temp_data[j * rows + i] = raw_data[i * cols + j];
            }
        }
    }

    
    m.getData().swap(temp);
    m.setRowSize(cols);
    m.setColumnSize(rows);
}

template<typename NumberType>
inline void show(
    Matrix<NumberType>& matrix
){
    std::cout << "(Dtype: Matrix): [\n";
    const NumberType* data = matrix.getData().data();
    std::size_t _RowSize = matrix.getRowSize();
    std::size_t _ColumnSize = matrix.getColumnSize();

    for (size_t row_idx = 0; row_idx < _RowSize; row_idx++) {
        std::cout << "    #Row[" << row_idx << "]: [";
        bool mask = false;

        for (size_t col_idx = 0; col_idx < _ColumnSize; col_idx++) {
            if (mask) std::cout << ", ";
            mask = true;
            std::cout << data[row_idx * _ColumnSize + col_idx];
        }
        std::cout << "]\n";
    }

    std::cout << "], Shape: (" << _RowSize << ", " << _ColumnSize << ")" << ", ObjectId: " << matrix.getObjectId() << '\n';
}

template<typename NumberType>
void reshape(
    Matrix<NumberType>& matrix,
    NumPair new_shape){
    size_t size = matrix.getData().size();

        new_shape.first  = new_shape.first  == -1 ? size / new_shape.second : new_shape.first;
        new_shape.second = new_shape.second == -1 ? size / new_shape.first  : new_shape.second;

#ifndef DISABLE_VALIDATION
        size_t product = new_shape.first * new_shape.second;
        if (product != size)
            throw std::runtime_error("Invalid shape for applying new shape!");
#endif

    matrix.setRowSize(new_shape.first);
    matrix.setColumnSize(new_shape.second);
}

template<typename NumberType>
void slice(
    Matrix<NumberType>& matrix,
    Matrix<NumberType>& rtnAddr,
    const NumPair start,
    const NumPair end,
    const std::size_t i_stride = 1,
    const std::size_t j_stride = 1,
    const std::size_t row_tile = 128,
    const std::size_t col_tile = 128
){
    #ifndef NO_SIZE_INVALIDITY_CHECK
        if (start.first >= matrix.getRowSize()|| end.first > matrix.getRowSize() ||
            start.second >= matrix.getColumnSize() || end.second > matrix.getColumnSize())
            throw std::runtime_error("Out of range Error!");
#endif

        const std::size_t row_len = (end.first  - start.first)/i_stride;
        const std::size_t col_len = (end.second - start.second)/j_stride;

#ifndef NO_SZIE_EQUALITY_CHECK
        if(row_len != rtnAddr.getRowSize() || col_len != rtnAddr.getColumnSize())
            throw std::runtime_error("Invalid shape for res!");
#endif
        NumberType* res_data = rtnAddr.getData().data();
        NumberType* raw_data = matrix.getData().data();
        size_t res_idx = 0;

        for (size_t i_tile = start.first; i_tile < end.first; i_tile += row_tile) {
            for (size_t j_tile = start.second; j_tile < end.second; j_tile += col_tile) {
                size_t i_end = std::min(i_tile + row_tile, end.first);
                size_t j_end = std::min(j_tile + col_tile, end.second);

                for (size_t i = i_tile; i < i_end; i += i_stride) {
                    size_t base = i * matrix.getColumnSize();
                    size_t j = j_tile;
                    for (; j + 7*j_stride < j_end; j += 8*j_stride) {
                        const std::size_t inner_base = base + j;
                        res_data[res_idx]     = raw_data[inner_base];
                        res_data[res_idx + 1] = raw_data[inner_base + 1*j_stride];
                        res_data[res_idx + 2] = raw_data[inner_base + 2*j_stride];
                        res_data[res_idx + 3] = raw_data[inner_base + 3*j_stride];
                        res_data[res_idx + 4] = raw_data[inner_base + 4*j_stride];
                        res_data[res_idx + 5] = raw_data[inner_base + 5*j_stride];
                        res_data[res_idx + 6] = raw_data[inner_base + 6*j_stride];
                        res_data[res_idx + 7] = raw_data[inner_base + 7*j_stride];
                        res_idx += 8;
                    }

                    for (; j < j_end; j += j_stride) {
                        res_data[res_idx]     = raw_data[base + j];
                        res_idx++;
                    }
                }
            }
        }
}

template<typename NumberType, typename FunctionType>
void map(
    Matrix<NumberType>& matrix,
    FunctionType& func
){
    size_t idx = 0;
    NumberType* raw_data = matrix.getData().data();
    std::size_t size = matrix.getData().size();

    for (; idx + 7 < size; idx += 8) {
        func(raw_data[idx]);
        func(raw_data[idx + 1]);
        func(raw_data[idx + 2]);
        func(raw_data[idx + 3]);
        func(raw_data[idx + 4]);
        func(raw_data[idx + 5]);
        func(raw_data[idx + 6]);
        func(raw_data[idx + 7]);
    }

    for (; idx < size; idx++)
        func(raw_data[idx]);
}

template<typename NumberType1, 
         typename NumberType2,
         typename NumberType3,
         typename FunctionType,
         typename IndexerType1,
         typename IndexerType2>
 void zip_with_accum(
    Matrix<NumberType1>& mat1,
    Matrix<NumberType2>& mat2,
    Matrix<NumberType3>& res,
    FunctionType& func,
    IndexerType1& indexer1,
    IndexerType2& indexer2
){
     std::size_t size = mat1.getData().size();

    const NumberType1* m1_data = mat1.getData().data();
    const NumberType2* m2_data = mat2.getData().data();
    NumberType3* res_data      = res.getData().data();

    std::size_t idx = 0;

    for (; idx + 7 < size; idx += 8) {
        res_data[idx]     += func(m1_data[indexer1(idx)],     m2_data[indexer2(idx)]);
        res_data[idx + 1] += func(m1_data[indexer1(idx + 1)], m2_data[indexer2(idx + 1)]);
        res_data[idx + 2] += func(m1_data[indexer1(idx + 2)], m2_data[indexer2(idx + 2)]);
        res_data[idx + 3] += func(m1_data[indexer1(idx + 3)], m2_data[indexer2(idx + 3)]);
        res_data[idx + 4] += func(m1_data[indexer1(idx + 4)], m2_data[indexer2(idx + 4)]);
        res_data[idx + 5] += func(m1_data[indexer1(idx + 5)], m2_data[indexer2(idx + 5)]);
        res_data[idx + 6] += func(m1_data[indexer1(idx + 6)], m2_data[indexer2(idx + 6)]);
        res_data[idx + 7] += func(m1_data[indexer1(idx + 7)], m2_data[indexer2(idx + 7)]);
    }

    for (; idx < size; idx++)
        res_data[idx] += func(m1_data[indexer1(idx)], m2_data[indexer2(idx)]);
}


template<typename NumberType1, 
         typename NumberType2,
         typename NumberType3,
         typename FunctionType,
         typename IndexerType1,
         typename IndexerType2>
 void zip_no_accum(
    Matrix<NumberType1>& mat1,
    Matrix<NumberType2>& mat2,
    Matrix<NumberType3>& res,
    FunctionType& func,
    IndexerType1& indexer1,
    IndexerType2& indexer2
){
     std::size_t size = mat1.getData().size();

    const NumberType1* m1_data = mat1.getData().data();
    const NumberType2* m2_data = mat2.getData().data();
    NumberType3* res_data      = res.getData().data();

    std::size_t idx = 0;

    for (; idx + 7 < size; idx += 8) {
        res_data[idx]     = func(m1_data[indexer1(idx)],     m2_data[indexer2(idx)]);
        res_data[idx + 1] = func(m1_data[indexer1(idx + 1)], m2_data[indexer2(idx + 1)]);
        res_data[idx + 2] = func(m1_data[indexer1(idx + 2)], m2_data[indexer2(idx + 2)]);
        res_data[idx + 3] = func(m1_data[indexer1(idx + 3)], m2_data[indexer2(idx + 3)]);
        res_data[idx + 4] = func(m1_data[indexer1(idx + 4)], m2_data[indexer2(idx + 4)]);
        res_data[idx + 5] = func(m1_data[indexer1(idx + 5)], m2_data[indexer2(idx + 5)]);
        res_data[idx + 6] = func(m1_data[indexer1(idx + 6)], m2_data[indexer2(idx + 6)]);
        res_data[idx + 7] = func(m1_data[indexer1(idx + 7)], m2_data[indexer2(idx + 7)]);
    }

    for (; idx < size; idx++)
        res_data[idx] = func(m1_data[indexer1(idx)], m2_data[indexer2(idx)]);
}

template<typename NumberType1, 
         typename NumberType2,
         typename NumberType3,
         typename FunctionType,
         typename IndexerType1,
         typename IndexerType2>
 void zip(
    Matrix<NumberType1>& mat1,
    Matrix<NumberType2>& mat2,
    Matrix<NumberType3>& res,
    FunctionType& func,
    IndexerType1& indexer1,
    IndexerType2& indexer2,
    const bool accum = false
){
    if(accum) zip_with_accum(mat1, mat2, res, func, indexer1, indexer2);
    else zip_no_accum(mat1, mat2, res, func, indexer1, indexer2);
} 


template<typename NumberType1,
         typename NumberType2,
         typename HandleType,
         typename IndexerType1,
         typename IndexerType2>
HandleType innerProduct(
    Matrix<NumberType1>& m1,
    Matrix<NumberType2>& m2,
    IndexerType1& indexer1,
    IndexerType2& indexer2
){
    std::size_t size = m1.getData().size();

#ifndef DISABLE_VALIDATION
        if (m1.getRowSize() != m2.getRowSize() || m1.getColumnSize() != m2.getColumnSize())
            throw std::runtime_error("Shape mismatched for m1 and m2");
#endif

    const NumberType1* a = m1.getData().data();
    const NumberType2* b = m2.getData().data();


    HandleType acc0 = 0.0f, acc1 = 0.0f, acc2 = 0.0f, acc3 = 0.0f;
    HandleType acc4 = 0.0f, acc5 = 0.0f, acc6 = 0.0f, acc7 = 0.0f;

    std::size_t i = 0;

    #define CST2 static_cast<HandleType>
    for (; i + 7 < size; i += 8) {
        acc0 += CST2(a[indexer1(i)])     * CST2(b[indexer2(i)]);
        acc1 += CST2(a[indexer1(i + 1)]) * CST2(b[indexer2(i + 1)]);
        acc2 += CST2(a[indexer1(i + 2)]) * CST2(b[indexer2(i + 2)]);
        acc3 += CST2(a[indexer1(i + 3)]) * CST2(b[indexer2(i + 3)]);
        acc4 += CST2(a[indexer1(i + 4)]) * CST2(b[indexer2(i + 4)]);
        acc5 += CST2(a[indexer1(i + 5)]) * CST2(b[indexer2(i + 5)]);
        acc6 += CST2(a[indexer1(i + 6)]) * CST2(b[indexer2(i + 6)]);
        acc7 += CST2(a[indexer1(i + 7)]) * CST2(b[indexer2(i + 7)]);
    }

    for (; i < size; i++)
        acc0 += CST2(a[indexer1(i)]) * CST2(b[indexer2(i)]);

    return acc0 + acc1 + acc2 + acc3 + acc4 + acc5 + acc6 + acc7;
}

template<typename NumberType1, 
         typename NumberType2, 
         typename Reducer>
void reduce_with_accum(
    Matrix<NumberType1>& matrix,
    Matrix<NumberType2>& rtn,
    Reducer& func,
    std::size_t axis = 0
){
    const std::size_t stride       = (axis == 0) ? matrix.getColumnSize() : 1;
    const std::size_t other_stride = (axis == 0) ? 1 : matrix.getColumnSize() ;
    const std::size_t out_size     = (axis == 0) ? matrix.getColumnSize() : matrix.getRowSize();
    const std::size_t reduce_size  = (axis == 0) ? matrix.getRowSize() : matrix.getColumnSize();
    const NumberType1* data = matrix.getData().data();
    NumberType2* rtn_addr = rtn.getData().data();
#ifndef DISABLE_VALIDATION
    if(rtn.getData().size() != out_size)
            throw std::runtime_error("Invalid size error!");
#endif
        

    for (size_t res_idx = 0; res_idx < out_size; res_idx++) {

        size_t base = res_idx * other_stride;
        func.initialize(data[base]);
        size_t axis_idx = 1;

        for (; axis_idx + 7 < reduce_size; axis_idx += 8) {
            func << data[base + stride * (axis_idx)];
            func << data[base + stride * (axis_idx + 1)];
            func << data[base + stride * (axis_idx + 2)];
            func << data[base + stride * (axis_idx + 3)];
            func << data[base + stride * (axis_idx + 4)];
            func << data[base + stride * (axis_idx + 5)];
            func << data[base + stride * (axis_idx + 6)];
            func << data[base + stride * (axis_idx + 7)];
        }
        for (; axis_idx < reduce_size; axis_idx++)
            func << data[base + stride * axis_idx];
        rtn_addr[res_idx] += func();
    }
}
template<typename NumberType1, 
         typename NumberType2, 
         typename Reducer>
void reduce_no_accum(
    Matrix<NumberType1>& matrix,
    Matrix<NumberType2>& rtn,
    Reducer& func,
    std::size_t axis = 0
){
    const std::size_t stride       = (axis == 0) ? matrix.getColumnSize() : 1;
    const std::size_t other_stride = (axis == 0) ? 1 : matrix.getColumnSize() ;
    const std::size_t out_size     = (axis == 0) ? matrix.getColumnSize() : matrix.getRowSize();
    const std::size_t reduce_size  = (axis == 0) ? matrix.getRowSize() : matrix.getColumnSize();
    const NumberType1* data = matrix.getData().data();

    NumberType2* rtn_addr = rtn.getData().data();
#ifndef DISABLE_VALIDATION
    if(rtn.getData().size() != out_size)
            throw std::runtime_error("Invalid size error!");
#endif
        

    for (size_t res_idx = 0; res_idx < out_size; res_idx++) {

        size_t base = res_idx * other_stride;
        func.initialize(data[base]);
        size_t axis_idx = 1;

        for (; axis_idx + 7 < reduce_size; axis_idx += 8) {
            func << data[base + stride * (axis_idx)];
            func << data[base + stride * (axis_idx + 1)];
            func << data[base + stride * (axis_idx + 2)];
            func << data[base + stride * (axis_idx + 3)];
            func << data[base + stride * (axis_idx + 4)];
            func << data[base + stride * (axis_idx + 5)];
            func << data[base + stride * (axis_idx + 6)];
            func << data[base + stride * (axis_idx + 7)];
        }
        for (; axis_idx < reduce_size; axis_idx++)
            func << data[base + stride * axis_idx];
        rtn_addr[res_idx] = func();
    }
}
template<typename NumberType1, 
         typename NumberType2, 
         typename Reducer>
void reduce(
    Matrix<NumberType1>& matrix,
    Matrix<NumberType2>& rtn,
    Reducer& func,
    std::size_t axis = 0,
    const bool accum = false
){
    if(accum) reduce_with_accum(matrix, rtn, func, axis);
    else reduce_no_accum(matrix, rtn, func, axis);
}

template <typename NumberTypeA,
          typename NumberTypeB,
          typename NumberTypeC>
void matmul_with_accum(
    Matrix<NumberTypeA>& m1,
    Matrix<NumberTypeB>& m2,
    Matrix<NumberTypeC>& res,
    const std::size_t row_tile = 128,
    const std::size_t col_tile = 128,
    const std::size_t matmulk_tile = 64
){
#ifndef DISABLE_VALIDATION
    if (m1.getColumnSize() != m2.getRowSize())
        throw std::runtime_error("Invalid shapes of m1 and m2 for MatMul!");
    if (m1.getRowSize() != res.getRowSize() || m2.getColumnSize() != res.getColumnSize())
        throw std::runtime_error("Invalid shape of res in MatMul!");
#endif

    const NumberTypeA* m1_data = m1.getData().data();
    const NumberTypeB* m2_data = m2.getData().data();
    NumberTypeC* res_data      = res.getData().data();

        
        #pragma omp parallel for 
        for (size_t i_tile = 0; i_tile < m1.getRowSize(); i_tile += row_tile) {
            for (size_t k_tile = 0; k_tile < m1.getColumnSize(); k_tile += matmulk_tile) {
                for (size_t j_tile = 0; j_tile < m2.getColumnSize(); j_tile += col_tile) {

                    size_t i_end = std::min(i_tile + row_tile, m1.getRowSize());
                    size_t k_end = std::min(k_tile + matmulk_tile, m1.getColumnSize());
                    size_t j_end = std::min(j_tile + col_tile, m2.getColumnSize());

                    for (size_t i = i_tile; i < i_end; i++) {
                        for (size_t k = k_tile; k < k_end; k++) {
                            size_t j = j_tile;

                            NumberTypeC a        = static_cast<NumberTypeC>(m1_data[i * m1.getColumnSize() + k]);
                            const NumberTypeB* b = &m2_data[k * m2.getColumnSize() + j];
                            NumberTypeC* c       = &res_data[i * res.getColumnSize() + j];

                            #define CST static_cast<NumberTypeC>
                            
                            for (; j + 7 < j_end; j += 8) {
                                c[0] += a * CST(b[0]);
                                c[1] += a * CST(b[1]);
                                c[2] += a * CST(b[2]);
                                c[3] += a * CST(b[3]);
                                c[4] += a * CST(b[4]);
                                c[5] += a * CST(b[5]);
                                c[6] += a * CST(b[6]);
                                c[7] += a * CST(b[7]);
                                c += 8;
                                b += 8;
                            }

                            for (; j < j_end; j++) {
                                *c += a * CST((*b));
                                c++;
                                b++;
                            } // j
                        } // k
                    } // i

                } // j_tile
            } // k_tile
        } // i_tile
}
template <typename NumberTypeA,
          typename NumberTypeB,
          typename NumberTypeC>
void matmul_no_accum(
    Matrix<NumberTypeA>& m1,
    Matrix<NumberTypeB>& m2,
    Matrix<NumberTypeC>& res,
    const std::size_t row_tile = 128,
    const std::size_t col_tile = 128,
    const std::size_t matmulk_tile = 64
){
    Linalg::fill(res, _getzero);
#ifndef DISABLE_VALIDATION
    if (m1.getColumnSize() != m2.getRowSize())
        throw std::runtime_error("Invalid shapes of m1 and m2 for MatMul!");
    if (m1.getRowSize() != res.getRowSize() || m2.getColumnSize() != res.getColumnSize())
        throw std::runtime_error("Invalid shape of res in MatMul!");
#endif

    const NumberTypeA* m1_data = m1.getData().data();
    const NumberTypeB* m2_data = m2.getData().data();
    NumberTypeC* res_data      = res.getData().data();

        
        #pragma omp parallel for 
        for (size_t i_tile = 0; i_tile < m1.getRowSize(); i_tile += row_tile) {
            for (size_t k_tile = 0; k_tile < m1.getColumnSize(); k_tile += matmulk_tile) {
                for (size_t j_tile = 0; j_tile < m2.getColumnSize(); j_tile += col_tile) {

                    size_t i_end = std::min(i_tile + row_tile, m1.getRowSize());
                    size_t k_end = std::min(k_tile + matmulk_tile, m1.getColumnSize());
                    size_t j_end = std::min(j_tile + col_tile, m2.getColumnSize());

                    for (size_t i = i_tile; i < i_end; i++) {
                        for (size_t k = k_tile; k < k_end; k++) {
                            size_t j = j_tile;

                            NumberTypeC a        = static_cast<NumberTypeC>(m1_data[i * m1.getColumnSize() + k]);
                            const NumberTypeB* b = &m2_data[k * m2.getColumnSize() + j];
                            NumberTypeC* c       = &res_data[i * res.getColumnSize() + j];

                            #define CST static_cast<NumberTypeC>
                            
                            for (; j + 7 < j_end; j += 8) {
                                c[0] += a * CST(b[0]);
                                c[1] += a * CST(b[1]);
                                c[2] += a * CST(b[2]);
                                c[3] += a * CST(b[3]);
                                c[4] += a * CST(b[4]);
                                c[5] += a * CST(b[5]);
                                c[6] += a * CST(b[6]);
                                c[7] += a * CST(b[7]);
                                c += 8;
                                b += 8;
                            }

                            for (; j < j_end; j++) {
                                *c += a * CST((*b));
                                c++;
                                b++;
                            } // j
                        } // k
                    } // i

                } // j_tile
            } // k_tile
        } // i_tile
}
template <typename NumberTypeA,
          typename NumberTypeB,
          typename NumberTypeC>
void matmul(
    Matrix<NumberTypeA>& m1,
    Matrix<NumberTypeB>& m2,
    Matrix<NumberTypeC>& res,
    const bool accum = false,
    const std::size_t row_tile = 128,
    const std::size_t col_tile = 128,
    const std::size_t matmulk_tile = 64
){
    if(accum) matmul_with_accum(m1, m2, res, row_tile, col_tile, matmulk_tile);
    else matmul_no_accum(m1, m2, res, row_tile, col_tile, matmulk_tile);
}

/*

A simple and efficient matmul implementation for user defined indexed access.
Args:
....Matrix<NumberTypeA>& m1 => first Matrix                                                                       
....Matrix<NumberTypeB>& m2 => second Matrix                                                                        
....Matrix<NumberTypeC>& res => result buffer (must be initialized with 0 if want to get only the result of matmul) 
....Indexer1& idxer1 => User defined first indexer                                                      
....Indexer2& idxer2 => User defined second indexer                                                 
....const std::size_t row_tile = 128 => row tile size for tiling                                                   
....const std::size_t col_tile = 128 => column tile size                                                           
....const std::size_t matmulk_tile = 64 => tile size for k                                                      

Returns:
....void

*/
template <typename NumberTypeA,
          typename NumberTypeB,
          typename NumberTypeC,
          typename Indexer1,
          typename Indexer2>
void matmul_with_accum(
    Matrix<NumberTypeA>& m1,
    Matrix<NumberTypeB>& m2,
    Matrix<NumberTypeC>& res,
    const NumPair m1_shape,
    const NumPair m2_shape,
    Indexer1& idxer1,
    Indexer2& idxer2,
    const std::size_t row_tile = 128,
    const std::size_t col_tile = 128,
    const std::size_t matmulk_tile = 64
){
#ifndef DISABLE_VALIDATION
    if (m1_shape.second != m2_shape.first)
        throw std::runtime_error("Invalid shapes of m1 and m2 for MatMul!");
    if (m1_shape.first != res.getRowSize() || m2_shape.second != res.getColumnSize())
        throw std::runtime_error("Invalid shape of res in MatMul!");
#endif

    const NumberTypeA* __restrict__ a_data = m1.getData().data();
    const NumberTypeB* __restrict__ b = m2.getData().data();
    NumberTypeC* __restrict__ c_data      = res.getData().data();

        
        #pragma omp parallel for 
        for (size_t i_tile = 0; i_tile < m1_shape.first; i_tile += row_tile) {
            for (size_t k_tile = 0; k_tile < m1_shape.second; k_tile += matmulk_tile) {
                for (size_t j_tile = 0; j_tile < m2_shape.second; j_tile += col_tile) {

                    size_t i_end = std::min(i_tile + row_tile, m1_shape.first);
                    size_t k_end = std::min(k_tile + matmulk_tile, m1_shape.second);
                    size_t j_end = std::min(j_tile + col_tile, m2_shape.second);

                    for (size_t i = i_tile; i < i_end; i++) {
                        for (size_t k = k_tile; k < k_end; k++) {
                            size_t j = j_tile;

                            const NumberTypeC a        = static_cast<NumberTypeC>(
                                a_data[idxer1(i * m1_shape.second + k)]
                            );
                            NumberTypeC* c = &c_data[i * res.getColumnSize() + j];

                            #define CST static_cast<NumberTypeC>
                            
                            for (; j + 7 < j_end; j += 8) {
                                const std::size_t b_base = k * m2_shape.second + j;
                                c[0] += a * CST(b[idxer2(b_base)]);
                                c[1] += a * CST(b[idxer2(b_base+1)]);
                                c[2] += a * CST(b[idxer2(b_base+2)]);
                                c[3] += a * CST(b[idxer2(b_base+3)]);
                                c[4] += a * CST(b[idxer2(b_base+4)]);
                                c[5] += a * CST(b[idxer2(b_base+5)]);
                                c[6] += a * CST(b[idxer2(b_base+6)]);
                                c[7] += a * CST(b[idxer2(b_base+7)]);
                                c += 8;
                            }

                            for (; j < j_end; j++) 
                            {
                                const std::size_t b_base = k * m2_shape.second + j;
                                const std::size_t c_base = i * res.getColumnSize() + j;
                                *c   += a * CST(b[idxer2(b_base)]);
                                c++;
                            }
                             // j
                        } // k
                    } // i

                } // j_tile
            } // k_tile
        } // i_tile
}
template <typename NumberTypeA,
          typename NumberTypeB,
          typename NumberTypeC,
          typename Indexer1,
          typename Indexer2>
void matmul_no_accum(
    Matrix<NumberTypeA>& m1,
    Matrix<NumberTypeB>& m2,
    Matrix<NumberTypeC>& res,
    const NumPair m1_shape,
    const NumPair m2_shape,
    Indexer1& idxer1,
    Indexer2& idxer2,
    const std::size_t row_tile = 128,
    const std::size_t col_tile = 128,
    const std::size_t matmulk_tile = 64
){
    Linalg::fill(res, _getzero);
#ifndef DISABLE_VALIDATION
    if (m1_shape.second != m2_shape.first)
        throw std::runtime_error("Invalid shapes of m1 and m2 for MatMul!");
    if (m1_shape.first != res.getRowSize() || m2_shape.second != res.getColumnSize())
        throw std::runtime_error("Invalid shape of res in MatMul!");
#endif

    const NumberTypeA* __restrict__ a_data = m1.getData().data();
    const NumberTypeB* __restrict__ b = m2.getData().data();
    NumberTypeC* __restrict__ c_data      = res.getData().data();

        
        #pragma omp parallel for 
        for (size_t i_tile = 0; i_tile < m1_shape.first; i_tile += row_tile) {
            for (size_t k_tile = 0; k_tile < m1_shape.second; k_tile += matmulk_tile) {
                for (size_t j_tile = 0; j_tile < m2_shape.second; j_tile += col_tile) {

                    size_t i_end = std::min(i_tile + row_tile, m1_shape.first);
                    size_t k_end = std::min(k_tile + matmulk_tile, m1_shape.second);
                    size_t j_end = std::min(j_tile + col_tile, m2_shape.second);

                    for (size_t i = i_tile; i < i_end; i++) {
                        for (size_t k = k_tile; k < k_end; k++) {
                            size_t j = j_tile;

                            const NumberTypeC a        = static_cast<NumberTypeC>(
                                a_data[idxer1(i * m1_shape.second + k)]
                            );
                            NumberTypeC* c = &c_data[i * res.getColumnSize() + j];

                            #define CST static_cast<NumberTypeC>
                            
                            for (; j + 7 < j_end; j += 8) {
                                const std::size_t b_base = k * m2_shape.second + j;
                                c[0] += a * CST(b[idxer2(b_base)]);
                                c[1] += a * CST(b[idxer2(b_base+1)]);
                                c[2] += a * CST(b[idxer2(b_base+2)]);
                                c[3] += a * CST(b[idxer2(b_base+3)]);
                                c[4] += a * CST(b[idxer2(b_base+4)]);
                                c[5] += a * CST(b[idxer2(b_base+5)]);
                                c[6] += a * CST(b[idxer2(b_base+6)]);
                                c[7] += a * CST(b[idxer2(b_base+7)]);
                                c += 8;
                            }

                            for (; j < j_end; j++) 
                            {
                                const std::size_t b_base = k * m2_shape.second + j;
                                const std::size_t c_base = i * res.getColumnSize() + j;
                                *c   += a * CST(b[idxer2(b_base)]);
                                c++;
                            }
                             // j
                        } // k
                    } // i

                } // j_tile
            } // k_tile
        } // i_tile
}

template <typename NumberTypeA,
          typename NumberTypeB,
          typename NumberTypeC,
          typename Indexer1,
          typename Indexer2>
void matmul(
    Matrix<NumberTypeA>& m1,
    Matrix<NumberTypeB>& m2,
    Matrix<NumberTypeC>& res,
    const NumPair m1_shape,
    const NumPair m2_shape,
    Indexer1& idxer1,
    Indexer2& idxer2,
    const bool accum = false,
    const std::size_t row_tile = 128,
    const std::size_t col_tile = 128,
    const std::size_t matmulk_tile = 64
)
{
    if(accum) matmul_with_accum(m1, m2, res, m1_shape, m2_shape, idxer1, idxer2, row_tile, col_tile, matmulk_tile);
    else matmul_no_accum(m1, m2, res, m1_shape, m2_shape, idxer1, idxer2, row_tile, col_tile, matmulk_tile);
}
}


#endif