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

### Windows

Open this folder in Visual Studio and configure the CMake project, or from a Developer Command Prompt:

```bat
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

Specialized CPU binaries (same names as Pikafish):

```bat
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DNEWGG_ARCH=avx2
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DNEWGG_ARCH=bmi2
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DNEWGG_ARCH=avx512
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DNEWGG_ARCH=avxvnni
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DNEWGG_ARCH=vnni512
```

Clang/MinGW is recommended for `avxvnni` and `vnni512` so the matching `-m` flags are available.

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
