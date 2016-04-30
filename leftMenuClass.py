from PyQt4.QtGui import *
from PyQt4.QtCore import *

class leftMenu(QWidget):
    """This class creates the main menu"""

    menuClicked = pyqtSignal(str)

    #Constructor
    def __init__(self,buttonList):
        super(QWidget,self).__init__() #Call super class constructor

        #Create buttonList Layout
        self.buttonLayout = QHBoxLayout()
        #Add buttons to the layout
        for btn in buttonList:
            self.button = QToolButton()
            self.button.setText(btn[0])
            self.button.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
            self.button.setIcon(QIcon(btn[1]))
            myCallable = getattr(self,btn[0].replace(' ','_')+"Clicked")
            self.button.clicked.connect(myCallable)
            self.buttonLayout.addWidget(self.button)

        self.buttonLayout.addStretch(1)
        self.setLayout(self.buttonLayout)

    def ReadClicked(self):
        self.menuClicked.emit("Read")

    def WriteClicked(self):
        self.menuClicked.emit("Write")
    def Set_PINClicked(self):
        self.menuClicked.emit("Set Pin")
    def Export_FileClicked(self):
        self.menuClicked.emit("Export File")
    def Import_FileClicked(self):
        self.menuClicked.emit("Import File")

    def Set_KeyLayoutClicked(self):
        self.menuClicked.emit("Set KeyLayout")


    def buttonLeftClick(self):
        print 'Clicked!'


