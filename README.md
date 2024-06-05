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