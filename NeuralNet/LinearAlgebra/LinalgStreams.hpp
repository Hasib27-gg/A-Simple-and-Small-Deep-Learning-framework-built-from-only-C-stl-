#ifndef LINALG_STREAMS_HPP
#define LINALG_STREAMS_HPP
#include "Matrix.hpp"


namespace Linalg{
    template<typename NumberType>
    class RowStream{
        private:
        Linalg::Matrix<NumberType>* _Matrix;
        std::size_t _Start;
        std::size_t _ObjectId;
        public:

        inline void initStream(Matrix<NumberType>& mat,
                               const std::size_t start,
                               const std::size_t ObjId){
            self._Matrix = &mat;
            #ifndef DISABLE_VALIDATION
            if(start >= self._Matrix->getRowSize())
            {
                const std::string repr = "[ObjId: " + std::to_string(ObjId) + "][func: initStream] Start is out of range!";
                throw std::out_of_range(repr);
            }
            #endif
            self._Start = start;
            self._ObjectId = ObjId;
        }

        inline NumberType& operator[](const std::size_t idx){
            const std::size_t flat_idx = self._Start * self._Matrix->getColumnSize() + idx; 
            #ifndef DISABLE_VALIDATION
            if(self._Matrix->getData().size() <= flat_idx)
            {
                const std::string repr = "[ObjId: " + std::to_string(self._ObjectId) 
                                         + "][func: operator[]] Index is out of range!";
                throw std::out_of_range(repr);
            }
            #endif
            return self._Matrix->getData().data()[flat_idx];
        }
        inline std::size_t getNumItems(){
            return self._Matrix->getColumnSize();
        }
        inline void showStreamStatus(){
            std::cout << "(Dtype: RowStream):\n";
            std::cout << "    Self ObjectId:" + std::to_string(self._ObjectId) + '\n'; 
            std::cout << "    Traget ObjectId:" + std::to_string(self._Matrix->getObjectId()) + '\n'; 
            std::cout << "    Row no: " + std::to_string(self._Start) + '\n';
        }
        inline void showValues(){
            std::cout << "[";
            bool mask = false;
            NumberType* data = self._Matrix->getData().data();
            std::size_t flat_start = self._Start * self._Matrix->getColumnSize();
            for(std::size_t idx = 0; idx < self._Matrix->getColumnSize(); idx++){
                if(mask) std::cout << ", ";
                mask = true;
                std::cout << data[flat_start + idx];
            }
            std::cout << "]";
        }

        void getValues(FlatArray<NumberType>& return_address){
            #ifndef DISABLE_VALIDATION
            if(self.getNumItems() != return_address.size())
            {
                const std::string repr = "[ObjId: " + std::to_string(self._ObjectId) 
                                        + "][func: getValues] Size mismatch for return address!";
                throw std::out_of_range(repr);
            }
            #endif
            const NumberType* data = self._Matrix->getData().data();
            NumberType* rtn = return_address.data();
            
            const std::size_t size = self._Matrix->getColumnSize();
            const std::size_t start = self._Start * self._Matrix->getColumnSize();

            const NumberType* start_data = &data[start];
            std::memcpy(rtn, start_data, size * sizeof(NumberType)); 
                    
        }

        template<
        typename FunctionType
        >
        void mapValues(FunctionType& func){
            NumberType* data = self._Matrix->getData().data();
            const std::size_t end = self._Matrix->getColumnSize();
            const std::size_t start = self._Start * self._Matrix->getColumnSize();

            std::size_t bound = end & (~7ULL);

            #pragma omp parallel for
            for(std::size_t idx = 0; idx < bound; idx += 8){
                const std::size_t base = idx + start;
                func(data[base]);
                func(data[base + 1]);
                func(data[base + 2]);
                func(data[base + 3]);
                func(data[base + 4]);
                func(data[base + 5]);
                func(data[base + 6]);
                func(data[base + 7]);
            }

            for(std::size_t idx = bound; idx < end; idx++)
                func(data[idx + start]);
        }
        

        template<
        typename FunctionType,
        typename ReturnType
        >
        ReturnType reduceValues(FunctionType& func){
            const NumberType* data = self._Matrix->getData().data();
            const std::size_t end = self._Matrix->getColumnSize();
            const std::size_t start = self._Start * self._Matrix->getColumnSize();
            std::size_t idx = 0;
            for(; idx + 7 < end; idx += 8){
                const std::size_t base = idx + start;
                func << data[base];
                func << data[base + 1];
                func << data[base + 2];
                func << data[base + 3];
                func << data[base + 4];
                func << data[base + 5];
                func << data[base + 6];
                func << data[base + 7];
            }
            for(; idx < end; idx++)
                func << data[idx + start];
            return static_cast<ReturnType>(func());
        }


