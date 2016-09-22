import sys
import json
import os, random, struct, hashlib
from Crypto.Cipher import AES
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from leftMenuClass import *
from credListClass import *
from credEditClass import *

from libsmttool.memtype import *

GUI_VERSION = "0.4"

class Window(QMainWindow):
    """This class creates a main window"""

    cl = []

    #Constructor
    def __init__(self):
        super(QMainWindow,self).__init__() #Call super class constructor

        #Set Title
        self.setWindowTitle("MtTool - area0x33")

        #Create Top header
        self.topFont = QFont("Helvetica", 32, QFont.Bold)
        self.topPalette = QPalette()
        self.topPalette.setColor(QPalette.Foreground,Qt.white)
        self.topLayout = QHBoxLayout()
        self.topLabel = QLabel()
        self.topLabel.setText("MemType Tool")
        self.topLabel.setFont(self.topFont)
        self.topLabel.setPalette(self.topPalette)
        self.topLabel.setAlignment(Qt.AlignLeft)

        self.topLogo = QLabel()
        self.topLogo.setPixmap(QPixmap("img/logo.png"))
        self.topLogo.setAlignment(Qt.AlignRight)
        self.topLayout.addWidget(self.topLabel)
        self.topLayout.addWidget(self.topLogo)

        self.topWidget = QWidget()
        self.topWidget.setStyleSheet("background-color:darkgrey;")
        self.topWidget.setLayout(self.topLayout)

        #Create left main menu
        self.menu = leftMenu((("Read","img/read.png"),("Write","img/write.png"), ("Set PIN","img/pin.png"), ("Import File","img/open.png"), ("Export File","img/save.png"),("Set KeyLayout","img/keylayout.png")))

        self.menu.menuClicked.connect(self.menuClicked)
        #Create Center Layout
        self.centLayout = QVBoxLayout()
        self.centLayout.addWidget(self.menu)

        #Create console edit box
        self.consLayout = QVBoxLayout()
        self.consLayout.setMargin(0)
        self.consLayout.setSpacing(0)

        self.console = QLineEdit()
        self.console.setStyleSheet("color: white; background-color: black ;border:none;")
        self.consLayout.addWidget(self.console)


        #Create a status bar
        self.statBar = QStatusBar()
        self.statBar.setStyleSheet("color: rgb(127,127,127); background-color: black")
        self.consLayout.addWidget(self.statBar)

        #Credential List on the center
        self.setListMode()

        #Create Main Layout
        self.mainLayout = QVBoxLayout()
        self.mainLayout.addWidget(self.topWidget)
        self.mainLayout.addLayout(self.centLayout)
        self.mainLayout.addLayout(self.consLayout)
        self.mainLayout.setMargin(0)


        self.mainWidget = QWidget()
        self.mainWidget.setLayout(self.mainLayout)
        self.setCentralWidget(self.mainWidget)

        #Focus
        self.console.setFocus()
        self.statBar.showMessage(GUI_VERSION)

    def setEditMode(self):
        """Set window in edit mode"""
        try:
            self.centCredList.setParent(None)
        except AttributeError:
            pass
        self.centCredEdit = credEdit()
        self.centLayout.addWidget(self.centCredEdit)

        self.centCredEdit.okPressed.connect(self.editOk)
        self.centCredEdit.cancelPressed.connect(self.editCancel)



    def setListMode(self):
        """Set window in  list mode"""
        try:
            self.centCredEdit.setParent(None)
        except AttributeError:
            pass
        self.centCredList = credList()
        self.centCredList.newCredential.connect(self.addNewCredential)
        self.centCredList.deleteClicked.connect(self.deleteClicked)
        self.centCredList.openClicked.connect(self.openClicked)
        self.centLayout.addWidget(self.centCredList)

        #update the credentials on the list
        for i,cr in enumerate(self.cl):
            self.centCredList.addCredential(cr.name,i)

        #Set Focus
        self.console.setFocus(True)



    def editOk(self):
        """This method is called when confirming edition of a credential"""

        #Get edited credential and its possition
        (cred,pos) = self.centCredEdit.getCredential()
        #Update it on the list
        self.cl[pos]=cred
        #And open list mode
        self.setListMode()
        #Set Focus
        self.console.setFocus(True)



    def editCancel(self):
        """This method is called when canceled edition of a credential"""
        #Nothing to do, return to list mode
        self.setListMode()
        #Set Focus
        self.console.setFocus(True)


    def addNewCredential(self):
        """ Add credential to self.cl"""
        cred = credential()
        cred.name = "New Credential"
        cred.user = "user"
        cred.hop = "\t"
        cred.passw = "P455W0RD"
        cred.submit = "\n"
        self.cl.append(cred)
        self.centCredList.clearCredentials()
        for i,cr in enumerate(self.cl):
            self.centCredList.addCredential(cr.name,i)


    def showErrorMessage(self,msg):
        result = QMessageBox.critical(self, "Error", msg)

    def readButton(self):
        isDevice = True;
        self.isLocked = True;

        self.statBar.showMessage("Reading...")

        #Read from device
        try:
            self.m = memtype()
            self.block = self.m.read()
            self.isLocked = self.m.isLocked()
            self.m.disconnect()
        except Exception:
            isDevice=False
            self.showErrorMessage("MemType device not found!")
            self.statBar.showMessage("Error Reading!!")

        if self.isLocked and isDevice:
            isDevice = False
            self.showErrorMessage("Device Locked, unlock it before using!")
            self.statBar.showMessage("Error, unlock device!")

        if isDevice:
            self.text, ok = QInputDialog.getText(self, 'Enter PIN','Enter PIN:',mode=QLineEdit.Password)
            if not self.m.validatePin(str(self.text)):
                ok = False
                self.showErrorMessage("Error, wrong PIN!")
                self.statBar.showMessage("Error, wrong PIN!")

        if isDevice and ok:
            self.cl = decryptCredentialList(self.block, key=pinToKey(str(self.text)))
            #clean list of credentials
            self.centCredList.clearCredentials()
            #and add all the credentials to the list
            for i,cr in enumerate(self.cl):
                self.centCredList.addCredential(cr.name,i)
                self.statBar.showMessage(GUI_VERSION)
                self.console.setFocus()

    def writeButton(self):
        isDevice = True;
        self.isLocked = True;
        self.statBar.showMessage("Writting...")

        #Check device
        try:
            self.m = memtype()
            self.isLocked = self.m.isLocked()
        except Exception:
            isDevice=False
            self.showErrorMessage("MemType device not found!")

        if self.isLocked and isDevice:
            isDevice = False
            self.showErrorMessage("Device Locked, unlock it before using!")

        if isDevice:
            self.text, ok = QInputDialog.getText(self, 'Enter PIN','Enter PIN:',mode=QLineEdit.Password)
            if not self.m.validatePin(str(self.text)):
                ok = False
                self.showErrorMessage("Error, wrong PIN!")
                self.statBar.showMessage("Error, wrong PIN!")

        if isDevice and ok:
            #Write to device
            try:
                block = encryptCredentialList(self.cl, key=pinToKey(str(self.text)))
                self.m.write(block)
                self.m.disconnect()
            except Exception:
                self.showErrorMessage("Error writting to device!")
                self.statBar.showMessage("Error writting!!")

            self.statBar.showMessage(GUI_VERSION)
            self.console.setFocus()


    def setPinButton(self):
        isDevice = True;
        self.isLocked = True;

        self.statBar.showMessage("Writting PIN...")

        #Read from device
        try:
            self.m = memtype()
            self.block = self.m.read()
            self.isLocked = self.m.isLocked()
            #self.m.disconnect()
        except Exception:
            isDevice=False
            self.showErrorMessage("MemType device not found!")

        if self.isLocked and isDevice:
            isDevice = False
            self.showErrorMessage("Device Locked, unlock it before using!")

        if isDevice:
            self.text, ok = QInputDialog.getText(self, 'Enter Old PIN','Enter Old PIN:',mode=QLineEdit.Password)
            if not self.m.validatePin(str(self.text)):
                ok = False
                self.showErrorMessage("Error, wrong PIN!")
                self.statBar.showMessage("Error, wrong PIN!")

            if ok:
                self.ntext, nok = QInputDialog.getText(self, 'Set New PIN','Set New PIN:',mode=QLineEdit.Password)
                if nok:
                    self.vntext, vnok = QInputDialog.getText(self, 'Repeat New PIN','Repeat New PIN:',mode=QLineEdit.Password)

        if isDevice and ok and str(self.ntext)==str(self.vntext):
            self.cl = decryptCredentialList(self.block, key=pinToKey(str(self.text)))
            #clean list of credentials
            #~self.centCredList.clearCredentials()
            #and add all the credentials to the list
            #~for i,cr in enumerate(self.cl):
            #~    self.centCredList.addCredential(cr.name,i)

           #Reencrypt with new PIN and write to device
            try:
                block = encryptCredentialList(self.cl, key=pinToKey(str(self.ntext)))
                self.m.write(block)
                self.m.disconnect()
            except Exception:
                self.showErrorMessage("Error writting encrypted credentials with new PIN!!")

            #Change the PIN on the device (the last thing to do!!)
            try:
               self.m.writePinHash(pinToHash(str(self.ntext)))
            except Exception:
                self.showErrorMessage("Error setting the new PIN!!")



            self.statBar.showMessage(GUI_VERSION)
            self.console.setFocus()


    def setKeyboardButton(self):
        isDevice = True;
        self.isLocked = True;

        #Check device
        try:
            self.m = memtype()
            self.isLocked = self.m.isLocked()
        except Exception:
            isDevice=False
            self.showErrorMessage("MemType device not found!")

        if self.isLocked and isDevice:
            isDevice = False
            self.showErrorMessage("Device Locked, unlock it before using!")

        if isDevice:
             inFile= QFileDialog.getOpenFileName(self, 'Choose KeyLayout')
        if isDevice and str(inFile) != "":
            #Write to device
            try:
                self.m.writeKeyboardLayout(str(inFile))
                self.m.disconnect()
            except Exception:
                self.showErrorMessage("Error writting to device!")

    def exportFileButton(self):
        outFileName = QFileDialog.getSaveFileName(self, 'Export File')

        if str(outFileName) != "":
            #Convert to a list of dicts
            tl = [] #Transformed list
            for c in self.cl:
                cd = {}
                cd['name'] = c.name
                cd['user'] = c.user
                cd['hop'] = c.hop
                cd['passw'] = c.passw
                cd['submit'] = c.submit
                tl.append(cd)
            filepass, ok = QInputDialog.getText(self, 'File encryption pssword','Enter the file encryption password:',mode=QLineEdit.Password)
            filepass2, ok2 = QInputDialog.getText(self, 'File encryption pssword','Repeat the file encryption password:',mode=QLineEdit.Password)

            if ok and ok2 and filepass !="" and filepass2 != "" and filepass == filepass2 :
                iv = ''.join(chr(random.randint(0,0xFF)) for i in range(16))
                encryptor = AES.new(hashlib.md5(filepass).hexdigest(),AES.MODE_CBC,iv)

                with open(str(outFileName),'wb') as outfile:
                    text=json.dumps(tl)
                    if len(text) % 16 != 0:
                        text += ' ' * (16 - len(text) % 16)
                    outfile.write(iv)
                    outfile.write(encryptor.encrypt(text))

    def importFileButton(self):
        inFileName = QFileDialog.getOpenFileName(self, 'Import File','./')
        filepass, ok = QInputDialog.getText(self, 'File encryption pssword','Enter the file encryption password:',mode=QLineEdit.Password)

        if str(inFileName) != "" and filepass != "" and ok:

            with open(str(inFileName),'rb') as infile:
                iv = infile.read(16)
                decryptor = AES.new(hashlib.md5(filepass).hexdigest(), AES.MODE_CBC,iv)
                text = decryptor.decrypt(infile.read())
                tl = json.loads(text)
                #Clean credential list
                self.cl = []
                for cred in tl:
                    newCred = credential()
                    newCred.name = str(cred['name'])
                    newCred.user = str(cred['user'])
                    newCred.hop = str(cred['hop'])
                    newCred.passw = str(cred['passw'])
                    newCred.submit = str(cred['submit'])
                    self.cl.append(newCred)

            #clean list of credentials
            self.centCredList.clearCredentials()
            #and add all the credentials to the list
            for i,cr in enumerate(self.cl):
                self.centCredList.addCredential(cr.name,i)


    def menuClicked(self,button):
        #Check what menu button was pressed!
        if(button == "Set Pin"):
            #text, ok = QInputDialog.getText(self, 'Set new PIN','Enter new PIN:')
            self.setPinButton()
        elif(button == "Read"):
            self.readButton()
        elif(button == "Write"):
            self.writeButton()
        elif(button == "Import File"):
            self.importFileButton()
        elif(button == "Export File"):
            self.exportFileButton()
        elif(button == "Set KeyLayout"):
            self.setKeyboardButton()

    def upClicked(self,position):
        print "UP %d"%(position)

    def downClicked(self,position):
        print "DOWN %d"%(position)

    def openClicked(self,position):
        #Open the edit form
        self.setEditMode()
        self.centCredEdit.loadCredential(self.cl[position],position)



    def deleteClicked(self,position):
        #Delete element from the credential list
        del self.cl[position]
        #Update the window
        self.centCredList.clearCredentials()
        for i,cr in enumerate(self.cl):
            self.centCredList.addCredential(cr.name,i)



def main():
    #Create new application
    mttool = QApplication(sys.argv)
    #Create new instance of main window

    #Locale
    translator = QTranslator()
    translator.load(QLocale.system().name() + '.qm', 'translate')
    mttool.installTranslator(translator)

    mtWindow = Window()
    mtWindow.show() #Make instance visible
    mtWindow.raise_() #Raise instance to top of window stack
    mttool.exec_()

if __name__ == "__main__":
    main()
