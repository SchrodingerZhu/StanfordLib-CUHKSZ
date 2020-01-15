set -e
mkdir -p dist/libs/windows
cd build/abseil-cpp && cp absl/*/*.a .
for i in *.a; do ar -x $i; done;
ar cr libabsl.a ./*.o
ranlib libabsl.a
cp libabsl.a ../../dist/libs/windows
cd ../..
cp -r res dist
cp -r src dist
cp template-static dist/CMakeLists.txt
cp "build/libstanford-static.a" dist/libs/windows
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
zip -9 -r "x86_64-windows-mingw_w64-static.zip" dist
