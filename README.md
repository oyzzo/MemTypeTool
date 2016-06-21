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
6)Download and Run zadig:
`http://zadig.akeo.ie/`
Find Memtype on zadig (may need to activate Option/List all devices) and change driver to libusb-win32:
7)Done!!! Run the MemTypeTool GUI:
`python main.py`

If you want to use it as a keyboard again, go to the Device Manager and delete drivers on it.
It will install windows drivers. Next time you want to read or write from it you'll need to repeat steps 6 and 7 ;)


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
