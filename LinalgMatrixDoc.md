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
void reshape(
    Matrix<NumberType>& matrix,
    NumPair new_shape)
```

### [9] Function:
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


### [10] Function:
```
template<typename NumberType, typename FunctionType>
void map(
    Matrix<NumberType>& matrix,
    FunctionType& func
)
```


### [11] [Zip Kernel](ZipKernelDoc.hpp)
### [12] [Reduce Kernel](ReduceKernelDoc.hpp)
### [13] [MatMul Kernel](MatMulKernelDoc.hpp)



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
Simply It just creates an Matrix<NumberType> instance, sets the row, col sizes and the object id then resizes the underlying FlatArray, finally shrinks the FlatArray (to not occupy too much memory) th[...]

#### Args:
##### - `const std::size_t rowSize` : The desired number of rows for the matrix
##### - `const std::size_t columnSize` : The desired number of columns for the matrix
##### - `const std::size_t objectId` : A unique identifier for this matrix (useful for debugging)

#### Returns:
##### - A newly created `Matrix<NumberType>` with allocated memory

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

#### Args:
##### - `Matrix<NumberType>& from` : Source matrix to copy from
##### - `Matrix<NumberType>& to` : Destination matrix to copy to (will be resized)
##### - `std::size_t newId` : New object id for the destination matrix

#### Returns:
##### - void (modifies destination matrix in-place)

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
If the destination matrix is already allocated (must have the same capacity of the source matrix), then this function copies all the values from the source matrix to the destination matrix. It us[...]

This is specifically designed to be used in modules of NeuralNet:: , beucase we pre - allocated buffers (so that forward pass don't have to wait for the os to allocate the memory) , and then copy[...]

The first overload is meant to be used if the 2 matricies has same dtype. Otherwise the second matrix can be used for copy and cast at once.

#### Args (Overload 1 - Same Type):
##### - `Matrix<NumberType>& from` : Source matrix to copy from
##### - `Matrix<NumberType>& to` : Pre-allocated destination matrix (must have same dimensions as source)
##### - `std::size_t newId` : New object id for the destination matrix

#### Args (Overload 2 - Type Conversion):
##### - `Matrix<NumberType1>& from` : Source matrix with NumberType1 elements
##### - `Matrix<NumberType2>& to` : Pre-allocated destination matrix with NumberType2 elements (casts values during copy)
##### - `std::size_t newId` : New object id for the destination matrix

#### Returns:
##### - void (modifies destination matrix in-place)

#### Example:
```
Linalg::Matrix<float> source_1 = createMatrix<float>(4, 4, 0x001)
Linalg::Matrix<float> source_2 = createMatrix<float>(4, 4, 0x002)
Linalg::Matrix<double> source_3 = createMatrix<double>(4, 4, 0x003)
Linalg::copy_if_allocated(source_1 , source_2, 0x004); // destination must be alloacted
Linalg::copy_if_allocated(source_1 , source_3, 0x004); // 2nd overload calls!, the source 3 has the values of source 1

```

### [5] Function named fill(). Code:
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

#### Args:
##### - `Matrix<NumberType>& matrix` : The matrix to fill
##### - `ValueGenerator& gen` : A functor/generator object that takes an index and returns a value of type NumberType

#### Returns:
##### - void (modifies matrix in-place)

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


### [6] Function named transpose(). Code:
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
This is a core kenrel in the whole project . Performs matrix tranpose inplace efficiently. Check [wiki](https://en.wikipedia.org/wiki/Transpose) for understanding how exactly matrix tranpose() works.

### Args:
#### - `Matrix<NumberType>& m` : The input matrix (will be transposed in-place)
#### - `const std::size_t rtnId = NULL_OBJECT_ID` : New object id for the matrix (optional, removed feature)
#### - `const std::size_t rowTile = 128` : Row tile size for cache blocking. Default is 128 (sweet spot for L1/L2 cache)
#### - `const std::size_t colTile = 128` : Column tile size for cache blocking. Default is 128 (adjust based on hardware)

### Returns:
#### - void (modifies matrix in-place)

### Detailed code exploration:
Now many things can go in the head "why use 4 nested loops". Let's derive the same thing:
In traditional sense, you may often see that the whole thing is:
```

for(i =0 ; i < r, i++)
    for (j = 0; j < c; j++)
      mat[j][i] = mat[i][j]
