#!/usr/bin/env bash
set -e
mkdir -p dist/libs/linux
cd build/abseil-cpp && cp absl/*/*.a .
for i in *.a; do ar -x $i; done;
ar cr libabsl.a ./*.o
ranlib libabsl.a
cp libabsl.a ../../dist/libs/linux
cd ../..
cp -r res dist
cp -r src dist
cp misc/template-static.cmake dist/CMakeLists.txt
cp misc/template-static.qmake dist/my_project.pro
cp misc/lib-static.conf dist/libs
cp "build/libstanford-static.a" dist/libs/linux

# Generate Header files
mkdir -p dist/includes/stanford
mkdir -p dist/includes/abseil
cp -r abseil-cpp/absl dist/includes/abseil
cd StanfordCPPLib && cp --parents */*.h ../dist/includes/stanford && cp macro.h ../dist/includes/stanford &&\
 cp images.qrc ../dist/includes/stanford && cd ..
zip -9 -r "x86_64-linux-gcc-static.zip" dist
