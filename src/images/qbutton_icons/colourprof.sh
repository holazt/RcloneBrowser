#!/bin/bash

for f in *.png
do
 echo "Processing $f"
 # do something on $f
convert $f +profile "*" $f
done
