mkdir build\msvc2015-x86
pushd build\msvc2015-x86
cmake -G "Visual Studio 14 2015" %* ../..
popd