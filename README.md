# PyTorch Lite

_Note: The Android port is not ready for general usage yet._

## What is PyTorch Lite?

PyTorch Lite aims to be a machine learning framework for on-device inference. We are simplifying deployment of PyTorch models on mobile devices.

## The Project

This project is my attempt to port libtorch for mobile (Android for the start). PyTorch 1.0 gained support for [using it directly from C++](https://pytorch.org/tutorials/advanced/cpp_export.html) and deploying models there.

With PyTorch Lite, you load your model and are ready to go — no more jumping through hoops; no cumbersome ONNX, no learning Caffe2. Just PyTorch and libtorch.

## Porting

It would be nice to make it easier to build libtorch such that it can be used with Android Studio's NDK. PyTorch maintainers have port libtorch for Android.

Below is the step to build mobile libtorch from source (master).

## Context

Currently the official / supported solution for mobile is PyTorch -> ONNX -> {Caffe2/CoreML/nnapi}.

That being said, [@t-vi](https://github.com/t-vi) wants to take a crack at porting libtorch to run with Android NDK. Read https://lernapparat.de/pytorch-android/ for more details.

See some of the issues or feature requests:
- [Android OSS fixes](https://github.com/pytorch/pytorch/pull/15509) to get AICamera Android app working again on PyTorch master.
- [Support the Android NDK with libtorch](https://github.com/pytorch/pytorch/issues/14258).
- [Improve `build_android.sh` experience for Caffe2](https://github.com/pytorch/pytorch/issues/13116).

PyTorch maintainers commits to the codebase before May 4 2019:
- cmake:
  - new macro `FEATURE_TORCH_MOBILE` used by libtorch mobile build to enable features that are not enabled by Caffe2 mobile build. Should only use it when necessary as the PyTorch team are committed to converging libtorch mobile and Caffe2 mobile builds and removing it eventually.
  - rename `BUILD_ATEN_MOBILE` to `INTERN_BUILD_MOBILE` and make it private.
- PR for [CMakeLists changes to enable libtorch for Android](https://github.com/pytorch/pytorch/pull/19762):

## Guide

This worked for me on Ubuntu 16.04, Python 3.6, Android NDK r19 and CMake 3.6.0.

1. git clone PyTorch source and switch to commit 3ac4d928248a4b5949b5fafe0d2f43c649cd0cd9:

```sh
git clone --recurse-submodules -j8 https://github.com/pytorch/pytorch.git
cd pytorch

# We are using this PyTorch master commit 3ac4d928248a4b5949b5fafe0d2f43c649cd0cd9 (May 8, 2019, 8:40 AM GMT+8, "tweak scripts/build_android.sh for ABI and header install")
git checkout 3ac4d928248a4b5949b5fafe0d2f43c649cd0cd9
```

2. You'll need to download the Android NDK (above r18 release) if you have not and CMake 3.6.0 or higher is required.

```sh
$ cat ~/m/dev/android/sdk/ndk-bundle/source.properties
Pkg.Desc = Android NDK
Pkg.Revision = 19.2.5345600
```

3. Then we compile libtorch for arm-v7a ABI and then for x86.

Set build environment:
- PyTorch folder is at $PYTORCH_ROOT
- This repository folder is at $AICAMERA_ROOT
- Android NDK folder is at $ANDROID_NDK

```sh
# make sure $PYTORCH_ROOT, $AICAMERA_ROOT and $ANDROID_NDK are set
export ANDROID_NDK=~/m/dev/android/sdk/ndk-bundle/
# export PYTORCH_ROOT=~/m/dev/scratch/repo/pytorch/
export AICAMERA_ROOT=~/m/dev/android/android-studio-projects/FastaiCamera/
```

Then, do the following:

```sh
# Only for my case - cmake OS level version is older than android-cmake
# nano ~/.bashrc
# Add android-cmake to PATH
# export PATH="/home/cedric/m/dev/android/sdk/cmake/3.6.4111459/bin:/home/cedric/miniconda3/bin:$PATH"

# Only for my case - activate Python environment
# workon py36mixwork
```

Build libtorch:
- This script will use Clang implicitly if NDK >= 18.
- The architecture is overridable by providing an environment variable `ANDROID_ABI`. By default, the architecture is "armeabi-v7a with NEON".
- `BUILD_CAFFE2_MOBILE` is the master switch to choose between libcaffe2 vs. libtorch mobile build. When it's enabled it builds original libcaffe2 mobile library without ATen/TH ops nor TorchScript support; When it's disabled it builds libtorch mobile library, which contains ATen/TH ops and native support for TorchScript model, but doesn't contain not-yet-unified Caffe2 ops.

```sh
build_args+=("-DBUILD_CAFFE2_MOBILE=OFF")
build_args+=("-DCMAKE_PREFIX_PATH=$(python -c 'from distutils.sysconfig import get_python_lib; print(get_python_lib())')")
build_args+=("-DPYTHON_EXECUTABLE=$(python -c 'import sys; print(sys.executable)')")
./scripts/build_android.sh "${build_args[@]}"
```

```sh
# Error experienced. Fix by changing cmake to use the Android NDK version (adding to PATH above)
Build with ANDROID_ABI[armeabi-v7a with NEON], ANDROID_NATIVE_API_LEVEL[21]
Bash: GNU bash, version 4.3.48(1)-release (x86_64-pc-linux-gnu)
Caffe2 path: /home/cedric/m/dev/scratch/repo/pytorch
Using Android NDK at /home/cedric/m/dev/android/sdk/ndk-bundle/
Android NDK version: 19
Building protoc
... ... ...
... ... ...
CMake Error at /home/cedric/m/dev/android/sdk/ndk-bundle/build/cmake/android.toolchain.cmake:38 (cmake_minimum_required):
  CMake 3.6.0 or higher is required.  You are running version 3.5.1
```

```sh
# Successful run
Build with ANDROID_ABI[armeabi-v7a with NEON], ANDROID_NATIVE_API_LEVEL[21]
Bash: GNU bash, version 4.3.48(1)-release (x86_64-pc-linux-gnu)
Caffe2 path: /home/cedric/m/dev/scratch/repo/pytorch
Using Android NDK at /home/cedric/m/dev/android/sdk/ndk-bundle/
Android NDK version: 19
Building protoc
-- Configuring done
-- Generating done
-- Build files have been written to: /home/cedric/m/dev/scratch/repo/pytorch/build_host_protoc/build
[1/1] Install the project...
-- Install configuration: ""
-- Up-to-date: /home/cedric/m/dev/scratch/repo/pytorch/build_host_protoc/lib/libprotobuf-lite.a

[796/796] Install the project...
-- Install configuration: "Release"
Installation completed, now you can copy the headers/libs from /home/cedric/m/dev/scratch/repo/pytorch/build_android/install to your Android project directory.
... ... ...
... ... ...
-- Up-to-date: /home/cedric/m/dev/scratch/repo/pytorch/build_host_protoc/lib/cmake/protobuf/protobuf-config.cmake
-- std::exception_ptr is supported.
-- NUMA is disabled
-- Turning off deprecation warning due to glog.
-- Use custom protobuf build.
-- Caffe2 protobuf include directory: $<BUILD_INTERFACE:/home/cedric/m/dev/scratch/repo/pytorch/third_party/protobuf/src>$<INSTALL_INTERFACE:include>
-- Trying to find preferred BLAS backend of choice: Eigen
-- Brace yourself, we are building NNPACK
-- NNPACK backend is neon
-- Using third party subdirectory Eigen.
CMake Warning at cmake/Dependencies.cmake:676 (find_package):
  Could not find a package configuration file provided by "pybind11" with any
  of the following names:

    pybind11Config.cmake
    pybind11-config.cmake

  Add the installation prefix of "pybind11" to CMAKE_PREFIX_PATH or set
  "pybind11_DIR" to a directory containing one of the above files.  If
  "pybind11" provides a separate development package or SDK, be sure it has
  been installed.
Call Stack (most recent call first):
  CMakeLists.txt:258 (include)


-- Could NOT find pybind11 (missing:  pybind11_INCLUDE_DIR)
-- Using third_party/pybind11.
-- Using custom protoc executable
--
-- ******** Summary ********
--   CMake version         : 3.6.0-rc2
--   CMake command         : /home/cedric/m/dev/android/sdk/cmake/3.6.4111459/bin/cmake
--   System                : Android
--   C++ compiler          : /home/cedric/m/dev/android/sdk/ndk-bundle/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++
--   C++ compiler version  : 8.0
--   CXX flags             : -g -DANDROID -fdata-sections -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes -mfpu=vfpv3-d16 -fno-addrsig -march=armv7-a -mthumb -mfpu=neon -Wa,--noexecstack -Wformat -Werror=format-security -stdlib=libc++ -frtti -fexceptions  -Wno-deprecated -fvisibility-inlines-hidden -Wnon-virtual-dtor
--   Build type            : Release
--   Compile definitions   :
--   CMAKE_PREFIX_PATH     : /home/cedric/.virtualenvs/py36mixwork/lib/python3.6/site-packages
--   CMAKE_INSTALL_PREFIX  : /home/cedric/m/dev/scratch/repo/pytorch/build_android/install
--   CMAKE_MODULE_PATH     : /home/cedric/m/dev/scratch/repo/pytorch/cmake/Modules
--
--   ONNX version          : 1.5.0
--   ONNX NAMESPACE        : onnx_c2
--   ONNX_BUILD_TESTS      : OFF
--   ONNX_BUILD_BENCHMARKS : OFF
--   ONNX_USE_LITE_PROTO   : OFF
--   ONNXIFI_DUMMY_BACKEND : OFF
--   ONNXIFI_ENABLE_EXT    : OFF
--
--   Protobuf compiler     :
--   Protobuf includes     :
--   Protobuf libraries    :
--   BUILD_ONNX_PYTHON     : OFF
--
-- ******** Summary ********
--   CMake version         : 3.6.0-rc2
--   CMake command         : /home/cedric/m/dev/android/sdk/cmake/3.6.4111459/bin/cmake
--   System                : Android
--   C++ compiler          : /home/cedric/m/dev/android/sdk/ndk-bundle/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++
--   C++ compiler version  : 8.0
--   CXX flags             : -g -DANDROID -fdata-sections -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes -mfpu=vfpv3-d16 -fno-addrsig -march=armv7-a -mthumb -mfpu=neon -Wa,--noexecstack -Wformat -Werror=format-security -stdlib=libc++ -frtti -fexceptions  -Wno-deprecated -fvisibility-inlines-hidden -Wnon-virtual-dtor
--   Build type            : Release
--   Compile definitions   :
--   CMAKE_PREFIX_PATH     : /home/cedric/.virtualenvs/py36mixwork/lib/python3.6/site-packages
--   CMAKE_INSTALL_PREFIX  : /home/cedric/m/dev/scratch/repo/pytorch/build_android/install
--   CMAKE_MODULE_PATH     : /home/cedric/m/dev/scratch/repo/pytorch/cmake/Modules
--
--   ONNX version          : 1.4.1
--   ONNX NAMESPACE        : onnx_c2
--   ONNX_BUILD_TESTS      : OFF
--   ONNX_BUILD_BENCHMARKS : OFF
--   ONNX_USE_LITE_PROTO   : OFF
--   ONNXIFI_DUMMY_BACKEND : OFF
--
--   Protobuf compiler     :
--   Protobuf includes     :
--   Protobuf libraries    :
--   BUILD_ONNX_PYTHON     : OFF
-- don't use NUMA
disabling CUDA because USE_CUDA is set false
-- Looking for clock_gettime in rt
-- Looking for clock_gettime in rt - not found
-- Looking for mmap
-- Looking for mmap - found
-- Looking for shm_open
-- Looking for shm_open - not found
-- Looking for shm_unlink
-- Looking for shm_unlink - not found
-- Looking for malloc_usable_size
-- Looking for malloc_usable_size - found
-- Looking for sys/types.h
-- Looking for sys/types.h - found
-- Looking for stdint.h
-- Looking for stdint.h - found
-- Looking for stddef.h
-- Looking for stddef.h - found
-- Check size of void*
-- Check size of void* - done
-- NCCL operators skipped due to no CUDA support
-- Excluding ideep operators as we are not using ideep
-- Excluding image processing operators due to no opencv
-- Excluding video processing operators due to no opencv
-- MPI operators skipped due to no MPI support
CMake Warning at CMakeLists.txt:457 (message):
  Generated cmake files are only fully tested if one builds with system glog,
  gflags, and protobuf.  Other settings may generate files that are not well
  tested.


CMake Warning at CMakeLists.txt:508 (message):
  Generated cmake files are only available when building shared libs.


--
-- ******** Summary ********
-- General:
--   CMake version         : 3.6.0-rc2
--   CMake command         : /home/cedric/m/dev/android/sdk/cmake/3.6.4111459/bin/cmake
--   System                : Android
--   C++ compiler          : /home/cedric/m/dev/android/sdk/ndk-bundle/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++
--   C++ compiler id       : Clang
--   C++ compiler version  : 8.0
--   BLAS                  : Eigen
--   CXX flags             : -g -DANDROID -fdata-sections -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes -mfpu=vfpv3-d16 -fno-addrsig -march=armv7-a -mthumb -mfpu=neon -Wa,--noexecstack -Wformat -Werror=format-security -stdlib=libc++ -frtti -fexceptions  -Wno-deprecated -fvisibility-inlines-hidden -O2 -fPIC -Wno-narrowing -Wall -Wextra -Wno-missing-field-initializers -Wno-type-limits -Wno-array-bounds -Wno-unknown-pragmas -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-result -Wno-strict-overflow -Wno-strict-aliasing -Wno-error=deprecated-declarations -Wno-error=pedantic -Wno-error=redundant-decls -Wno-error=old-style-cast -Wno-invalid-partial-specialization -Wno-typedef-redefinition -Wno-unknown-warning-option -Wno-unused-private-field -Wno-inconsistent-missing-override -Wno-aligned-allocation-unavailable -Wno-c++14-extensions -Wno-constexpr-not-const -Wno-missing-braces -Qunused-arguments -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-math-errno -fno-trapping-math
--   Build type            : Release
--   Compile definitions   : ONNX_NAMESPACE=onnx_c2
--   CMAKE_PREFIX_PATH     : /home/cedric/.virtualenvs/py36mixwork/lib/python3.6/site-packages
--   CMAKE_INSTALL_PREFIX  : /home/cedric/m/dev/scratch/repo/pytorch/build_android/install
--
--   TORCH_VERSION         : 1.0.0
--   CAFFE2_VERSION        : 1.0.0
--   BUILD_CAFFE2_MOBILE   : OFF
--   BUILD_ATEN_ONLY       : OFF
--   BUILD_BINARY          : OFF
--   BUILD_CUSTOM_PROTOBUF : ON
--     Protobuf compiler   :
--     Protobuf includes   :
--     Protobuf libraries  :
--   BUILD_DOCS            : OFF
--   BUILD_PYTHON          : OFF
--   BUILD_CAFFE2_OPS      : OFF
--   BUILD_SHARED_LIBS     : OFF
--   BUILD_TEST            : OFF
--   INTERN_BUILD_MOBILE   : ON
--   USE_ASAN              : OFF
--   USE_CUDA              : OFF
--   USE_ROCM              : OFF
--   USE_EIGEN_FOR_BLAS    : ON
--   USE_FBGEMM            : OFF
--   USE_FFMPEG            : OFF
--   USE_GFLAGS            : OFF
--   USE_GLOG              : OFF
--   USE_LEVELDB           : OFF
--   USE_LITE_PROTO        : OFF
--   USE_LMDB              : OFF
--   USE_METAL             : OFF
--   USE_MKL               :
--   USE_MKLDNN            :
--   USE_NCCL              : OFF
--   USE_NNPACK            : ON
--   USE_NUMPY             : ON
--   USE_OBSERVERS         : OFF
--   USE_OPENCL            : OFF
--   USE_OPENCV            : OFF
--   USE_OPENMP            : OFF
--   USE_PROF              : OFF
--   USE_QNNPACK           : ON
--   USE_REDIS             : OFF
--   USE_ROCKSDB           : OFF
--   USE_ZMQ               : OFF
--   USE_DISTRIBUTED       : OFF
--   Public Dependencies  : Threads::Threads
--   Private Dependencies : qnnpack;nnpack;cpuinfo;fp16;log;foxi_loader;dl
-- Configuring done
-- Generating done
-- Build files have been written to: /home/cedric/m/dev/scratch/repo/pytorch/build_android
Will install headers and libs to /home/cedric/m/dev/scratch/repo/pytorch/build_android/install for further Android project usage.
[71/796] Building CXX object third_party/protobuf/cmake/CMakeFiles/libprotobuf.dir/__/src/google/protobuf/util/internal/proto_writer.cc.o
In file included from /home/cedric/m/dev/scratch/repo/pytorch/third_party/protobuf/src/google/protobuf/util/internal/proto_writer.cc:31:
... ... ...
... ... ...
[796/796] Install the project...
-- Install configuration: "Release"
Installation completed, now you can copy the headers/libs from /home/cedric/m/dev/scratch/repo/pytorch/build_android/install to your Android project directory.
```

4. Test libtorch.so on Android device.

Copy them over into AICamera Android project directory:

```sh
# use my fork of AICamera
git clone https://github.com/cedrickchee/pytorch-android.git
cd pytorch-android

# copy headers
cp -r build_android/install/include/* $AICAMERA_ROOT/app/src/main/cpp/

# copy arm libs
rm -rf $AICAMERA_ROOT/app/src/main/jniLibs/armeabi-v7a/
mkdir $AICAMERA_ROOT/app/src/main/jniLibs/armeabi-v7a
cp -r build_android/lib/lib* $AICAMERA_ROOT/app/src/main/jniLibs/armeabi-v7a/
```

## Deploy Resnet-18 Model to PyTorch Lite

This is an example to get you started easily deploying a PyTorch model using the C++ frontend, libtorch and PyTorch Lite.

```sh
git clone https://github.com/cedrickchee/pytorch-lite.git

cd pytorch-lite
```

First, export your Python model using Torch Script. To do that, you have to download and install libtorch CPU distribution in your computer where you run your Jupyter Notebook.
- 1.1, cuda 9:       https://download.pytorch.org/libtorch/cu90/libtorch-shared-with-deps-latest.zip
- 1.1, cpu:          https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-latest.zip
- Nightly, cuda 10:  https://download.pytorch.org/libtorch/nightly/cu100/libtorch-shared-with-deps-latest.zip
- Nightly, cpu:      https://download.pytorch.org/libtorch/nightly/cpu/libtorch-shared-with-deps-latest.zip
- 1.0, cpu:          https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-1.0.0.zip

Open the Jupyter Notebook `CPP_Export.ipynb` in `notebooks/resnet18-app/` directory to see the full instruction for this step.

Next, build the ScriptModule (C++ application) from within the `resnet18-app/` directory:

```sh
cd notebooks/resnet18-app/

# please change your /path/to/libtorch
# where /path/to/libtorch should be the full path to the unzipped libtorch CPU distribution.
cmake -DCMAKE_PREFIX_PATH=/path/to/libtorch

# If all goes well, it will look something like this:
-- The C compiler identification is GNU 5.4.0
-- The CXX compiler identification is GNU 5.4.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found torch: /path/to/libtorch
-- Configuring done
-- Generating done
-- Build files have been written to: /home/cedric/m/dev/work/repo/pytorch-lite/notebooks/resnet18-app

make
```

## Android Development

Open AICamera using Android Studio, connect your Android device and run the project to build and deploy the APK to your devices.

## Analysis

Even if libtorch does compile for Android, it's not really optimized for ARM64 right now, and the binary size will be pretty large.

- More issues: https://github.com/pytorch/pytorch/issues/9925

---

Alternatively, build mobile libtorch using [t-vi's PyTorch fork](https://github.com/t-vi/pytorch):

```sh
git clone --recurse-submodules -j8 https://github.com/t-vi/pytorch.git t-vi-pytorch
cd t-vi-pytorch
git checkout libtorch_android
git submodule init
git submodule update

export ANDROID_NDK=~/m/dev/android/sdk/ndk-bundle/
BUILD_LIBTORCH_PY=$PWD/tools/build_libtorch.py
scripts/build_host_protoc.sh
mkdir -p android-build
pushd android-build
VERBOSE=1 DEBUG=1 python $BUILD_LIBTORCH_PY --android-abi="armeabi-v7a with NEON"
popd
```
