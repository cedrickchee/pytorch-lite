#include <torch/script.h> // One-stop header.

#include <iostream>
#include <memory>

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: example-app <path-to-exported-script-module>\n";
        return -1;
    }

    // Deserialize the ScriptModule from a file using torch::jit::load().
    std::shared_ptr<torch::jit::script::Module> module = torch::jit::load(argv[1]);

    assert(module != nullptr);
    std::cout << "ok\n";

    // Create a vector of inputs.
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(torch::ones({1, 3, 224, 224}));

    // Execute the model and turn its output into a tensor.
    at::Tensor output = module->forward(inputs).toTensor();

    std::cout << output.slice(/*dim=*/1, /*start=*/0, /*end=*/5) << '\n';
}