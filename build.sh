#!/bin/bash
# Copyright (C) 2019 Moxa Inc. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

set -e
DIR_WRK=$(dirname $(readlink -e $0))

DIR_SDK=$DIR_WRK/output/sdk_aws
export DIR_ENV=$DIR_WRK/output/env_aws

TOOLCHAIN_FILE=$DIR_WRK/config/toolchain-arm-linux.cmake
SYSROOT=$DIR_ENV/arm-linux-gnueabihf
OPENSSL_INCLUDE_DIR=$SYSROOT/usr/include
OPENSSL_SSL_LIBRARY=$SYSROOT/usr/lib/arm-linux-gnueabihf/libssl.so
OPENSSL_CRYPTO_LIBRARY=$SYSROOT/usr/lib/arm-linux-gnueabihf/libcrypto.so

DIR_CMAKE=$DIR_SDK/build_cmake
BIN_SAMPLE=$DIR_WRK/sample/binary

function sdk_build()
{
    echo "-- Process: $FUNCNAME ..."
    rm -rf $DIR_CMAKE $BIN_SAMPLE
    mkdir -p $DIR_CMAKE $BIN_SAMPLE
    pushd $DIR_CMAKE > /dev/null
    cmake ../. \
        -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE \
        -DCMAKE_SYSROOT=$SYSROOT \
        -DOPENSSL_INCLUDE_DIR=$OPENSSL_INCLUDE_DIR \
        -DOPENSSL_SSL_LIBRARY=$OPENSSL_SSL_LIBRARY \
        -DOPENSSL_CRYPTO_LIBRARY=$OPENSSL_CRYPTO_LIBRARY
    make
    popd > /dev/null
    echo "-- Process: $FUNCNAME DONE!!"
}

sdk_build
