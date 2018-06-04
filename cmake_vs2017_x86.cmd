mkdir build\msvc2017-x86
pushd build\msvc2017-x86
cmake -G "Visual Studio 15 2017" %*  -DTEETIME_ENABLE_TESTS=ON -DTEETIME_ENABLE_BENCHMARKS=ON ../..
popd