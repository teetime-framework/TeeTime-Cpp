# TeeTime

## About

TeeTime is a Pipe-and-Filter Framework for C++. It is loosely based on the TeeTime reference implementation for Java, but has been adapted to C++ in many ways to support all the language features modern C++ programs rely on.
It provides support for the modeling and the execution of P&F architectures. In particular, it features:

 * a type-safe way to develop and connect stages
 * only a minimal multi-threaded overhead
 * a couple oif primitive and composite ready-to-use stages
 * threads can be assigned to stages arbitrarily
 * support for windows and linux 

## How to build

 * clone the repository
 * update all git sub modules  
 * windows:
   * run cmake_vs2015_x64.cmd (for 64bit builds) or cmake_vs2015_x86.cmd for(32bit build)
   * open solution file in build/msvc2015-x64 (or build/msvc2015-x86) with VisualStudio
 * linux
   * run run_cmake.sh <compiler> <buildtype>
     * compiler can be ''gcc'' or ''clang''
     * buildtype can be ''release'' or ''debug''
   * run make in build/<compiler>/<buildtype>
 