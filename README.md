# Basic JavaScript Compiler

A simple JavaScript compiler implementation in C++ that demonstrates the fundamental concepts of compiler design. This project serves as an educational tool to understand the basic components of a compiler.

## Features

- **Lexical Analysis**: Converts source code into tokens
- **Parsing**: Generates Abstract Syntax Tree (AST) from tokens
- **Basic JavaScript Support**:
  - Function declarations
  - Variable declarations
  - Binary operations (+, -, *, /)
  - Function calls
  - Return statements


## Prerequisites

- CMake (3.10 or higher)
- C++ Compiler with C++17 support
- Visual Studio 2019/2022 (for Windows) or GCC/Clang (for Unix)

## Building the Project

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Generate build files:
```bash
cmake ..
```

3. Build the project:
```bash
cmake --build . --config Release
```

## Usage

Run the compiler with a JavaScript file:
```bash
./js_compiler path/to/your/file.js
```

Example JavaScript input:
```javascript
function add(a, b) {
    return a + b;
}

let x = 10;
let y = 20;
let result = add(x, y);
```

## Limitations

This is a basic compiler implementation with the following limitations:

- No code optimization
- Limited JavaScript feature support
- No bytecode generation
- No runtime environment
- Basic error handling
- No support for complex JavaScript features (classes, async/await, etc.)

## Future Improvements

Potential areas for enhancement:

1. Code optimization
2. More JavaScript features support
3. Bytecode generation
4. Better error handling and reporting
5. Symbol table implementation
6. Type checking
7. Support for modern JavaScript features

## Contributing

This is an educational project demonstrating basic compiler concepts. Feel free to fork and extend it for your learning purposes.


## Acknowledgments

This project is created for educational purposes to demonstrate basic compiler construction concepts. It is not intended for production use.
