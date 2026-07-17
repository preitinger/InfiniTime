#!/bin/sh

ALL_APPS=Apps::ShoppingList2,Apps::ShoppingListAdmin,Apps::Timer,Apps::Alarm,Apps::Music,Apps::Paint,Apps::HeartRate,Apps::Calculator,Apps::Steps,Apps::Dice,Apps::Weather
ONLY_MY_APPS=Apps::ShoppingList2,Apps::ShoppingListAdmin
sudo docker run --rm -it   -v ${PWD}:/sources -e ENABLE_USERAPPS="$ALL_APPS" --user $(id -u):$(id -g)   infinitime/infinitime-build
