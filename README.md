# MemTypeTool
MemType Tool - New GUI For managing the [MemType Offline password keeper](http://www.area0x33.com/blog/?page_id=329):
 - [Project Sources.](https://github.com/jim17/memtype)
 - [Hackaday.io Project Page and build logs.](https://hackaday.io/project/8342-memtype-open-source-password-keeper)
 - [Get a MemType from Tindie!!](https://www.tindie.com/products/area0x33/memtype-open-source-password-keeper/)


## Documentation
The documentation for the MemType can be found here:
http://www.area0x33.com/documentation/memtype.html


## Install
Extract the release archive and run the launcher.
If no release is provided you can compile it from sources easily.

If no device is found when running the GUI, check the usb permissions. 
For ubuntu users there's a udev rules file with the right permissions:

```
sudo cp 99-MemType.rules /etc/udev/rules.d/
reboot
```


### Linux
Under release folder download linux64.tar.gz for a 64 bit linux, and linux32.tar.gz for a 32 bit linux.

Extract the contents:

`tar xvfz linux64.tar.gz`

And execute the launcher.

### MacOSX
No prebuilt package yet.
Check the Compiling instructions.

### Windows
No prebuilt package yet.
Check the Compiling instructions.

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
