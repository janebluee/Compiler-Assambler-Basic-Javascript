#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/optimizer.hpp"
#include "../include/thread_pool.hpp"
#include "../include/memory_pool.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.js>" << std::endl;
        return 1;
    }

    try {
        auto start = std::chrono::high_resolution_clock::now();
        
        js::ThreadPool threadPool;
        js::MemoryPool<js::ASTNode> nodePool;
        
        std::string source = read_file(argv[1]);
        
        js::Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        js::Parser parser(tokens);
        auto ast = parser.parse();
        
        js::Optimizer optimizer;
        ast = optimizer.optimizeExpression(std::move(ast));
        
        
        std::cout << "\nOptimized AST:" << std::endl;
        ast->print();  
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Compilation successful! Time taken: " << duration.count() << "ms" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
