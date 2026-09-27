# NewGG

Xiangqi engine (GGzero / NewGG).

## Build with CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

The binary is `build/NewGG` on Linux/macOS, or `build/Release/NewGG.exe` with Visual Studio.

### Linux (Clang + libc++, same as the original Makefile)

```bash
sudo apt install clang libc++-dev libc++abi-dev cmake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  -DNEWGG_USE_LIBCXX=ON
cmake --build build --parallel
```

### Windows (Visual Studio / MSVC)

```bat
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

### Windows (MinGW-w64)

From an MSYS2 **MINGW64** shell (`C:\msys64\mingw64.exe`), install the toolchain if needed:

```bash
pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
```

Delete any previous `build-mingw` folder first so CMake regenerates ASCII object wrappers (required for Chinese source filenames):

```bash
rm -rf build-mingw
cmake -S . -B build-mingw -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++ \
  -DNEWGG_ARCH=avx2
cmake --build build-mingw --parallel
```

The binary is `build-mingw/NewGG.exe`. Change `NEWGG_ARCH` to `x86-64`, `bmi2`, `avx512`, `avxvnni`, or `vnni512` as needed.

### macOS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

For an Intel binary on Apple Silicon:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=x86_64
cmake --build build --parallel
```

## Makefile (Ubuntu)

Install clang and libc++, then:

```bash
make
```

### Android (NDK, arm64-v8a)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-28 \
  -DANDROID_STL=c++_static \
  -DNEWGG_ARCH=arm64
cmake --build build --parallel
```

## GitHub Actions

Push to `master`/`main` or run the **NewGG** workflow manually. It builds:

- `NewGG-Linux-x86-64`
- `NewGG-macOS-x86-64`
- `NewGG-Android-arm64`
- `NewGG-Windows-x86-64.exe`
- `NewGG-Windows-AVX2.exe`
- `NewGG-Windows-BMI2.exe`
- `NewGG-Windows-AVX512.exe`
- `NewGG-Windows-AVXVNNI.exe`
- `NewGG-Windows-VNNI512.exe`

Download the combined **NewGG** artifact from the workflow run.

点击链接加入群聊【中国象棋 GGzero】：https://jq.qq.com/?_wv=1027&k=oiEp8yOm
