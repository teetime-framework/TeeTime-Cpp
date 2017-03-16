# TeeTime

This is the C++ implementation of the Pipe-and-Filter (P&F) Framework [TeeTime](http://teetime-framework.github.io/).
It is based on the [TeeTime reference implementation for Java](https://github.com/teetime-framework/TeeTime), but has been adapted to C++ in many ways to support all the language features modern C++ programs rely on.
It provides support for the modeling and the execution of P&F architectures.

## Features

 * a type-safe way to develop and connect stages
 * arbitrary pipeline branches
 * threads can be assigned to stages arbitrarily
 * only a minimal overhead in multi-threaded scenarios 
 * several ready-to-use stages for common tasks
 * create lightweight stages from functions and lambdas 
 * support for windows and linux
 * lightweight library (no dependencies, easy to integrate)

### Known limitations:	
TeeTime for C++ is currently beeing developed and tested on x86 and x64 platforms only (windows and linux). Compared to the Java version, TeeTime for C++ is limited in some ways:
 * no support for loops
 * no support for adaptive taskfarms
 * no support for composite stages
 * less ready-to-use stages in general

There are several reasons for that:
 * TeeTime for C++ is still a very young project, so not everything has been implemented yet
 * Most of the development and research for TeeTime is beeing done with the Java version, so the Java version is usually a bit ahead in terms of new features
 * Some TeeTime features of the Java version rely on features in the language or in the standard library, that are not available in C++ (like reflection or xml processing).

## Build

### Requirements

 * CMake 2.8.0
 * VisualStudio 2015 (Windows) or GCC 4.8 (Linux) 
 * clang and later versions of GCC and VisualStudio should be fine as well but have not yet been tested

### How to build

 * clone the repository from `https://github.com/teetime-framework/TeeTime-Cpp.git` recursively
 * Windows:
   * run `cmake_vs2015_x64.cmd` (for 64bit builds) or `cmake_vs2015_x86.cmd` for (for 32bit builds)
   * open generated `TeeTime.sln` solution file in `build/msvc2015-x64` (or `build/msvc2015-x86`) with VisualStudio
 * Linux
   * run `cmake_linux.sh <compiler> <buildtype>`
     * compiler can be ''gcc'' or ''clang''
     * buildtype can be ''release'' or ''debug''
   * run make in `build/<compiler>/<buildtype>` 
 
## Install

TeeTime for C++ is not distributed as a precompiled binary. If you want to use TeeTime in your project it is highly recommended to build everything from source and integrate TeeTime directly into your build. 
This way you don't have to deal with different binary versions of the library for different compilers, platforms or configurations.
TeeTime has no external dependencies <sup>1</sup>, so all you need is TeeTime itself. You can download TeeTime as a zip file from github or integrate it as git submodule into your project.

If you are using [CMake](https://cmake.org) that is particularly easy. All you have to do is add the `TeeTime-Cpp/src` directory to your build tree and make your application depend on ''teetime''. 
CMake will automatically add the include directory and link your application statically with TeeTime. There is a [TeeTime-Cpp-Examples](https://github.com/teetime-framework/TeeTime-Cpp-Examples) project that integrates TeeTime as a git submodule and demonstrates this approach.

   
[1] TeeTime uses [googletest](https://github.com/google/googletest) and [google benchmark](https://github.com/google/benchmark) (for unit testing and micro benchmarks), but those are only needed for the development of TeeTime itself. They are not required to use TeeTime.