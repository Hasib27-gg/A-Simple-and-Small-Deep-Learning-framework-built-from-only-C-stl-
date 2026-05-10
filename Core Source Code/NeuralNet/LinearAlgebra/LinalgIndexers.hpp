#ifndef INDEXERS_HPP
#define INDEXERS_HPP

#include "Matrix.hpp"
namespace Linalg {
namespace Indexers {
inline std::size_t defaultIndexer(const std::size_t index){
    return index;
}

class RowRepeater{
  private:
  std::size_t len;
  public:

  inline void setColumnSize(const std::size_t len){
    self.len = len;
  }
  inline std::size_t operator()(const std::size_t idx){
    return idx % len;
  }
};
class ColumnRepeater{
  private:
  std::size_t len;
  public:

  inline void setColumnSize(const std::size_t len){
    self.len = len;
  }
  inline std::size_t operator()(const std::size_t idx){
    return idx/len;
  }
};
}


}


#endif