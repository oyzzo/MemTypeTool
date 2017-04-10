#!/bin/bash
# Code Style formatting using indent
for i in `ls *.[c,h]`; \
do echo -n "Indenting... $i"; indent -linux -nbbo -bad -l120 -nut -i4 $i; if [ $? -eq 0 ]; then echo " [OK]"; fi;\
done

# Erase files
echo -n "Cleaning files..."
rm *~
if [ $? -eq 0 ]; then echo " [OK]"; fi;

# Compile
echo -n "Compiling..."
gcc -Wall memtype.c noekeon.c `pkg-config pkg-config --libs --cflags libusb-1.0` -o memtype
if [ $? -eq 0 ]; then echo " [OK]"; fi;