        inline std::size_t getObjectId(){
            return self._ObjectId;
        }

        

    };

    template<typename NumberType>
    class ColumnStream{
        private:
        Linalg::Matrix<NumberType>* _Matrix;
        std::size_t _Start;
        std::size_t _ObjectId;
        public:

        inline void initStream(Matrix<NumberType>& mat,
                               const std::size_t start,
                               const std::size_t ObjId){
            self._Matrix = &mat;
            #ifndef DISABLE_VALIDATION
            if(start >= self._Matrix->getColumnSize())
            {
                const std::string repr = "[ObjId: " + std::to_string(ObjId) + "][func: initStream] Start is out of range!";
                throw std::out_of_range(repr);
            }
            #endif
            self._Start = start;
            self._ObjectId = ObjId;
        }

        inline NumberType& operator[](const std::size_t idx){
            const std::size_t flat_idx = self._Start + idx * self._Matrix->getColumnSize(); 
            #ifndef DISABLE_VALIDATION
            if(self._Matrix->getData().size() <= flat_idx)
            {
                const std::string repr = "[ObjId: " + std::to_string(self._ObjectId) 
                                         + "][func: operator[]] Index is out of range!";
                throw std::out_of_range(repr);
            }
            #endif
            return self._Matrix->getData().data()[flat_idx];
        }
        inline std::size_t getNumItems(){
            return self._Matrix->getRowSize();
        }
        inline void showStreamStatus(){
            std::cout << "(Dtype: ColumnStream):\n";
            std::cout << "    Self ObjectId:" + std::to_string(self._ObjectId) + '\n'; 
            std::cout << "    Traget ObjectId:" + std::to_string(self._Matrix->getObjectId()) + '\n'; 
            std::cout << "    Row no: " + std::to_string(self._Start) + '\n';
        }
        inline void showValues(){
            std::cout << "[";
            bool mask = false;
            NumberType* data = self._Matrix->getData().data();
            std::size_t flat_start = self._Start;
            std::size_t stride = self._Matrix->getColumnSize();
            for(std::size_t idx = 0; idx < self._Matrix->getRowSize(); idx++){
                if(mask) std::cout << ", ";
                mask = true;
                std::cout << data[flat_start + idx*stride];
            }
            std::cout << "]";
        }

        void getValues(FlatArray<NumberType>& return_address){
            #ifndef DISABLE_VALIDATION
            if(self.getNumItems() != return_address.size())
            {
                const std::string repr = "[ObjId: " + std::to_string(self._ObjectId) 
                                        + "][func: getValues] Size mismatch for return address!";
                throw std::out_of_range(repr);
            }
            #endif
            const NumberType* data = self._Matrix->getData().data();
            NumberType* rtn = return_address.data();
            const std::size_t end = self._Matrix->getRowSize();
            const std::size_t start = self._Start;
            const std::size_t stride = self._Matrix->getColumnSize();
            std::size_t bound = end & (~7ULL);

            #pragma omp parallel for
            for(std::size_t idx = 0; idx < bound; idx += 8){
                const std::size_t base = idx*stride + start;
                rtn[idx]     = data[base];
                rtn[idx + 1] = data[base + 1*stride];
                rtn[idx + 2] = data[base + 2*stride];
                rtn[idx + 3] = data[base + 3*stride];
                rtn[idx + 4] = data[base + 4*stride];
                rtn[idx + 5] = data[base + 5*stride];
                rtn[idx + 6] = data[base + 6*stride];
                rtn[idx + 7] = data[base + 7*stride];
            }

            for(std::size_t idx = bound; idx < end; idx++)   
                rtn[idx]     = data[idx*stride + start];
                    
        }

        template<
        typename FunctionType
        >
        void mapValues(FunctionType& func){
            NumberType* data = self._Matrix->getData().data();
            const std::size_t end = self._Matrix->getRowSize();
            const std::size_t start = self._Start;
            const std::size_t stride = self._Matrix->getColumnSize();
            std::size_t bound = end & (~7ULL);

            #pragma omp parallel for
            for(std::size_t idx = 0; idx < bound; idx += 8){
                const std::size_t base = idx*stride + start;
                func(data[base]);
                func(data[base + 1*stride]);
                func(data[base + 2*stride]);
                func(data[base + 3*stride]);
                func(data[base + 4*stride]);
                func(data[base + 5*stride]);
                func(data[base + 6*stride]);
                func(data[base + 7*stride]);
            }

            for(std::size_t idx = bound; idx < end; idx++)
                func(data[idx*stride + start]);
        }
        