```

while this is clean for understanding , it is ridiculusly inefficient . The above version actually applies 2 stratigies , one of which is just loop unrolling , and the other one is ***tiling*** .[...]
### Example:


```
Linalg::Matrix<float> mat = Linalg::createMatrix<float>(4, 4, 0x001);
Linalg::ValueGenerators::RandomGenerator<> gen; // we will see later, check the doc for ValueGenerators
Linalg::fill(mat, gen);
Linalg::transpose(mat, 64, 64); // the operation
```



### [7] Function named show(). Code:
```
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
```

### What it does: 
Simply just adds a beatiful format of the matrix and it's attributes to the pre-allocated std::cout buffer using the operator <<. It is very helpful for debuging the results, losses ,weights, gradient[...]

#### Args:
##### - `Matrix<NumberType>& matrix` : The matrix to display/print

#### Returns:
##### - void (outputs to std::cout)

### Example:

Code:
```
int main() {

    Linalg::ValueGenerators::RandomGenerator<> gen;
    Linalg::Matrix<float> m = Linalg::createMatrix<float>(6, 6, 0x11);
    Linalg::fill(m, gen);
    Linalg::show(m);
    
    return EXIT_SUCCESS;
}
```
Result:
```
(Dtype: Matrix): [
    #Row[0]: [1.91393, 0.31236, 1.35564, 1.99902, 1.91616, 1.67221]
    #Row[1]: [1.36079, 0.94794, 0.592085, 0.254448, 0.0645791, 0.00179017]
    #Row[2]: [0.0901733, 0.339359, 0.653735, 1.06755, 1.42204, 1.75581]
    #Row[3]: [1.9408, 0.911471, 0.246562, 0.265578, 0.883162, 0.00464344]
    #Row[4]: [1.66335, 0.327028, 1.99403, 1.10398, 1.74314, 1.74487]
    #Row[5]: [1.10141, 1.99374, 0.325118, 1.66141, 0.00439805, 0.489629]
], Shape: (6, 6), ObjectId: 17

```

### [8] Function named reshape(). Code:
```
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
```

### What it does: 
Simply it changes the shape of the matrix. It can also be used as numpy style like if you don't know the other length , just do {new_len, -1} or {-1, new_len} . Since Linalg::Matrix already store[...]

#### Args:
##### - `Matrix<NumberType>& matrix` : The matrix whose shape will be modified
##### - `NumPair new_shape` : A pair of (rows, columns) for the new shape. Can use -1 for automatic inference (e.g., {-1, 9} to compute rows automatically)

#### Returns:
##### - void (modifies matrix shape in-place, no reallocation)

### Example:
Code:
```
int main() {

    Linalg::ValueGenerators::RandomGenerator<> gen;
    Linalg::Matrix<float> m = Linalg::createMatrix<float>(6, 6, 0x11);
    Linalg::fill(m, gen);
    std::cout << "Before Reshape:\n";
    Linalg::show(m);
    Linalg::reshape(m, {4, 9});
    std::cout << "\n\nAfter Reshape:\n";
    Linalg::show(m);
    
    return EXIT_SUCCESS;
}


```

Output:

```
Before Reshape:
(Dtype: Matrix): [
    #Row[0]: [1.91393, 0.31236, 1.35564, 1.99902, 1.91616, 1.67221]
    #Row[1]: [1.36079, 0.94794, 0.592085, 0.254448, 0.0645791, 0.00179017]
    #Row[2]: [0.0901733, 0.339359, 0.653735, 1.06755, 1.42204, 1.75581]
    #Row[3]: [1.9408, 0.911471, 0.246562, 0.265578, 0.883162, 0.00464344]
    #Row[4]: [1.66335, 0.327028, 1.99403, 1.10398, 1.74314, 1.74487]
    #Row[5]: [1.10141, 1.99374, 0.325118, 1.66141, 0.00439805, 0.489629]
], Shape: (6, 6), ObjectId: 17


After Reshape:
(Dtype: Matrix): [
    #Row[0]: [1.91393, 0.31236, 1.35564, 1.99902, 1.91616, 1.67221, 1.36079, 0.94794, 0.592085]
    #Row[1]: [0.254448, 0.0645791, 0.00179017, 0.0901733, 0.339359, 0.653735, 1.06755, 1.42204, 1.75581]
    #Row[2]: [1.9408, 0.911471, 0.246562, 0.265578, 0.883162, 0.00464344, 1.66335, 0.327028, 1.99403]
    #Row[3]: [1.10398, 1.74314, 1.74487, 1.10141, 1.99374, 0.325118, 1.66141, 0.00439805, 0.489629]
], Shape: (4, 9), ObjectId: 17
```


### [9] Function named slice(). Code:
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
```

### Args: 
##### - `Matrix<NumberType>& matrix` : The input matrix to slice from
##### - `Matrix<NumberType>& rtnAddr` : Pre-allocated output matrix (must have correct dimensions based on start, end, and strides)
##### - `const NumPair start` : Starting indices (inclusive) as {row_start, col_start}
##### - `const NumPair end` : Ending indices (exclusive) as {row_end, col_end}
##### - `const std::size_t i_stride = 1` : Step size for row iteration (1 for consecutive rows, 2 for every other row, etc.)
##### - `const std::size_t j_stride = 1` : Step size for column iteration (1 for consecutive columns, 2 for every other column, etc.)
##### - `const std::size_t row_tile = 128` : Row tile size for cache blocking (adjust based on hardware)
##### - `const std::size_t col_tile = 128` : Column tile size for cache blocking (adjust based on hardware)

### Returns:
##### - void (fills destination matrix in-place)

