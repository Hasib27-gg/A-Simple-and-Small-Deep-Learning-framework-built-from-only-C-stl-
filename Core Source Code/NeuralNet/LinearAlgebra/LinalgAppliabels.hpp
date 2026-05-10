#ifndef MAT_APPLIABLES_HPP
#define MAT_APPLIABLES_HPP

#include "Matrix.hpp"


namespace Linalg {
namespace Appliables {

template <typename NumberType> class Linear {
private:
  NumberType scaler;
  NumberType shifter;

public:
  inline void setScaler(const NumberType scaler) { self.scaler = scaler; }
  inline void setShifter(const NumberType shifter) { self.shifter = shifter; }
  inline void operator()(NumberType &val) {
    val = self.scaler * val + self.shifter;
  }
};

template <typename NumberType> class Quadratic {
private:
  NumberType alpha;
  NumberType beta;
  NumberType gamma;

public:
  inline void setAlpha(const NumberType alpha) { self.alpha = alpha; }

  inline void setBeta(const NumberType beta) { self.beta = beta; }

  inline void setGamma(const NumberType gamma) { self.gamma = gamma; }

  inline void operator()(NumberType &val) {
    val = self.alpha * (val * val) + self.beta * (val) + self.gamma;
  }
};

template <typename NumberType> class Add {
private:
  NumberType constant;

public:
  inline void setValue(const NumberType c) { self.constant = c; }
  inline void operator()(NumberType &val) { val += self.constant; }
};
template <typename NumberType> class Subtract {
private:
  NumberType constant;

public:
  inline void setValue(const NumberType c) { self.constant = c; }
  inline void operator()(NumberType &val) { val -= self.constant; }
};

template <typename NumberType> class Multiply {
private:
  NumberType constant;

public:
  inline void setValue(const NumberType c) { self.constant = c; }
  inline void operator()(NumberType &val) { val *= self.constant; }
};

template <typename NumberType> class Divide {
private:
  NumberType constant;

public:
  inline void setValue(const NumberType c) { self.constant = c; }
  inline void operator()(NumberType &val) { val /= self.constant; }
};

template <typename NumberType> class Square {
private:
public:
  inline void operator()(NumberType &val) { val *= val; }
};
template <typename NumberType> class Cube {

public:
  inline void operator()(NumberType &val) { val *= val * val; }
};
template <typename NumberType> class Exp {

public:
  inline void operator()(NumberType &val) { val = std::exp(val); }
};
template <typename NumberType> class NaturalLog {

public:
  inline void operator()(NumberType &val) { val = std::log(val); }
};
template <typename NumberType> class Log2 {

public:
  inline void operator()(NumberType &val) { val = std::log(val); }
};
template <typename NumberType> class Log10 {

public:
  inline void operator()(NumberType &val) { val = std::log10(val); }
};

template <typename NumberType> class Log {

  NumberType base;

public:
  inline void setBase(const NumberType base) { self.base = base; }
  inline void operator()(NumberType &val) { val = std::log(base, val); }
};
template <typename NumberType> class Abs {

public:
  inline void operator()(NumberType &val) { val = std::abs(val); }
};
template <typename NumberType> class Flip {

public:
  inline void operator()(NumberType &val) { val = val * -1; }
};
template <typename NumberType> class Inverse {

public:
  inline void operator()(NumberType &val) { val = (1 / val); }
};
template <typename NumberType> class Max {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = std::max(self.key, val); }
};
template <typename NumberType> class Min {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = std::min(self.key, val); }
};
template <typename NumberType> class Greater {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val > key; }
};
template <typename NumberType> class Less {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val < key; }
};
template <typename NumberType> class GreaterEqual {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val >= key; }
};
template <typename NumberType> class LessEqual {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val <= key; }
};
template <typename NumberType> class Euqals {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val == key; }
};
template <typename NumberType> class NotEqual {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val != key; }
};
template <typename NumberType> class LogicalOR {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val || key; }
};
template <typename NumberType> class LogicalAND {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val && key; }
};
template <typename NumberType> class BitWiseOR {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val | key; }
};
template <typename NumberType> class BitWiseAND {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val & key; }
};
template <typename NumberType> class BitWiseXOR {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val ^ key; }
};
template <typename NumberType> class BitWiseLeftShift {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val << key; }
};
template <typename NumberType> class BitWiseRightShift {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val >> key; }
};
template <typename NumberType> class Reminder {
  NumberType key;

public:
  inline void setValue(const NumberType key) { self.key = key; }
  inline void operator()(NumberType &val) { val = val % key; }
};
template <typename NumberType> class BitWiseNot {

public:
  inline void operator()(NumberType &val) { val = !val; }
};
template <typename NumberType> class Sine {
public:
  inline void operator()(NumberType &val) { val = std::sin(val); }
};
template <typename NumberType> class CoSine {
public:
  inline void operator()(NumberType &val) { val = std::cos(val); }
};
template <typename NumberType> class Tangent {
public:
  inline void operator()(NumberType &val) { val = std::tan(val); }
};
template <typename NumberType> class Secant {
public:
  inline void operator()(NumberType &val) { val = (1 / std::cos(val)); }
};
template <typename NumberType> class CoSecant {
public:
  inline void operator()(NumberType &val) { val = (1 / std::sin(val)); }
};
template <typename NumberType> class CoTangent {
public:
  inline void operator()(NumberType &val) { val = (1 / std::tan(val)); }
};

