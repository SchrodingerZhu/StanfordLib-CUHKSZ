#!/usr/bin/env sh
set -e
mkdir build
cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . --target stanford-static "-j$(nproc)" && cd ..
mkdir -p dist/libs/darwin
cd build/abseil-cpp && cp absl/*/*.a .
for i in *.a; do ar -x $i; done;
ar cr libabsl.a ./*.o
ranlib libabsl.a
cp libabsl.a ../../dist/libs/darwin
cd ../..
cp -r res dist
cp -r src dist
cp misc/template-static.cmake dist/CMakeLists.txt
cp misc/template-static.qmake dist/my_project.pro
cp misc/lib-static.conf dist/libs
cp "build/libstanford-static.a" dist/libs/darwin
# Generate Header files
mkdir -p dist/includes/mimalloc
mkdir -p dist/includes/stanford
mkdir -p dist/includes/abseil
cp -r abseil-cpp/absl dist/includes/abseil
cp mimalloc/include/* dist/includes/mimalloc
cd StanfordCPPLib && rsync -R */*.h ../dist/includes/stanford && cp macro.h ../dist/includes/stanford &&\
 cp images.qrc ../dist/includes/stanford && cd ..
zip -9 -r "x86_64-darwin-clang-static.zip" dist
