#!/bin/sh

rm -rf build/output/*.zip
. my_build_docker.sh &&
./my_flash.sh &&
read -p "Warte, bis die Uhr gebootet hat. Drücke [Enter] zum Synchronisieren..." &&
./my_setTime.sh
