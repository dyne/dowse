#!/bin/bash
SIZE=$1

for i in BTN-* ; do
    T=${i%%.png} ;
    mkdir $SIZE
    convert $i -resize ${SIZE} $SIZE/${T}-${SIZE}.png ;
done
