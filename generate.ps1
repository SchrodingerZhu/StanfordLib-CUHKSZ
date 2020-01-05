mkdir build
cd build
cmake CC="C:\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\bin\gcc.exe" CXX="C:\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\bin\g++.exe" .. -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles"
cmake --build . --target stanford "-j2"
cd ..
mkdir -p dist/libs
cp -r res dist
cp -r src dist
cp template dist/CMakeLists.txt
cp "build/libstanford.dll" dist/libs
cp "build/mimalloc/libmimalloc.dll" dist/libs

mkdir -p dist/includes/mimalloc
mkdir -p dist/includes/stanford
mkdir -p dist/includes/abseil
cp -r abseil-cpp/absl dist/includes/abseil
cp mimalloc/include/* dist/includes/mimalloc
cd StanfordCPPLib
cp --parents */*.h ../dist/includes/stanford
cp macro.h ../dist/includes/stanford
cp images.qrc ../dist/includes/stanford
cd ..
zip -9 -r "dist.zip" dist
