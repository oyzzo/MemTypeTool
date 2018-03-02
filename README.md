# MemTypeTool
MemType Tool - New GUI For managing the [MemType Offline password keeper](http://www.area0x33.com/blog/?page_id=329):
 - [Project Sources.](https://github.com/jim17/memtype)
 - [Hackaday.io Project Page and build logs.](https://hackaday.io/project/8342-memtype-open-source-password-keeper)
 - [Get a MemType from Tindie!!](https://www.tindie.com/products/area0x33/memtype-open-source-password-keeper/)


## Documentation
The documentation for the MemType can be found here:
http://www.area0x33.com/documentation/memtype.html


## Linux
Under release folder download linux64.tar.gz for a 64 bit linux.

Extract the contents:

`tar xvfz linux64.tar.gz`

And execute the launcher.

If no device is found when running the GUI, check the usb permissions. 
For ubuntu users there's a udev rules file with the right permissions:

```
sudo cp 99-MemType.rules /etc/udev/rules.d/
reboot
```

## Windows, MacOSX & Others.
There is a virtualbox Lubuntu image with MemType GUI preinstalled that runs in any OS where virtualbox can be run.

1) Install VirtualBox and the Extension pack.
https://www.virtualbox.org/

2) Download and extract (using 7-Zip) the Lubuntu image:
https://github.com/oyzzo/MemTypeTool/releases/download/1.0.1/Lubuntu.17.04.64bit.vmdk.7z

3) Create a new Linux (Ubuntu) 64bit Machine in VirtualBox, when asked for the disk open 
the extracted Lubuntu vdk image.

4) Run it and add the memtype device (Devices --> USB) to the virtual machine.

NOTE: The password for the virtualmachine user is same as the user name: memtype.

Done!

## Compiling
Compiling the sources is super easy!.

1) Install QT-Creator with QT5.5 or above.
http://www.qt.io/download-open-source

2) Download and extract the sources:
https://github.com/oyzzo/MemTypeTool/archive/master.zip

3) Open the MemtypeManager project in QT-Creator.

4) Click the build and run green play button!

You'll find the compiled binary inside a build folder.

## Keyboards
Using the GUI you can change the MemType keyboard layout to match your computer keyboard layout.
The current available layouts files are located under `keyboard` directory.

## About

* Miguel Angel Borrego
* Noel Carriquí
* [www.area0x33.com](http://www.area0x33.com)