### Code Explanation: 
Simply, it takes a slice of the matrix . Slice starts and ends at exactly given (inclusive). It uses tiling (cache blocking) and loop unrolling to extract the values of a slice . This kernel was speci[...]

### Example:

Code:
```
int main() {

    Linalg::ValueGenerators::RandomGenerator<> gen;
    Linalg::Matrix<float> m = Linalg::createMatrix<float>(6, 6, 0x11);
    Linalg::Matrix<float> buff = Linalg::createMatrix<float>(3, 3, 0x11);
    Linalg::fill(m, gen);
    std::cout << "The source matrix:\n";
    Linalg::show(m);
    Linalg::slice(m , buff, {0, 0}, {3,3});
    std::cout << "\n\nSlice from (0, 0) to (3, 3):\n";
    Linalg::show(buff);
    
    return EXIT_SUCCESS;
}

```
Result:

```
The source matrix:
(Dtype: Matrix): [
    #Row[0]: [1.91393, 0.31236, 1.35564, 1.99902, 1.91616, 1.67221]
    #Row[1]: [1.36079, 0.94794, 0.592085, 0.254448, 0.0645791, 0.00179017]
    #Row[2]: [0.0901733, 0.339359, 0.653735, 1.06755, 1.42204, 1.75581]
    #Row[3]: [1.9408, 0.911471, 0.246562, 0.265578, 0.883162, 0.00464344]
    #Row[4]: [1.66335, 0.327028, 1.99403, 1.10398, 1.74314, 1.74487]
    #Row[5]: [1.10141, 1.99374, 0.325118, 1.66141, 0.00439805, 0.489629]
], Shape: (6, 6), ObjectId: 17


Slice from (0, 0) to (3, 3):
(Dtype: Matrix): [
    #Row[0]: [1.91393, 0.31236, 1.35564]
    #Row[1]: [1.36079, 0.94794, 0.592085]
    #Row[2]: [0.0901733, 0.339359, 0.653735]
], Shape: (3, 3), ObjectId: 17
```

### [10] The Holy Function named map(). Code:
```
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
```

### What it does: 
There are exactly 3 big ballers of the entire project. One of which is map(). Simply if you give a properly formatted functor , then it just applies that function to all the other values. Uses lo[...]

```
template<typename NumTy>
class MyFunc{

public:

// must be inlined , otherwise the runtime would be very slow
inline void operator() (NumTy& val){ ... } // take value by reference
};
```

#### Args:
##### - `Matrix<NumberType>& matrix` : The matrix to apply the function to
##### - `FunctionType& func` : A functor/function object that takes a reference to NumberType and applies an operation

#### Returns:
##### - void (modifies matrix elements in-place)

### Example:

code:
```

template <typename NumberType> class SquareRoot {
    public:
  inline void operator()(NumberType &val) { val = std::sqrt(val); }
};
int main() {

    Linalg::ValueGenerators::RandomGenerator<> gen;
    SquareRoot<float> func;
    Linalg::Matrix<float> m = Linalg::createMatrix<float>(6, 6, 0x11);
    Linalg::fill(m, gen);
    std::cout << "The source matrix:\n";
    Linalg::show(m);
    Linalg::map(m , func);
    std::cout << "\n\nAfter sqrt() is applied to each value:\n";
    Linalg::show(m);
    
    return EXIT_SUCCESS;
}

```

output:

```
The source matrix:
(Dtype: Matrix): [
    #Row[0]: [1.91393, 0.31236, 1.35564, 1.99902, 1.91616, 1.67221]
    #Row[1]: [1.36079, 0.94794, 0.592085, 0.254448, 0.0645791, 0.00179017]
    #Row[2]: [0.0901733, 0.339359, 0.653735, 1.06755, 1.42204, 1.75581]
    #Row[3]: [1.9408, 0.911471, 0.246562, 0.265578, 0.883162, 0.00464344]
    #Row[4]: [1.66335, 0.327028, 1.99403, 1.10398, 1.74314, 1.74487]
    #Row[5]: [1.10141, 1.99374, 0.325118, 1.66141, 0.00439805, 0.489629]
], Shape: (6, 6), ObjectId: 17


After sqrt() is applied to each value:
(Dtype: Matrix): [
    #Row[0]: [1.38345, 0.558892, 1.16432, 1.41387, 1.38425, 1.29314]
    #Row[1]: [1.16653, 0.973622, 0.769471, 0.504429, 0.254124, 0.0423104]
    #Row[2]: [0.300289, 0.582545, 0.808538, 1.03323, 1.19249, 1.32507]
    #Row[3]: [1.39312, 0.95471, 0.49655, 0.515342, 0.939767, 0.0681428]
    #Row[4]: [1.28971, 0.571863, 1.4121, 1.0507, 1.32028, 1.32093]
    #Row[5]: [1.04948, 1.412, 0.570191, 1.28896, 0.0663178, 0.699735]
], Shape: (6, 6), ObjectId: 17
```