template <typename NumberType> class ArcSine {
public:
  inline void operator()(NumberType &val) { val = std::asin(val); }
};
template <typename NumberType> class ArcCoSine {
public:
  inline void operator()(NumberType &val) { val = std::acos(val); }
};
template <typename NumberType> class ArcTangent {
public:
  inline void operator()(NumberType &val) { val = std::atan(val); }
};
template <typename NumberType> class ArcSecant {
public:
  inline void operator()(NumberType &val) { val = std::acos(1 / val); }
};
template <typename NumberType> class ArcCoSecant {
public:
  inline void operator()(NumberType &val) { val = std::asin(1 / val); }
};
template <typename NumberType> class ArcCoTangent {
public:
  inline void operator()(NumberType &val) { val = std::atan(1 / val); }
};

template <typename NumberType> class SineH {
public:
  inline void operator()(NumberType &val) { val = std::sinh(val); }
};
template <typename NumberType> class CoSineH {
public:
  inline void operator()(NumberType &val) { val = std::cosh(val); }
};
template <typename NumberType> class TangentH {
public:
  inline void operator()(NumberType &val) { val = std::tanh(val); }
};
template <typename NumberType> class SecantH {
public:
  inline void operator()(NumberType &val) { val = (1 / std::cosh(val)); }
};
template <typename NumberType> class CoSecantH {
public:
  inline void operator()(NumberType &val) { val = (1 / std::sinh(val)); }
};
template <typename NumberType> class CoTangentH {
public:
  inline void operator()(NumberType &val) { val = (1 / std::tanh(val)); }
};

template <typename NumberType> class ArcSineH {
public:
  inline void operator()(NumberType &val) { val = std::asinh(val); }
};
template <typename NumberType> class ArcCoSineH {
public:
  inline void operator()(NumberType &val) { val = std::acosh(val); }
};
template <typename NumberType> class ArcTangentH {
public:
  inline void operator()(NumberType &val) { val = std::atanh(val); }
};
template <typename NumberType> class ArcSecantH {
public:
  inline void operator()(NumberType &val) { val = std::acosh(1 / val); }
};
template <typename NumberType> class ArcCoSecantH {
public:
  inline void operator()(NumberType &val) { val = std::asinh(1 / val); }
};
template <typename NumberType> class ArcCoTangentH {
public:
  inline void operator()(NumberType &val) { val = std::atanh(1 / val); }
};
template <typename NumberType> class Pow {
private:
  NumberType pow;

public:
  inline void setPow(const NumberType val) { self.pow = val; }
  inline void operator()(NumberType &val) { val = std::pow(val, self.pow); }
};
template <typename NumberType> class SquareRoot {

  inline void operator()(NumberType &val) { val = std::sqrt(val); }
};
template <typename NumberType> class CubeRoot {

  inline void operator()(NumberType &val) { val = std::cbrt(val); }
};

template <typename NumberType> class Round {

  inline void operator()(NumberType &val) { val = std::round(val); }
};

template <typename NumberType> class Ceiling {

  inline void operator()(NumberType &val) { val = std::ceil(val); }
};

template <typename NumberType> class Floor {

  inline void operator()(NumberType &val) { val = std::floor(val); }
};
template <typename NumberType> class Sign {

  inline void operator()(NumberType &val) { val = (val > 0) - (val < 0); }
};


} // namespace Appliables
} // namespace Linalg
#endif