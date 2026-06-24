#!/bin/sh

mkdir -p build &&
cd build &&
cmake -G Ninja -DARM_NONE_EABI_TOOLCHAIN_PATH=/usr -DNRF5_SDK_PATH=/home/peter/nrf5sdk/nRF5_SDK_15.3.0_59ac345 -DBUILD_DFU=1 -S .. -DENABLE_USERAPPS="Apps::ShoppingList" &&
ninja -j14 pinetime-app
