# jitome

**JIT**-**O**ptimized **M**ath **E**xpression

## Example

```cpp
#include <jitome.hpp>

int main()
{
    std::string func_body = "(a, b, c) {a + b * c}";

    jitome::JitCompiler<double(double, double, double)> func(func_body);

    std::cout << func(1.0, 2.0, 3.0) << std::endl; // prints 7

    return 0;
}
```

## Prerequisites & Dependency

- x64 Linux
- C++17
- [Xbyak](https://github.com/herumi/xbyak)

## Syntax

See [syntax.abnf](syntax.abnf).

## Licensing terms

This product is licensed under the terms of the [MIT License](LICENSE).

- Copyright (c) 2021-now Toru Niina

All rights reserved.
