#include "NeuralNet/LinearAlgebra/LinalgAppliabels.hpp"
#include "NeuralNet/LinearAlgebra/LinalgValueGenerators.hpp"
#include "NeuralNet/LinearAlgebra/Matrix.hpp"
#define DISABLE_VALIDATION
#include "NeuralNet/NeuralNet.hpp"
#include <random>
#include <chrono>


namespace RandomBit {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.5);
}

inline float getRandomBinary(const std::size_t) {
    return RandomBit::d(RandomBit::gen) ? 1.0f : 0.0f;
}
class Model{
public:

    NeuralNet::Linear<> linear_1;
    NeuralNet::LayerNorm<> bn1;
    NeuralNet::ELU<> elu1;

    NeuralNet::Linear<> linear_2;
    NeuralNet::LayerNorm<> bn2;
    NeuralNet::ELU<> elu2;

    NeuralNet::Linear<> linear_3;
    NeuralNet::LayerNorm<> bn3;
    NeuralNet::ELU<> elu3;

    NeuralNet::Linear<> fc;

public:

    inline void init(const Linalg::NumPair inp_shape){

        linear_1.init(inp_shape, 256, false);
        bn1.init({inp_shape.first, 256});
        elu1.init({inp_shape.first, 256});

        linear_2.init({inp_shape.first, 256}, 128, false);
        bn2.init({inp_shape.first, 128});
        elu2.init({inp_shape.first, 128});

        linear_3.init({inp_shape.first, 128}, 64, false);
        bn3.init({inp_shape.first, 64});
        elu3.init({inp_shape.first, 64});

        fc.init({inp_shape.first, 64}, 1, true);
    }

    inline Linalg::Matrix<float> forward(Linalg::Matrix<float> x){

        x = linear_1.forward(x);
        x = bn1.forward(x);
        x = elu1.forward(x);

        x = linear_2.forward(x);
        x = bn2.forward(x);
        x = elu2.forward(x);

        x = linear_3.forward(x);
        x = bn3.forward(x);
        x = elu3.forward(x);

        x = fc.forward(x);

        return x;
    }

    inline void backward(Linalg::Matrix<float> loss){

        loss = fc.backward(loss);

        loss = elu3.backward(loss);
        loss = bn3.backward(loss);
        loss = linear_3.backward(loss);

        loss = elu2.backward(loss);
        loss = bn2.backward(loss);
        loss = linear_2.backward(loss);

        loss = elu1.backward(loss);
        loss = bn1.backward(loss);
        loss = linear_1.backward(loss);
    }

    template<typename OptimizerType>
    inline void step(OptimizerType& opt){

        linear_1.step(opt);
        bn1.step(opt);

        linear_2.step(opt);
        bn2.step(opt);

        linear_3.step(opt);
        bn3.step(opt);

        fc.step(opt);
    }

    inline void clear_grad(){

        linear_1.clear_grad();
        bn1.clear_grad();

        linear_2.clear_grad();
        bn2.clear_grad();

        linear_3.clear_grad();
        bn3.clear_grad();

        fc.clear_grad();
    }

    template<typename OptimizerType>
    inline void clear_buff(OptimizerType& opt){

        opt.clear_buffer(linear_1.weight_additionals());
        opt.clear_buffer(linear_1.bias_additionals());

        opt.clear_buffer(linear_2.weight_additionals());
        opt.clear_buffer(linear_2.bias_additionals());

        opt.clear_buffer(linear_3.weight_additionals());
        opt.clear_buffer(linear_3.bias_additionals());

        opt.clear_buffer(fc.weight_additionals());
        opt.clear_buffer(fc.bias_additionals());

        opt.clear_buffer(bn1.gamma_additionals());
        opt.clear_buffer(bn1.beta_additionals());

        opt.clear_buffer(bn2.gamma_additionals());
        opt.clear_buffer(bn2.beta_additionals());

        opt.clear_buffer(bn3.gamma_additionals());
        opt.clear_buffer(bn3.beta_additionals());
    }
};




int main() {
   // ... [Your setup code for data generation] ...
    Linalg::ValueGenerators::RandomGenerator<float> gen;
    Linalg::Matrix<float> input = Linalg::createMatrix<float>(100'000, 1000, 0x001);
    Linalg::fill(input, gen);

    Linalg::Matrix<float> target = Linalg::createMatrix<float>(100'000, 1, 0x001);
    Linalg::Reductions::Mean<float, float> reduc;
    Linalg::reduce_no_accum(input , target, reduc, 1);
    
    Linalg::Appliables::Linear<float> lin;
    lin.setScaler(3.657);
    lin.setShifter(4.8);
    Linalg::map(target, lin);

    // Update inp_shape to match your 4096 batch size
    Linalg::NumPair inp_shape = {100'000, 1000};
    NeuralNet::MSE<> criterion;
    NeuralNet::AdamW<> optimizer;

    criterion.init({inp_shape.first, 1});
    optimizer.init(1e-3);
    
    Model myModel;
    myModel.init(inp_shape);

    std::size_t num_epochs = 10;

    std::cout << "Starting benchmark for " << num_epochs << " epochs..." << std::endl;

    // --- Start Timer ---
    auto start = std::chrono::high_resolution_clock::now();

    for(std::size_t epoch = 0; epoch < num_epochs; epoch++){
        myModel.clear_grad();
        auto output = myModel.forward(input);
        auto loss = criterion.forward(output, target);
        auto loss_grad = criterion.backward();
        myModel.backward(loss_grad);
        myModel.step(optimizer);

     
            std::cout << "Epoch: " << epoch << " | Loss: " << loss(0, 0) << "\n" << std::flush;
        
    }

    // --- End Timer ---
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> diff = end - start;
    double total_ms = diff.count() * 1000.0;

    std::cout << "\n\n" << std::string(30, '=') << "\n";
    std::cout << "BENCHMARK RESULTS\n";
    std::cout << std::string(30, '=') << "\n";
    std::cout << "Total Time:      " <<  diff.count() << " seconds\n";
    std::cout << "Avg per Epoch:   " << (total_ms / num_epochs) << " ms\n";
    std::cout << std::string(30, '=') << "\n";

    myModel.clear_buff(optimizer);

    return 0;
}