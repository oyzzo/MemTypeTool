from PyQt4.QtGui import *
from PyQt4.QtCore import *

class credEdit(QWidget):
    """This class creates a Credential edit form"""

    #Create custom signals
    okPressed = pyqtSignal()
    cancelPressed = pyqtSignal()


    #Constructor
    def __init__(self):
        super().__init__() #Call super class constructor

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
