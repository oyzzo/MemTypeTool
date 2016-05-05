import sys
import json
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from leftMenuClass import *
from credListClass import *
from credEditClass import *

from libsmttool.memtype import *

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
        #Credential List on the center
        self.setListMode()

        #Create Main Layout
        self.mainLayout = QVBoxLayout()
        self.mainLayout.addWidget(self.topWidget)
        self.mainLayout.addLayout(self.centLayout)


        self.mainWidget = QWidget()
        self.mainWidget.setLayout(self.mainLayout)
        self.setCentralWidget(self.mainWidget)

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



    def editOk(self):
        """This method is called when confirming edition of a credential"""
        print "OK PRESSED!"

    def editCancel(self):
        """This method is called when canceled edition of a credential"""
        #Nothing to do, return to list mode
        self.setListMode()

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

        #Read from device
        try:
            self.m = memtype()
            self.block = self.m.read()
            self.isLocked = self.m.isLocked()
            self.m.disconnect()
        except Exception:
            isDevice=False
            self.showErrorMessage("MemType device not found!")

        if self.isLocked and isDevice:
            isDevice = False
            self.showErrorMessage("Device Locked, unlock it before using!")

        if isDevice:
            self.text, ok = QInputDialog.getText(self, 'Enter PIN','Enter PIN:',mode=QLineEdit.Password)
        if isDevice and ok:
            self.cl = decryptCredentialList(self.block, key=pinToKey(str(self.text)))
            #clean list of credentials
            self.centCredList.clearCredentials()
            #and add all the credentials to the list
            for i,cr in enumerate(self.cl):
                self.centCredList.addCredential(cr.name,i)

    def writeButton(self):
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
            self.text, ok = QInputDialog.getText(self, 'Enter PIN','Enter PIN:',mode=QLineEdit.Password)
        if isDevice and ok:
            #Write to device
            try:
                block = encryptCredentialList(self.cl, key=pinToKey(str(self.text)))
                self.m.write(block)
                self.m.disconnect()
            except Exception:
                self.showErrorMessage("Error writting to device!")

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
            with open(str(outFileName),'wb') as outfile:
                json.dump(self.cl,outfile)

    def importFileButton(self):
        inFileName = QFileDialog.getOpenFileName(self, 'Import File','./')
        if str(inFileName) != "":
            with open(str(inFileName),'rb') as infile:
                tl = json.load(infile)
                #Clean credential list
                self.cl = []
                for c in tl:
                    cred = json.loads(c)
                    newCred = credential()
                    newCred.name = cred['name']
                    newCred.user = cred['user']
                    newCred.hop = cred['hop']
                    newCred.passw = cred['passw']
                    newCred.submit = cred['submit']
                    self.cl.append(newCred)

            #clean list of credentials
            self.centCredList.clearCredentials()
            #and add all the credentials to the list
            for i,cr in enumerate(self.cl):
                self.centCredList.addCredential(cr.name,i)


    def menuClicked(self,button):
        #Check what menu button was pressed!
        if(button == "Set Pin"):
            text, ok = QInputDialog.getText(self, 'Set new PIN','Enter new PIN:')
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
        self.centCredEdit.loadCredential(self.cl[position])



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
    mtWindow = Window()
    mtWindow.show() #Make instance visible
    mtWindow.raise_() #Raise instance to top of window stack
    mttool.exec_()

if __name__ == "__main__":
    main()
