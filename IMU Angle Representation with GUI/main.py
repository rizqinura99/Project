from PyQt5 import QtGui, QtCore, QtWidgets, uic
from PyQt5.QtCore import QThread, QTimer
from PyQt5.QtGui import QPixmap, QTransform
from PyQt5.QtWidgets import QMainWindow
from threading import Thread
from serial import Serial
import struct, sys
def trap_exc_during_debug(*args):
    print(args)

FileUI = 'D:\Kuliah\Komponen Sistem Kontrol\Program Tugas Akhir\MainWindow.ui'
RotatePic = 'D:\Kuliah\Komponen Sistem Kontrol\Program Tugas Akhir\Circle.png'
arduinoSerial = Serial('COM4', 9600)
rollDegree = 0

class MainUI(QMainWindow):
    def __init__(self):
        super(MainUI, self).__init__()
        uic.loadUi(FileUI, self)

        self.Timer = QTimer()
        self.Timer.timeout.connect(self.getData)
        self.Timer.start(100)

    def getData(self):
        self.degree_label.setText('{}°'.format(int(rollDegree)))
        self.pixmap = QPixmap(RotatePic)
        rotatedPic = self.pixmap.transformed(QTransform().rotate(-rollDegree),QtCore.Qt.SmoothTransformation)
        rect = QtCore.QRect(self.pixmap.rect())
        rect.moveCenter(rotatedPic.rect().center())
        rotatedPic = rotatedPic.copy(rect)
        self.rotate_label.setPixmap(QPixmap(rotatedPic))
        # self.degree_label.setText('210')

def dataThread():
    global rollDegree
    while True:
        if arduinoSerial.read() == b'$':
            data = bytearray(arduinoSerial.read(4))
            rollDegree = struct.unpack('f', data[0:4])[0]


app = QtWidgets.QApplication(sys.argv)
window = MainUI()

threadHandler = Thread(target=dataThread)
threadHandler.setDaemon(True)
threadHandler.start()

window.show()
sys.exit(app.exec_())