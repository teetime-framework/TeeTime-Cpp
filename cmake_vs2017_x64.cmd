mkdir build\msvc2017-x64
pushd build\msvc2017-x64
cmake -G "Visual Studio 15 2017 Win64" %* -DTEETIME_ENABLE_TESTS=ON -DTEETIME_ENABLE_BENCHMARKS=ON ../..
popd