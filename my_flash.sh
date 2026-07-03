#!/bin/sh

cd ../pinetime-bluetooth-utilities &&
python3 dfu.py ../InfiniTime/build/output/pinetime-mcuboot-app-dfu-*.zip &&
cd ../InfiniTime

