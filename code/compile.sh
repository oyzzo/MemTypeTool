#!/bin/bash
# Code Style formatting using indent
for i in `ls *.[c,h]`; \
do echo -n "Indenting... $i"; indent -linux -nbbo -bad -l120 $i; if [ $? -eq 0 ]; then echo " [OK]"; fi;\
done

# Erase files
echo -n "Cleaning files..."
rm *~
if [ $? -eq 0 ]; then echo " [OK]"; fi;

# Compile
echo -n "Compiling memtypeTest..."
gcc -Wall memtype.c noekeon.c memtypeTest.c `pkg-config pkg-config --libs --cflags libusb-1.0` -o memtypeTest
if [ $? -eq 0 ]; then echo " [OK]"; fi;
echo -n "Compiling noekeonTest..."
gcc -Wall memtype.c noekeon.c noekeonTest.c `pkg-config pkg-config --libs --cflags libusb-1.0` -o noekeonTest
if [ $? -eq 0 ]; then echo " [OK]"; fi;
