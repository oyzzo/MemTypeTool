from PyQt4.QtGui import *
from PyQt4.QtCore import *

class credItem(QWidget):
    """This class creates a credential item widget"""
    #Constructor
    def __init__(self,crd):
        super().__init__() #Call super class constructor

        #Create the elements of the credential
        self.crdLabel = QLabel(crd)
        self.dltIcon = QLabel()
        self.dltIcon.setPixmap(QPixmap("img/trashcan.png"))
        self.dltIcon.setAlignment(Qt.AlignRight)
        self.upIcon = QLabel()
        self.downIcon = QLabel()
        self.upIcon.setPixmap((QPixmap("img/up.png")))
        self.downIcon.setPixmap((QPixmap("img/down.png")))

        #Create Arrow Layout
        self.posLayout = QVBoxLayout()
        self.posLayout.addWidget(self.upIcon)
        self.posLayout.addWidget(self.downIcon)

        #Create the main Layout
        self.crdLayout = QHBoxLayout()
        self.crdLayout.addLayout(self.posLayout)
        self.crdLayout.addWidget(self.crdLabel)
        self.crdLayout.addWidget(self.dltIcon)
        #Create the main widget
        self.mainWidget = QWidget()
        self.mainWidget.setStyleSheet("background-color:white;")
        self.mainWidget.setLayout(self.crdLayout)

        #Create the Self layout
        self.selfLayout = QVBoxLayout()
        self.selfLayout.addWidget(self.mainWidget)
        self.setLayout(self.selfLayout)


class credList(QWidget):
    """This class creates the main credentials list"""

    #Constructor
    def __init__(self,crdList):
        super().__init__() #Call super class constructor

        #Create credList container widget
        self.credWidget = QWidget()
        #Create credList Layout
        self.credLayout = QVBoxLayout()
        #Add credentials to the layout
        for crd in crdList:
            self.addCredential(crd)

        self.credWidget.setLayout(self.credLayout)


        #Scroll Area Properties
        self.scroll = QScrollArea()
        self.scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.scroll.setWidgetResizable(True)
        self.scroll.setAlignment(Qt.AlignCenter)
        self.scroll.setWidget(self.credWidget)


        #Add new credential button
        self.newCred = QToolButton()
        self.newCred.setText("New")
        self.newCred.clicked.connect(lambda: self.addCredential('New Credential'))
        self.newCred.setIcon(QIcon("img/newcred.png"))

        #Scroll Area Layer add 
        self.vLayout = QVBoxLayout()
        self.vLayout.addWidget(self.scroll)
        self.vLayout.addWidget(self.newCred)
        self.setLayout(self.vLayout)

    def addCredential(self,crd):
        """method to add a new credential to the list"""

        self.credential = credItem(crd)
        self.credLayout.addWidget(self.credential)
        self.credWidget.adjustSize()


