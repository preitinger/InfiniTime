#!/bin/sh

path=build_vscode

mkdir -p ${path} &&
cd ${path} &&
cmake -G Ninja -DCMAKE_C_COMPILER=/usr/bin/arm-none-eabi-gcc -DCMAKE_CXX_COMPILER=/usr/bin/arm-none-eabi-g++ CMAKE_BUILD_TYPE=Debug -DARM_NONE_EABI_TOOLCHAIN_PATH=/usr -DNRF5_SDK_PATH=/home/peter/nrf5sdk/nRF5_SDK_15.3.0_59ac345 -DCMAKE_CXX_FLAGS="-D__STDC_FORMAT_MACROS -DLOCAL_DEV_BUILD=1" -S .. -DENABLE_USERAPPS="Apps::ShoppingList2, Apps::ShoppingListAdmin" &&
ninja -j14 pinetime-app
