#!/bin/sh

sudo docker run --rm -it   -v ${PWD}:/sources -e ENABLE_USERAPPS="Apps::ShoppingList2,Apps::ShoppingListAdmin" --user $(id -u):$(id -g)   infinitime/infinitime-build
