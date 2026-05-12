#ifndef LINALG_ZIPPABLES
#define LINALG_ZIPPABLES
#include "Matrix.hpp"

namespace Linalg {

namespace Zipables {

template <typename T1, typename T2, typename T3> class Add {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) + static_cast<T3>(num2);
  }
};

template <typename T1, typename T2, typename T3> class Subtract {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) - static_cast<T3>(num2);
  }
};

template <typename T1, typename T2, typename T3> class Multiply {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) * static_cast<T3>(num2);
  }
};

template <typename T1, typename T2, typename T3> class Divide {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) /(static_cast<T3>(num2) + 1e-8);
  }
};

template <typename T1, typename T2, typename T3> class Max {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return std::max(static_cast<T3>(num1), static_cast<T3>(num2));
  }
};
template <typename T1, typename T2, typename T3> class Min {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return std::min(static_cast<T3>(num1), static_cast<T3>(num2));
  }
};

template <typename T1, typename T2, typename T3> class Equals {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) == static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class NotEqual {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) != static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class Greater {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) > static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class Less {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) < static_cast<T3>(num2);
  }
};

template <typename T1, typename T2, typename T3> class GreaterEqual {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) >= static_cast<T3>(num2);
  }
};

template <typename T1, typename T2, typename T3> class LessEqual {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) <= static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class BitWiseOR {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) | static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class BitWiseXOR {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) ^ static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class LogicalOR {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) || static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class BitWiseAND {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) & static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class LogicalAND {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) && static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class Pow {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return std::pow(static_cast<T3>(num1), static_cast<T3>(num2));
  }
};
template <typename T1, typename T2, typename T3> class BitWiseLeftShift {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) << static_cast<T3>(num2);
  }
};
template <typename T1, typename T2, typename T3> class BitWiseRightShift {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) >> static_cast<T3>(num2);
  }
};

template <typename T1, typename T2, typename T3> class Reminder {

public:
  inline T3 operator()(const T1 num1, const T2 num2) {
    return static_cast<T3>(num1) % static_cast<T3>(num2);
  }
};
} // namespace Zipables
} // namespace Linalg


#endif