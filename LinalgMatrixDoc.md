# Welcome to the detailed documentation for the file "Matrix.hpp"
## Preview of the contents:

### [1] Matrix class:
#### - member explanation and thier usage.
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
