#ifndef LINALG_REDUCTIONS_HPP
#define LINALG_REDUCTIONS_HPP


#include "Matrix.hpp"

namespace Linalg {
namespace Reductions {
template <typename NumberType, typename ReturnType> class Sum {
private:
  ReturnType acc;

public:
  inline void initialize(const NumberType value) {
    acc = static_cast<ReturnType>(value);
  }
  inline void operator<<(const NumberType value) {
    acc += static_cast<ReturnType>(value);
  }
  inline ReturnType operator()() { return acc; }
};
template <typename NumberType, typename ReturnType> class SumOfSquares {
private:
  ReturnType acc;

public:
  inline void initialize(const NumberType value) {
    acc = static_cast<ReturnType>(value) * static_cast<ReturnType>(value);
  }
  inline void operator<<(const NumberType value) {
    acc += static_cast<ReturnType>(value) * static_cast<ReturnType>(value);
  }
  inline ReturnType operator()() { return acc; }
};
template <typename NumberType, typename ReturnType> class NormEuclid {
private:
  ReturnType acc;

public:
  inline void initialize(const NumberType value) {
    acc = static_cast<ReturnType>(value) * static_cast<ReturnType>(value);
  }
  inline void operator<<(const NumberType value) {
    acc += static_cast<ReturnType>(value) * static_cast<ReturnType>(value);
  }
  inline ReturnType operator()() { return std::sqrt(acc); }
};
template <typename NumberType, typename ReturnType> class NormManhattan {
private:
  ReturnType acc;

public:
  inline void initialize(const NumberType value) {
    acc = std::abs(static_cast<ReturnType>(value));
  }
  inline void operator<<(const NumberType value) {
    acc += std::abs(static_cast<ReturnType>(value));
  }
  inline ReturnType operator()() { return acc; }
};
template <typename NumberType, typename ReturnType> class NormMinkowski {
private:
  ReturnType acc;
  ReturnType val;

public:
  inline void setPowValue(const NumberType value) { self.val = value; }
  inline void initialize(const NumberType value) {
    acc = std::pow(std::abs(static_cast<ReturnType>(value)), self.val);
  }
  inline void operator<<(const NumberType value) {
    acc += std::pow(std::abs(static_cast<ReturnType>(value)), self.val);
  }
  inline ReturnType operator()() { return acc; }
};
template <typename NumberType, typename ReturnType> class SumOfPow {
private:
  ReturnType acc;
  ReturnType val;

public:
  inline void setPowValue(const NumberType value) { self.val = value; }
  inline void initialize(const NumberType value) {
    acc = std::pow(value, self.val);
  }
  inline void operator<<(const NumberType value) {
    acc += std::pow(value, self.val);
  }
  inline ReturnType operator()() { return acc; }
};

template <typename NumberType, typename ReturnType,
          typename CounterType = std::size_t>
class Mean {
private:
  ReturnType acc;
  CounterType count;

public:
  inline void initialize(const NumberType value) {
    acc = static_cast<ReturnType>(value);
    count = 1;
  }
  inline void operator<<(const NumberType value) {
    acc += static_cast<ReturnType>(value);
    count++;
  }
  inline ReturnType operator()() { return acc / count; }
};
template <typename NumberType, typename ReturnType> class Max {
private:
  ReturnType running;

public:
  inline void initialize(const NumberType value) {
    running = static_cast<ReturnType>(value);
  }
  inline void operator<<(const NumberType value) {
    running = std::max(static_cast<ReturnType>(value), running);
  }
  inline ReturnType operator()() { return running; }
};
template <typename NumberType, typename ReturnType> class Range {
private:
  ReturnType runningMin;
  ReturnType runningMax;

public:
  inline void initialize(const NumberType value) {
    runningMin = static_cast<ReturnType>(value);
    runningMax = runningMin;
  }
  inline void operator<<(const NumberType value) {
    runningMin = std::min(static_cast<ReturnType>(value), runningMin);
    runningMax = std::max(static_cast<ReturnType>(value), runningMax);
  }
  inline ReturnType operator()() { return runningMax - runningMin; }
};
template <typename NumberType, typename ReturnType> class Min {
private:
  ReturnType running;

public:
  inline void initialize(const NumberType value) {
    running = static_cast<ReturnType>(value);
  }
  inline void operator<<(const NumberType value) {
    running = std::min(static_cast<ReturnType>(value), running);
  }
  inline ReturnType operator()() { return running; }
};
template <typename NumberType, typename ReturnType> class All {
private:
  ReturnType running = 0;

public:
  inline void initialize(const NumberType value) { running = value && running; }
  inline void operator<<(const NumberType value) { running = value && running; }
  inline ReturnType operator()() { return running; }
};
template <typename NumberType, typename ReturnType> class Any {
private:
  ReturnType running = 0;

public:
  inline void initialize(const NumberType value) { running = value || running; }
  inline void operator<<(const NumberType value) { running = value || running; }
  inline ReturnType operator()() { return running; }
};

template <typename NumberType, typename ReturnType,
          typename CounterType = std::size_t>
class ArgMax {
private:
  NumberType running;
  CounterType curr;
  ReturnType running_pos;

public:
  inline void initialize(const NumberType value) {
    running = value;
    curr = 0;
    running_pos = 0;
  }
  inline void operator<<(const NumberType value) {
    curr++;
    if (value > running) {
      running = value;
      running_pos = static_cast<ReturnType>(curr);
    }
  }
  inline ReturnType operator()() { return running_pos; }
};

template <typename NumberType, typename ReturnType,
          typename CounterType = std::size_t>
class ArgMin {
private:
  NumberType running;
  CounterType curr;
  ReturnType running_pos;

public:
  inline void initialize(const NumberType value) {
    running = value;
    curr = 0;
    running_pos = 0;
  }
  inline void operator<<(const NumberType value) {
    curr++;
    if (value < running) {
      running = value;
      running_pos = static_cast<ReturnType>(curr);
    }
  }
  inline ReturnType operator()() { return running_pos; }
};
template <typename NumberType, typename ReturnType,
          typename CounterType = std::size_t>
class Variance {
  ReturnType runningMean = 0;
  ReturnType runningVar = 0;
  CounterType count = 0;

public:
  inline void initialize(const NumberType value) {
    count = 1;
    runningMean = static_cast<ReturnType>(value);
    runningVar = 0;
  }

  inline void operator<<(const NumberType value) {
    count++;
    ReturnType delta = static_cast<ReturnType>(value) - runningMean;
    runningMean += delta / static_cast<ReturnType>(count);
    ReturnType delta_2 = static_cast<ReturnType>(value) - runningMean;
    runningVar += delta * delta_2;
  }

  inline ReturnType operator()() { return runningVar/count; }
};
template <typename NumberType, typename ReturnType,
          typename CounterType = std::size_t>
class Stdev {
  ReturnType runningMean = 0;
  ReturnType runningVar = 0;
  CounterType count = 0;

public:
  inline void initialize(const NumberType value) {
    count = 1;
    runningMean = static_cast<ReturnType>(value);
    runningVar = 0;
  }

  inline void operator<<(const NumberType value) {
    count++;
    ReturnType delta = static_cast<ReturnType>(value) - runningMean;
    runningMean += delta / static_cast<ReturnType>(count);
    ReturnType delta_2 = static_cast<ReturnType>(value) - runningMean;
    runningVar += delta * delta_2;
  }

  inline ReturnType operator()() { return std::sqrt((runningVar + 1e-5)/count); }
};
} // namespace Reductions
} // namespace Linalg

#endif