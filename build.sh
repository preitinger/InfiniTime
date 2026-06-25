#!/bin/sh
python3 -m venv .venv &&
source .venv/bin/activate &&
python3 -m pip install wheel &&
python3 -m pip install -r tools/mcuboot/requirements.txt &&

mkdir -p build &&
cd build &&
cmake -G Ninja -DARM_NONE_EABI_TOOLCHAIN_PATH=/usr -DNRF5_SDK_PATH=/home/peter/nrf5sdk/nRF5_SDK_15.3.0_59ac345 -DBUILD_DFU=1 -S .. -DENABLE_USERAPPS="Apps::Calculator" &&
ninja -j14 pinetime-app &&
ninja -j14 pinetime-mcuboot-app

# mkdir -p build &&
# cd build &&
# cmake -G Ninja -DARM_NONE_EABI_TOOLCHAIN_PATH=/usr -DNRF5_SDK_PATH=/home/peter/nrf5sdk/nRF5_SDK_15.3.0_59ac345 -DBUILD_DFU=1 -DBUILD_RESOURCES=1 -S .. -DENABLE_USERAPPS="Apps::ShoppingList" &&
# ninja -j14 pinetime-app &&
# ninja -j14 pinetime-mcuboot-app
