mkdir build\msvc2015-x64
pushd build\msvc2015-x64
cmake -G "Visual Studio 14 2015 Win64" %*  -DTEETIME_ENABLE_TESTS=ON -DTEETIME_ENABLE_BENCHMARKS=ON ../..
popd