        template<
        typename FunctionType,
        typename ReturnType
        >
        ReturnType reduceValues(FunctionType& func){
            const NumberType* data = self._Matrix->getData().data();
            const std::size_t end = self._Matrix->getRowSize();
            const std::size_t start = self._Start;
            const std::size_t stride = self._Matrix->getColumnSize();
            std::size_t idx = 0;
            for(; idx + 7 < end; idx += 8){
                const std::size_t base = idx*stride + start;
                func << data[base];
                func << data[base + 1*stride];
                func << data[base + 2*stride];
                func << data[base + 3*stride];
                func << data[base + 4*stride];
                func << data[base + 5*stride];
                func << data[base + 6*stride];
                func << data[base + 7*stride];
            }
            for(; idx < end; idx++)
                func << data[idx*stride + start];
            return static_cast<ReturnType>(func());
        }


        inline std::size_t getObjectId(){
            return self._ObjectId;
        }

        

    };


    template<typename StreamType1,
             typename StreamType2,
             typename StreamType3,
             typename FuncTy>
    void zipStreamsWithAccum(
        StreamType1& s1, 
        StreamType2& s2, 
        StreamType3& rtn_addr,
        FuncTy& func
      )
    {
        #ifndef DISABLE_VALIDATION
        if (s1.getNumItems() != s2.getNumItems()){
            const std::string repr = "[func: zipStreams] Size mismatch for s1 and s2";
            throw std::out_of_range(repr);
        }
        if (s1.getNumItems() != rtn_addr.getNumItems()){
            const std::string repr = "[func: zipStreams] Size mismatch for s1 and s2";
            throw std::out_of_range(repr);
        }
        #endif


        std::size_t size = s1.getNumItems();
        std::size_t bound = size & (~7ULL);

        #pragma omp parallel for
        for(std::size_t idx = 0; idx < bound; idx += 8){
            rtn_addr[idx]     += func(s1[idx], s2[idx]);
            rtn_addr[idx + 1] += func(s1[idx + 1], s2[idx + 1]);
            rtn_addr[idx + 2] += func(s1[idx + 2], s2[idx + 2]);
            rtn_addr[idx + 3] += func(s1[idx + 3], s2[idx + 3]);
            rtn_addr[idx + 4] += func(s1[idx + 4], s2[idx + 4]);
            rtn_addr[idx + 5] += func(s1[idx + 5], s2[idx + 5]);
            rtn_addr[idx + 6] += func(s1[idx + 6], s2[idx + 6]);
            rtn_addr[idx + 7] += func(s1[idx + 7], s2[idx + 7]);
        }

        for(std::size_t idx = bound; idx < size; idx++)
          rtn_addr[idx] += func(s1[idx], s2[idx]);
    }


    template<typename StreamType1,
             typename StreamType2,
             typename StreamType3,
             typename FuncTy>
    void zipStreamsNoAccum(
        StreamType1& s1, 
        StreamType2& s2, 
        StreamType3& rtn_addr,
        FuncTy& func
      )
    {
        #ifndef DISABLE_VALIDATION
        if (s1.getNumItems() != s2.getNumItems()){
            const std::string repr = "[func: zipStreams] Size mismatch for s1 and s2";
            throw std::out_of_range(repr);
        }
        if (s1.getNumItems() != rtn_addr.getNumItems()){
            const std::string repr = "[func: zipStreams] Size mismatch for s1 and s2";
            throw std::out_of_range(repr);
        }
        #endif


        std::size_t size = s1.getNumItems();
        std::size_t bound = size & (~7ULL);

        #pragma omp parallel for
        for(std::size_t idx = 0; idx < bound; idx += 8){
            rtn_addr[idx]     = func(s1[idx], s2[idx]);
            rtn_addr[idx + 1] = func(s1[idx + 1], s2[idx + 1]);
            rtn_addr[idx + 2] = func(s1[idx + 2], s2[idx + 2]);
            rtn_addr[idx + 3] = func(s1[idx + 3], s2[idx + 3]);
            rtn_addr[idx + 4] = func(s1[idx + 4], s2[idx + 4]);
            rtn_addr[idx + 5] = func(s1[idx + 5], s2[idx + 5]);
            rtn_addr[idx + 6] = func(s1[idx + 6], s2[idx + 6]);
            rtn_addr[idx + 7] = func(s1[idx + 7], s2[idx + 7]);
        }

        for(std::size_t idx = bound; idx < size; idx++)
          rtn_addr[idx] = func(s1[idx], s2[idx]);
    }
    
}
#endif