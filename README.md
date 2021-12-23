# jitome

JIT-Optimized Math Expression

## Example

```cpp
#include <jitome.hpp>

int main()
{
//                      return  arg1    arg2    arg3
    jitome::JitCompiler<double, double, double, double> func("(a, b, c) {a + b * c}");

    std::cout << func(1.0, 2.0, 3.0) << std::endl; // prints 7

    return 0;
}
```

## Prerequisites & Dependency

- x64 Linux
- C++17
- [Xbyak](https://github.com/herumi/xbyak)

## Licensing terms

This product is licensed under the terms of the [MIT License](LICENSE).

- Copyright (c) 2021-now Toru Niina

All rights reserved.
