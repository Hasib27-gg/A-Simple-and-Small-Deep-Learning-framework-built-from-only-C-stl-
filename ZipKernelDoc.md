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
The most useful zip function of all time. In high level , it tries to apply the following formula:
$$F(A, B) = \begin{pmatrix} 
(a_{1,1}, b_{1,1}) & \dots & (a_{1,n}, b_{1,n}) \\
\vdots & \ddots & \vdots \\
(a_{m,1}, b_{m,1}) & \dots & (a_{m,n}, b_{m,n})
\end{pmatrix}$$
