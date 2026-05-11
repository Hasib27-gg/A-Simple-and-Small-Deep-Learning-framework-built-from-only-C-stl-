# Welcome to the detailed documentation for the file "Matrix.hpp"
## Preview of the contents:

### [1] Matrix class:
#### - member explanation and thier usage.
#### - ``` inline NumberType& operator()```
#### - ```inline std::size_t getRowSize()```   
#### - ```inline std::size_t getColumnSize()```   
#### - ```inline std::size_t getObjectId()```   
#### - ```inline FlatArray<NumberType>& getData()```
#### - ```inline void setRowSize(const std::size_t newSize)```  
#### - ```inline void setColumnSize(const std::size_t newSize)```   
#### - ```inline void setObjectId(const std::size_t newId)``` 
### [2] Function: 
```
inline Matrix<NumberType> createMatrix(
    const std::size_t rowSize, 
    const std::size_t columnSize, 
    const std::size_t objectId)
```
### [3] Function: 
```
template<typename NumberType>
inline void copy(
    Matrix<NumberType>& from,
    Matrix<NumberType>& to,
    std::size_t           newId
)
```
### [4] Function:
```
template<typename NumberType>
inline void copy_if_allocated(
    Matrix<NumberType>& from,
    Matrix<NumberType>& to,
    std::size_t           newId
)
```
### [5] Function:

```
template<typename NumberType, typename ValueGenerator>
void fill(
    Matrix<NumberType>& matrix,
    ValueGenerator& gen
)
```
### [6] Function:
```
template<typename NumberType>
void transpose(
    Matrix<NumberType>& m,
    const std::size_t rtnId = NULL_OBJECT_ID,       
    const std::size_t rowTile = 128,
    const std::size_t colTile = 128
)
```

### [7] Function:
```
template<typename NumberType>
inline void show(
    Matrix<NumberType>& matrix
)
```

### [8] Function:
```
template<typename NumberType>
inline void show(
    Matrix<NumberType>& matrix
)
```


### [9] Function:
```
template<typename NumberType>
void reshape(
    Matrix<NumberType>& matrix,
    NumPair new_shape)
```

### [10] Function:
```
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
)
```


### [11] Function:
```
template<typename NumberType, typename FunctionType>
void map(
    Matrix<NumberType>& matrix,
    FunctionType& func
)
```


### [12] [Zip Kernel](ZipKernelDoc.hpp)
### [13] [Reduce Kernel](ReduceKernelDoc.hpp)
### [14] [MatMul Kernel](MatMulKernelDoc.hpp)



## Detailed Explanation Startings:

### Matrix Class:
#### Breif: This is the holy first thing in the whole project. It is just a simple wrapper around FlatArray(aka std::vector, std::size_t).
#### Code:

```

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
```

### Member Explanation:
#### [1]: FlatArray<NumberType>   _Data : A simple contigious std::vector for holding the flatten values.
#### [2]: std::size_t             _RowSize : The row size of the shape[0] of the matrix.
#### [3]: std::size_t             _ColumnSize : The column size of the matrix, which is same as shape[1].
#### [4]: std::size_t             _ObjId = NULL_OBJECT_ID : This is a very nice way to debug and name matricies . Though it is not used that in the whole project.

### Member functions explanation:

#### - ``` inline NumberType& operator()``` : Since the matrix stores all the values row - major, it returns a value located at the index (i, j) 
#### - ```inline std::size_t getRowSize()``` : simply returns the row size of the matrix  
#### - ```inline std::size_t getColumnSize()``` : simply returns the column size of the matrix.
#### - ```inline std::size_t getObjectId()```  : returns the object id of the matrix.
#### - ```inline FlatArray<NumberType>& getData()``` : returns the reference to the internal FlatArray<Ty> buffer.
#### - ```inline void setRowSize(const std::size_t newSize)``` : sets new row size.
#### - ```inline void setColumnSize(const std::size_t newSize)``` : sets new column size.
#### - ```inline void setObjectId(const std::size_t newId)```: sets a new object id.


