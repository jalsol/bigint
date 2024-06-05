# Infinite Integer Calculator

This repository contains a simple calculator (as an REPL) that can handle infinite integers. The calculator can perform addition, subtraction, and multiplication on integers of any size.

---

## Dependencies

- C++20 (GNU GCC 12.3.0, although any compiler should work)
- CMake (any recent version)

## Building

To build the calculator, run the following commands:

```bash
cmake -S. -Bbuild
make -Cbuild repl
```

This will create an executable named `repl` in the `build` directory.

## Usage

To run the calculator, execute the `repl` executable:

```bash
./build/repl
```

Then enter expressions to evaluate. For example:

```
>>> 123456789012345678901234567890 + 987654321098765432109876543210
1111111110111111111011111111100
>>> (2 + 3) * 4
20
>>> 2*3+3*4
18
>>> (-3) * 8
-24
```
---

## Project Structure

```
├── examples
│  └── main.cpp   # Example usage of the Bigint class
└── src
   ├── bigint.hpp # The Bigint class
   ├── main.cpp   # Main file for the REPL
   ├── parser.hpp # Expression parser functions
   └── token.hpp  # Token for the expression parser
```

Each header file contains both the declaration and implementation of the classes or functions, instead of separate `.hpp` and `.cpp` files. This will be explained in more detail in the next section.

---

## Implementation

### `Bigint`

#### Storage

The `Bigint` class stores an integer as an array of digits, with each digit represented as an `int8_t`. There is also a boolean flag to indicate if the number is negative.

```cpp
class Bigint {
public:
    using DataType = int8_t;
    using Buffer = std::basic_string<DataType>;
    // ... other methods ...
private:
    // ... other methods ...
    Buffer m_buf;
    bool m_neg = false;
};
```

`std::basic_string` is used instead of `std::vector`. It enables Small String Optimization, which can improve performance for small integers (under 16 digits).

The first constraint assumes that the maximum value of all immediate calculations must be at most 999. For convenience, I chose a base that is a power of 10, where plain multiplication would not "overflow". The only suitable base is 10. Using a byte to store a number less than 10 can be considered a waste of memory, but it is a trade-off for simplicity.

#### Addition & Subtraction

Addition and subtraction are implemented using the [standard algorithms](https://en.wikipedia.org/wiki/Addition#Performing_addition), with the carry being propagated to the next digit.

The codebase implements `add_unsigned()` and `sub_unsigned()` for unsigned addition and subtraction, respectively. The signed versions of these functions are implemented by checking the signs of the operands and calling the unsigned versions accordingly.

Without the maximum value constraint, the code can be highly performant (by using base 10^9). [This submission](https://judge.yosupo.jp/submission/212814) on [yosupo's Library Checker](https://judge.yosupo.jp/submissions/?problem=addition_of_big_integers&order=%2Btime&status=AC) is currently the fastest implementation of big integers, outperforming other implementations without any complex optimizations (48ms vs. 57ms).

#### Multiplication

For simplicity, multiplication is implemented using the [standard long multiplication algorithm](https://en.wikipedia.org/wiki/Multiplication_algorithm#Long_multiplication).

However, this algorithm may be slow if the numbers are large. There are other algorithms that can be used for different cases, such as Karatsuba multiplication or Fast Fourier Transform multiplication.

#### `constexpr`-friendliness

The `Bigint` class is designed to be `constexpr`-friendly. This means that the class can be used in constant expressions, allowing the compiler to evaluate expressions at compile time.

Since `constexpr` functions are inlined, the implementation of the `Bigint` class is in the header file. This is why the class is defined in a single header file instead of a separate header and source file.

Due to the current limitations of `std::basic_string` in `constexpr` contexts, it requires a lot of hacking to calculate the result at compile time. This is why the `Bigint` class is not fully `constexpr`-friendly yet.

### Expression parsing

For parsing expressions, the [Shunting yard algorithm](https://en.wikipedia.org/wiki/Shunting_yard_algorithm) is used. It converts an infix expression to a postfix expression, which can then be used to evaluate the expression using a stack.

This algorithm eliminates the need for a tree structure to represent the expression, which can be complex to implement.

This part of the codebase is also `constexpr`-friendly, allowing the expression to be parsed at compile time. However, it can only be as much `constexpr`-friendly as the `Bigint` class allows.

