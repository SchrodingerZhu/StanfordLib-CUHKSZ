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
