# Welcome to the detailed sub- documentation for the kernel Zip
## Overview:
There are exactly 3 different functions for the kernel method "zip"
### [1] Function named zip_no_accum(). Code:

```
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
```

### Basic info:
The most useful zip function of all time. In high level , it tries to apply the following formula:
Let $A$ and $B$ are two matricies shaped (m, n). Then this function performs literarly:
           $C$ = Function($A$, $B$)
Or if you like elementwise defination:
           $C[i, j]$ = Function($A[i, j]$, $B[i, j[$) for all 0 <= i <= m - 1 and 0 <= j <= n - 1
This zip is the 2nd holy baller. The usefulness of the function is unmatched. It generalizes from basic matrix operations like add, subtract, multiply, division etc. to context based kernel fusion all thanks to the help of generic functors. 

### About the code: 
Yes, we can obviously optimize this using SIMD. But since we are learning , this is just unrolled loop which is "fast enough". 
### Args:
#### Matrix<NumberType1>& mat1 : The first matrix (A)
#### Matrix<NumberType1>& mat2 : The second matrix (B)
#### Matrix<NumberType3>& res : The pre-allocated result buffer (user allocated)
#### FunctionType& func : The "Zipable" function (takes two raw value like float and return another raw_value)
#### IndexerType1& indexer1 : How to index the first one (must for broadcasting)
#### IndexerType2& indexer2 : How to index the second one (must for broadcasting)
### Returns: 
void (writes result to user allocated buffer)
### ***WARNING*** 
The zip function doesn't know the actual shape. Thus it can't also validate
### Example:


code:

```
int main() {

    Linalg::ValueGenerators::RandomGenerator<> gen;
    Linalg::Zipables::Add<float, float, float> func; // Don't worry , just a functor that adds two values
    Linalg::Matrix<float> m_1 = Linalg::createMatrix<float>(6, 6, 0x11);
    Linalg::Matrix<float> m_2 = Linalg::createMatrix<float>(6, 6, 0x11);
    Linalg::Matrix<float> res = Linalg::createMatrix<float>(6, 6, 0x11);


    Linalg::fill(m_1, gen);
    gen.setupValues(5, 6, 1);
    Linalg::fill(m_2, gen);


    Linalg::show(m_1);
    Linalg::show(m_2);
    Linalg::zip_no_accum(
        m_1,
        m_2,
        res,
        func,
        Linalg::Indexers::defaultIndexer, // a function if you have no broadcasting
        Linalg::Indexers::defaultIndexer
    );
    Linalg::show(res);


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
(Dtype: Matrix): [
    #Row[0]: [-4.52074, 5.19296, -1.2206, 0.906868, 3.39255, -3.32416]
    #Row[1]: [6.59102, -4.99837, 6.4836, -3.12584, 3.13381, 6.99411]
    #Row[2]: [6.49695, 5.03328, 3.16471, 0.687639, -1.44749, -3.47331]
    #Row[3]: [-4.61252, -4.98926, -4.45896, -2.96385, -1.07759, 1.40533]
    #Row[4]: [3.53222, 5.53484, 6.64477, 0.468828, -3.52063, -3.40653]
    #Row[5]: [0.298972, -4.97214, 4.98008, -3.03783, 6.96417, 1.62387]
], Shape: (6, 6), ObjectId: 17
(Dtype: Matrix): [
    #Row[0]: [-2.60681, 5.50532, 0.135035, 2.90589, 5.30871, -1.65194]
    #Row[1]: [7.9518, -4.05043, 7.07569, -2.87139, 3.19839, 6.9959]
    #Row[2]: [6.58712, 5.37264, 3.81845, 1.75519, -0.0254501, -1.7175]
    #Row[3]: [-2.67173, -4.07779, -4.2124, -2.69827, -0.194431, 1.40997]
    #Row[4]: [5.19557, 5.86187, 8.6388, 1.57281, -1.77749, -1.66167]
    #Row[5]: [1.40038, -2.9784, 5.3052, -1.37642, 6.96856, 2.1135]
], Shape: (6, 6), ObjectId: 17
```
### [2] Function named zip_with_accum(). Code:



```
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

```


### Basic info: 
This is just zip_no_accum() with a tiny twist in the formula:

Normal formula for zip -> $C$ = Function($A$, $B$)
Formula for this case -> $C$ = Function($A$, $B$) + $C$ 

Simply , it is used in certain situations like when you have to add to current buffer, not override it. Thus, improves code quality and makes the usage of bufferes very efficient . It is heavily used in the .backward() implementations of NeuralNet:: .

*** Documentation and coding style for this function is same as zip_no_accum() rather than the formula . So please check it before this ***
### Example:

code:
```
int main() {

    Linalg::ValueGenerators::RandomGenerator<> gen;
    Linalg::Zipables::Add<float, float, float> func; // Don't worry , just a functor that adds two values
    Linalg::Matrix<float> m_1 = Linalg::createMatrix<float>(6, 6, 0x11);
    Linalg::Matrix<float> m_2 = Linalg::createMatrix<float>(6, 6, 0x11);
    Linalg::Matrix<float> res = Linalg::createMatrix<float>(6, 6, 0x11);


    Linalg::fill(m_1, gen);
    gen.setupValues(5, 6, 1);
    Linalg::fill(m_2, gen);
    gen.setupValues(1, 3, 2);
    Linalg::fill(res, gen);

    Linalg::show(m_1);
    Linalg::show(m_2);
    Linalg::show(res);
    Linalg::zip_with_accum(
        m_1,
        m_2,
        res,
        func,
        Linalg::Indexers::defaultIndexer, // a function if you have no broadcasting
        Linalg::Indexers::defaultIndexer
    );
    Linalg::show(res);


    return EXIT_SUCCESS;
}
```


result:

```
(Dtype: Matrix): [
    #Row[0]: [1.91393, 0.31236, 1.35564, 1.99902, 1.91616, 1.67221]
    #Row[1]: [1.36079, 0.94794, 0.592085, 0.254448, 0.0645791, 0.00179017]
    #Row[2]: [0.0901733, 0.339359, 0.653735, 1.06755, 1.42204, 1.75581]
    #Row[3]: [1.9408, 0.911471, 0.246562, 0.265578, 0.883162, 0.00464344]
    #Row[4]: [1.66335, 0.327028, 1.99403, 1.10398, 1.74314, 1.74487]
    #Row[5]: [1.10141, 1.99374, 0.325118, 1.66141, 0.00439805, 0.489629]
], Shape: (6, 6), ObjectId: 17
(Dtype: Matrix): [
    #Row[0]: [-4.52074, 5.19296, -1.2206, 0.906868, 3.39255, -3.32416]
    #Row[1]: [6.59102, -4.99837, 6.4836, -3.12584, 3.13381, 6.99411]
    #Row[2]: [6.49695, 5.03328, 3.16471, 0.687639, -1.44749, -3.47331]
    #Row[3]: [-4.61252, -4.98926, -4.45896, -2.96385, -1.07759, 1.40533]
    #Row[4]: [3.53222, 5.53484, 6.64477, 0.468828, -3.52063, -3.40653]
    #Row[5]: [0.298972, -4.97214, 4.98008, -3.03783, 6.96417, 1.62387]
], Shape: (6, 6), ObjectId: 17
(Dtype: Matrix): [
    #Row[0]: [0.846574, 4.12954, -0.778274, 4.99991, -0.76037, 4.09648]
    #Row[1]: [0.8897, 1.95343, 3.19627, -0.162079, 4.79551, -0.999187]
    #Row[2]: [4.7418, -0.06292, 3.06691, 4.99705, 4.74848, 4.01664]
    #Row[3]: [3.08236, 1.84382, 0.776256, -0.236655, -0.806263, -0.99463]
    #Row[4]: [-0.72948, 0.0180766, 0.961204, 2.20266, 3.26611, 4.26742]
    #Row[5]: [4.82239, 1.73441, -0.260315, -0.203267, 1.64949, -0.98607]
], Shape: (6, 6), ObjectId: 17
(Dtype: Matrix): [
    #Row[0]: [-1.76023, 9.63486, -0.643239, 7.9058, 4.54834, 2.44454]
    #Row[1]: [8.8415, -2.097, 10.272, -3.03347, 7.9939, 5.99671]
    #Row[2]: [11.3289, 5.30972, 6.88535, 6.75225, 4.72303, 2.29914]
    #Row[3]: [0.410627, -2.23397, -3.43614, -2.93492, -1.00069, 0.415343]
    #Row[4]: [4.46609, 5.87994, 9.60001, 3.77547, 1.48862, 2.60575]
    #Row[5]: [6.22277, -1.24398, 5.04488, -1.57969, 8.61805, 1.12743]
], Shape: (6, 6), ObjectId: 17
```

### [2] Function named zip(). Code:
```


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
```

### Explanation: 
A tiny function which is simply used to make the API cleaner. 
#### Additional Arg: bool accum = false:
If this is false , it performs zip_no_accum(). Otherwise , it performs zip_with_accum(). 
