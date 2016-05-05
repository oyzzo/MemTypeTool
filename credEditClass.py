from PyQt4.QtGui import *
from PyQt4.QtCore import *

from libsmttool.memtype import *

class credEdit(QWidget):
    """This class creates a Credential edit form"""

    #Create custom signals
    okPressed = pyqtSignal()
    cancelPressed = pyqtSignal()

    #Position attribute
    position = 0

    #Constructor
    def __init__(self):
        super(QWidget,self).__init__() #Call super class constructor

        #Create credEdit Form Layout
        self.credLayout = QFormLayout()

        #Add the Credentials attributes
        self.crdNameLabel = QLabel("Name")
        self.crdNameEdit = QLineEdit()
        self.crdUserLabel = QLabel("User")
        self.crdUserEdit = QLineEdit()
        self.crdHopLabel = QLabel("Hop")
        self.crdHopEdit = QLineEdit()
        self.crdPassLabel = QLabel("Password")
        self.crdPassEdit = QLineEdit()
        self.crdSubmitLabel = QLabel("Submit")
        self.crdSubmitEdit = QLineEdit()

        #Add ok to save and cancel to discard edit
        self.okButton = QPushButton("Ok")
        self.cancelButton = QPushButton("Cancel")

        #Connect signals
        self.okButton.clicked.connect(self.okPressed)
        self.cancelButton.clicked.connect(self.cancelPressed)

        #Create the main Layout
        self.credLayout.addRow(self.crdNameLabel,self.crdNameEdit)
        self.credLayout.addRow(self.crdUserLabel,self.crdUserEdit)
        self.credLayout.addRow(self.crdHopLabel,self.crdHopEdit)
        self.credLayout.addRow(self.crdPassLabel,self.crdPassEdit)
        self.credLayout.addRow(self.crdSubmitLabel,self.crdSubmitEdit)
        self.credLayout.addRow(self.okButton,self.cancelButton)

        #Create the main Widget
        self.setLayout(self.credLayout)

    def getCredential(self):
        """Method to return the edited credential """
        cred = credential()
        cred.name = str(self.crdNameEdit.text())
        cred.user = str(self.crdUserEdit.text())
        cred.hop = str(self.crdHopEdit.text())
        cred.hop = str(cred.hop).replace("\\t","\t")
        cred.hop = str(cred.hop).replace("\\n","\n")
        cred.passw = str(self.crdPassEdit.text())
        cred.submit = str(self.crdSubmitEdit.text())
        cred.submit = str(cred.submit).replace("\\t","\t")
        cred.submit = str(cred.submit).replace("\\n","\n")

        return (cred,self.position)

    def loadCredential(self,cred,pos):
        """Method to show credential into the form"""


        self.position = pos

        self.crdNameEdit.setText(cred.name)
        self.crdUserEdit.setText(cred.user)
        self.crdHopEdit.setText(cred.hop)
        self.crdHopEdit.setText(str(self.crdHopEdit.text()).replace("\t","\\t"))
        self.crdHopEdit.setText(str(self.crdHopEdit.text()).replace("\n","\\n"))
        self.crdPassEdit.setText(cred.passw)
        self.crdSubmitEdit.setText(cred.submit)
        self.crdSubmitEdit.setText(str(self.crdSubmitEdit.text()).replace("\t","\\t"))
        self.crdSubmitEdit.setText(str(self.crdSubmitEdit.text()).replace("\n","\\n"))
