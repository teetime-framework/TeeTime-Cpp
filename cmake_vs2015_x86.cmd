mkdir build\msvc2015-x86
pushd build\msvc2015-x86
cmake -G "Visual Studio 14 2015" %*  -DTEETIME_ENABLE_TESTS=ON -DTEETIME_ENABLE_BENCHMARKS=ON ../..
popd