#!/bin/bash

# Compile macOS dependencies needed to compile on macOS
# Previously, you should install :
# - XCode
# - CMake

function check_tools {
  if hash xcode-select 2>/dev/null; then
    echo "Found Xcode"
  else
    echo "Missing Xcode "$@""
    exit
  fi

  if hash curl 2>/dev/null; then
    echo "Found curl"
  else
    echo "Missing curl "$@""
    exit
  fi

  if hash cmake 2>/dev/null; then
    echo "Found cmake"
  else
    echo "Missing cmake "$@""
    exit
  fi
}

function build_portaudio {
    echo "Building portaudio"
    rm -rf portaudio

    git clone -b v19.7.0 --single-branch --depth 1 https://github.com/PortAudio/portaudio.git

    pushd portaudio

    mkdir build_cmake && cd build_cmake

    cmake -G Xcode \
        -DCMAKE_INSTALL_PREFIX=${LOCAL_DIR}/portaudio \
        -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
        -DBUILD_SHARED_LIBS=OFF \
        ..

    rm -rf ${LOCAL_DIR}/portaudio
    cmake --build . --config Release --target install

    popd

    rm -rf portaudio
}

function build_openssl {
    echo "Building openssl_cmake"
    rm -rf openssl-cmake-1.1.1n-20220327
    rm -f 1.1.1n-20220327.tar.gz
    curl -L https://github.com/janbar/openssl-cmake/archive/refs/tags/1.1.1n-20220327.tar.gz -O
    tar zxf 1.1.1n-20220327.tar.gz

    pushd openssl-cmake-1.1.1n-20220327

    mkdir build && cd build

    cmake -G Xcode \
        -DCMAKE_INSTALL_PREFIX=${LOCAL_DIR}/openssl \
        -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
        -DBUILD_SHARED_LIBS=OFF \
        ..

    rm -rf ${LOCAL_DIR}/openssl
    cmake --build . --config Release --target install

    popd
    
    rm -rf openssl-cmake-1.1.1n-20220327
    rm -f 1.1.1n-20220327.tar.gz
}

mkdir tmp
pushd tmp

# Where we install all our dependencies
export LOCAL_DIR=${HOME}/local
mkdir -p ${LOCAL_DIR}

export MACOSX_DEPLOYMENT_TARGET="10.14"

check_tools
build_openssl
build_portaudio