## Explanation to all other functions:

### [2] Function named createMatrix(). Code:
```
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
```

#### What it does:
Simply It just creates an Matrix<NumberType> instance, sets the row, col sizes and the object id then resizes the underlying FlatArray, finally shrinks the FlatArray (to not occupy too much memory) thus returns the result.
#### Example:

```
Linalg::Matrix<float> mat = Linalg::createMatrix<float>(4, 4, 0x001); // creates a 4 x 4 matrix.
```

### [3] Function named copy(). Code: 
```
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
```

#### What it does:
[This function is basically Useless , I don't know why I created it]
It creates a deep copy of a matrix. It takes the matrix  Matrix<NumberType>& from , assigns the row size , col size using .set***Size() to underlying FlatArray,
uses operator= () to assign the from.getData() to the to.getData() and finally sets the new id to the to matrix.

#### Example:
```
Linalg::Matrix<float> source_1 = createMatrix<float>(4, 4, 0x001)
Linalg::Matrix<float> source_2 = createMatrix<float>(4, 4, 0x002)
Linalg::copy(source_1 , source_2, 0x003); // now source_2 has all the attributes of source_1
```


### [4] Function named copy_if_alloacted(), Code:
```
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
```


#### What it does: 
[Usefulness: Extremely Useful]
***Note: It has 2 overloads***
If the destination matrix is already allocated (must have the same capacity of the source matrix), then this function copies all the values from the source matrix to the destination matrix. It uses loop unrolling to exclude the overhead of the for loops. Though it can be optimized through SIMD , but the code would look like an alien wrote it . So , to be minimal in such mini project, it is already a bit faster becuase of loop unrolling. 

This is specifically designed to be used in modules of NeuralNet:: , beucase we pre - allocated buffers (so that forward pass don't have to wait for the os to allocate the memory) , and then copy the values of whatever we need to,

The first overload is meant to be used if the 2 matricies has same dtype. Otherwise the second matrix can be used for copy and cast at once.
#### Example:
```
Linalg::Matrix<float> source_1 = createMatrix<float>(4, 4, 0x001)
Linalg::Matrix<float> source_2 = createMatrix<float>(4, 4, 0x002)
Linalg::Matrix<float> source_3 = createMatrix<double>(4, 4, 0x003)
Linalg::copy_if_allocated(source_1 , source_2, 0x004); // destination must be alloacted
Linalg::copy_if_allocated(source_1 , source_3, 0x004); // 2nd overload calls!, the source 3 has the values of source 1

```
### [4] Function named fill(), Code:
```
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
```


#### Explanation: 
It fills the given matrix with a value generator. Uses loop unrolling to exclude loop overhead. 
A proper ValueGenerator type in this Linalg , is as following:


```
template<typename NumTy, // other types if needed for dependencies>
class [GeneratorName]{

private:

// dependencies

public:
// MUST use inline, because function call overhead is very high!
inline NumTy operator()(const std::size_t idx){

   NumTy res = [your transformation](dependencies, idx);
return res;
}
};
```

### Example:

```

template<typename OutTy = float>
    class ZeroGenerator{
        public:
        inline OutTy operator() (const std::size_t){
            return 0;
        } 
    };

Linalg::Matrix<float> mat = Linalg::createMatrix<float>(4, 4, 0x002);
ZeroGenerator gen;
Linalg::fill(mat, gen); // fills the matrix with 0
```


### [6] Function named transpose(), Code:
```
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
```


### Explanation: 
This is a core kenrel in the whole project . It tries to apply matrix tranpose efficiently. Check [wiki](https://en.wikipedia.org/wiki/Transpose) for understanding how exactly matrix tranpose() works.
### Args:
#### Matrix<NumberType>& m : the input matrix
#### const std::size_t rtnId = NULL_OBJECT_ID /* removed */




