mkdir build\msvc2015-x64
pushd build\msvc2015-x64
cmake -G "Visual Studio 14 2015 Win64" %* ../..
popd