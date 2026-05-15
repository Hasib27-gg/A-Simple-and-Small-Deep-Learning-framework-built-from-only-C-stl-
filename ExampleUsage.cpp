#include "NeuralNet/LinearAlgebra/LinalgAppliabels.hpp"
#include "NeuralNet/LinearAlgebra/LinalgValueGenerators.hpp"
#include "NeuralNet/LinearAlgebra/Matrix.hpp"
#define DISABLE_VALIDATION
#include "NeuralNet/NeuralNet.hpp"
#include <random>
#include <chrono>

class Model{
public:

    NeuralNet::Linear<> linear_1;
    NeuralNet::LayerNorm<> ln1;
    NeuralNet::ELU<> elu1;

    NeuralNet::Linear<> linear_2;
    NeuralNet::LayerNorm<> ln2;
    NeuralNet::ELU<> elu2;

    NeuralNet::Linear<> linear_3;
    NeuralNet::LayerNorm<> ln3;
    NeuralNet::ELU<> elu3;

    NeuralNet::Linear<> fc;

public:

    inline void init(const Linalg::NumPair inp_shape){

        linear_1.init(inp_shape, 256, false);
        ln1.init({inp_shape.first, 256});
        elu1.init({inp_shape.first, 256});

        linear_2.init({inp_shape.first, 256}, 128, false);
        ln2.init({inp_shape.first, 128});
        elu2.init({inp_shape.first, 128});

        linear_3.init({inp_shape.first, 128}, 64, false);
        ln3.init({inp_shape.first, 64});
        elu3.init({inp_shape.first, 64});

        fc.init({inp_shape.first, 64}, 1, true);
    }


    // forward and backward impl
    inline Linalg::Matrix<float> forward(Linalg::Matrix<float> x){

        x = linear_1.forward(x);
        x = ln1.forward(x);
        x = elu1.forward(x);

        x = linear_2.forward(x);
        x = ln2.forward(x);
        x = elu2.forward(x);

        x = linear_3.forward(x);
        x = ln3.forward(x);
        x = elu3.forward(x);

        x = fc.forward(x);

        return x;
    }
  
    inline void backward(Linalg::Matrix<float> loss){

        loss = fc.backward(loss);
        
        loss = elu3.backward(loss);
        loss = ln3.backward(loss);
        loss = linear_3.backward(loss);

        loss = elu2.backward(loss);
        loss = ln2.backward(loss);
        loss = linear_2.backward(loss);

        loss = elu1.backward(loss);
        loss = ln1.backward(loss);
        loss = linear_1.backward(loss);
    }

    template<typename OptimizerType>
    inline void step(OptimizerType& opt){

        linear_1.step(opt);
        ln1.step(opt);

        linear_2.step(opt);
        ln2.step(opt);

        linear_3.step(opt);
        ln3.step(opt);

        fc.step(opt);
    }

    // this function helps to make all the gradients 0
    inline void clear_grad(){

        linear_1.clear_grad();
        ln1.clear_grad();

        linear_2.clear_grad();
        ln2.clear_grad();

        linear_3.clear_grad();
        ln3.clear_grad();

        fc.clear_grad();
    }

    // A helper function to clear the optimizer states 
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

        opt.clear_buffer(ln1.gamma_additionals());
        opt.clear_buffer(ln1.beta_additionals());

        opt.clear_buffer(ln2.gamma_additionals());
        opt.clear_buffer(ln2.beta_additionals());

        opt.clear_buffer(ln3.gamma_additionals());
        opt.clear_buffer(ln3.beta_additionals());
    }
};




int main() {
    // Dummy heavy load
    Linalg::ValueGenerators::RandomGenerator<float> gen;
    Linalg::Matrix<float> input = Linalg::createMatrix<float>(100'000, 1000, 0x001);
    Linalg::fill(input, gen);

    // Defining linear target
    Linalg::Matrix<float> target = Linalg::createMatrix<float>(100'000, 1, 0x001);
    Linalg::Reductions::Mean<float, float> reduc;
    Linalg::reduce_no_accum(input , target, reduc, 1);
    
    Linalg::Appliables::Linear<float> lin;
    lin.setScaler(3.657);
    lin.setShifter(4.8);
    Linalg::map(target, lin);

    // Create the optimizers
    Linalg::NumPair inp_shape = {100'000, 1000};
    NeuralNet::MSE<> criterion;
    NeuralNet::AdamW<> optimizer;
    
    criterion.init({inp_shape.first, 1});
    optimizer.init(1e-3);
    
    // model instance
    Model myModel;
    myModel.init(inp_shape);

    std::size_t num_epochs = 10;

    std::cout << "Starting benchmark for " << num_epochs << " epochs..." << std::endl;

    // benchmark the training, see the example_result.txt to find the result
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

    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> diff = end - start;
    double total_ms = diff.count() * 1000.0;

    std::cout << "\n\n" << std::string(30, '=') << "\n";
    std::cout << "BENCHMARK RESULTS\n";
    std::cout << std::string(30, '=') << "\n";
    std::cout << "Total Time:      " <<  diff.count() << " seconds\n";
    std::cout << "Avg per Epoch:   " << (total_ms / num_epochs) << " ms\n";
    std::cout << std::string(30, '=') << "\n";

    // CLEAR OPTIMIZER STATES , VERY IMPORTATNT
    myModel.clear_buff(optimizer);

    return 0;
}
