#!/usr/bin/env sh
set -e
mkdir -p dist/libs
cp -r res dist
cp -r src dist
cp misc/template.cmake dist/CMakeLists.txt
cp misc/template.qmake dist/my_project.pro
cp misc/lib.conf dist/libs
cp "build/libstanford.so" dist/libs

# Generate Header files
mkdir -p dist/includes/stanford
mkdir -p dist/includes/abseil
cp -r abseil-cpp/absl dist/includes/abseil
cd StanfordCPPLib && cp --parents */*.h ../dist/includes/stanford && cp macro.h ../dist/includes/stanford &&\
 cp images.qrc ../dist/includes/stanford && cd ..
zip -9 -r "x86_64-linux-gcc.zip" dist
