#!/usr/bin.env sh
git submodule update --init --recursive
mkdir build
cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make "-j$(nproc)" && cd ..
mkdir -p dist/libs
cp -r res dist
cp -r src dist
cp template dist/CMakeLists.txt
cp build/libstanford.so dist/libs
cp build/mimalloc/libmimalloc.so dist/libs
cd dist/libs && ln -s libmimalloc.so libmimalloc.so.1.2 && cd ../..

# Generate Header files
mkdir -p dist/includes/mimalloc
mkdir -p dist/includes/stanford
mkdir -p dist/includes/abseil
cp -r abseil-cpp/absl dist/includes/abseil
cp mimalloc/include/* dist/includes/mimalloc
cd StanfordCPPLib && cp --parents */*.h ../dist/includes/stanford && cp macro.h ../dist/includes/stanford &&\
 cp images.qrc ../dist/includes/stanford && cd ..
zip -9 -r "$(uname)-dist.zip" dist
