# MemTypeTool
MemType Tool - GUI For managing the [MemType Offline password keeper](http://www.area0x33.com/blog/?page_id=329):
 - [Project Sources.](https://github.com/jim17/memtype)
 - [Hackaday.io Project Page and build logs.](https://hackaday.io/project/8342-memtype-open-source-password-keeper)
 - [Get a MemType from Tindie!!](https://www.tindie.com/products/area0x33/memtype-open-source-password-keeper/)


![screenshot](screenshots/screenshot.png)


##Install
Make sure you have `python`, `pyusb` and `PyQt4` installed.

###Ubuntu

Ubuntu Installation:

```
apt-get install python
apt-get install python-pyqt4
pip install pyusb
```

###MacOSX
Install python and pyqt4 using brew or mac ports. Then install pyusb using pip.

###Windows
1) Install Python 2.7:
`https://www.python.org/downloads/`
2) Add python to your PATH:
`C:\Python27\;C:\Python27\Scripts\`
3) Download and Install PyQt4 (Py2.7):
`https://riverbankcomputing.com/software/pyqt/download`
4) Download, unzip and isntall pyusb:
`https://github.com/walac/pyusb/archive/master.zip`
`python setup.py install`
5)Plug in the Memtype and let windows install the drivers for the device (we'll change them in the next step).
6)Download and extract libusb-win32:
`http://sourceforge.net/projects/libusb-win32/files/libusb-win32-releases/1.2.6.0/libusb-win32-bin-1.2.6.0.zip/download`
7) Execute install-filter-win (find it inside extracted folder, under /bin/x86 or other if your windows is 64bits)
 -Select Install Filter, next.
![screenshot](screenshots/install-filter-win-1.png)
 -Select Memtype (pid a033) and click Install
![screenshot](screenshots/install-filter-win-2.png)
8)Done!!! you can check it with the testlibusb-win found next to install-filter-win:
![screenshot](screenshots/testlibusb-win.png)

##Running
To run the **MemTypeTool**:

`python main.py`

##Keyboards
Using the GUI you can change the MemType keyboard layout to match your computer keyboard layout.
The current available layouts files are located under `keyboard` directory.

##About

* Miguel Angel Borrego
* Noel Carriquí
* [www.area0x33.com](http://www.area0x33.com)
