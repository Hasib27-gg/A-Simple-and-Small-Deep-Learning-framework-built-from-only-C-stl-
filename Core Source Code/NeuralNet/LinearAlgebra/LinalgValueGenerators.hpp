#ifndef VAL_GENS_HPP
#define VAL_GENS_HPP
#include "Matrix.hpp"
namespace Linalg {

    namespace ValueGenerators {
        template<typename OutTy = float>
    class ZeroGenerator{
        public:
        inline OutTy operator() (const std::size_t){
            return 0;
        } 
    };
    template<typename OutTy = float>
    class OneGenerator{
        
        public:
        inline OutTy operator() (const std::size_t){
            return 1;
        } 
    };
    template <typename OutTy = float>
    class RandomGenerator{
        private:

        std::size_t seed = 13;
        OutTy scaler = 1;
        OutTy shifter = 1;
        public:

        inline OutTy operator() (const std::size_t idx){
           std::size_t x = self.seed + idx; 
           x ^= x << 13;
           x ^= x >> 17;
           x ^= x << 5;

           OutTy random_val = std::sin(static_cast<OutTy>(x));
           return random_val * self.scaler + self.shifter;
        }

        inline void setupValues(
            const std::size_t seed = 13,
            const OutTy scaler = 1,
            const OutTy shifter = 1
        ){
            self.seed = seed;
            self.scaler = scaler;
            self.shifter = shifter;
        }
    };

    template<typename OutTy = float>
    class SequentialGenerator{
        
        public:
        inline OutTy operator() (const std::size_t idx){
            return idx;
        } 
    };
    }
}
#endif