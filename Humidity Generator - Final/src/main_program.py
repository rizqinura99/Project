#!/usr/bin/python3
from os import stat
from struct import pack, unpack
from serial import Serial
from time import sleep
import sys, csv
from PyQt5 import QtGui
from PyQt5.uic import loadUi
from PyQt5 import QtWidgets
from PyQt5.QtCore import QThread, QTimer

from PyQt5.QtWidgets import QDialog, QApplication
from PyQt5.QtGui import QPixmap
import numpy as np
from datetime import datetime
import serial
from filename import STATE, STATUS_ALAT_NORMAL_OFF, STATUS_ALAT_NORMAL_ON, STATUS_ALAT_PANAS_OFF, STATUS_ALAT_PANAS_ON, STATUS_ARUS_OFF, STATUS_ARUS_ON, STATUS_FAN_OFF, STATUS_FAN_ON, STATUS_FILTER_BERSIH, STATUS_FILTER_KOTOR, STATUS_FILTER_SEDANG, STATUS_LOCK_OFF, STATUS_LOCK_ON, STATUS_MODE_OFF, STATUS_MODE_ON, STATUS_RH, STATUS_STATOP_GREEN, STATUS_STATOP_RED, STATUS_UV_OFF, STATUS_UV_ON, STATUS_WATER_LEVEL0, STATUS_WATER_LEVEL1, STATUS_WATER_LEVEL2, STATUS_WATER_LEVEL3, STATUS_WATER_LEVEL4, STATUS_WATER_LEVEL5, UI1, UI2
#from excel import onButtonSave

arduinoSerial = serial.Serial('COM4', 57600)  # open serial port

data_humidity = [] #list
data_temp = []
data_tank1 = []
data_tank2 = []
data_rpm = []
waktu = []
saveHum = [66.45,0]
saveTemp = [29.78 ,0]

class SerialThread(QThread):
    def run(self):
        global warna,power, tank1Value, tank2Value, humidity_temp, temperature_temp,\
        CMPC, FAN, RO, WP, WH, RPM,\
        data_humidity, data_temp, data_tank1, data_tank2, data_rpm

        while True:
            dataRaw = arduinoSerial.read()
            if (dataRaw == b'@'):
                dataRaw = arduinoSerial.read(25)
                # print(dataRaw)
            
                '''
                self.warna,\            short 2 Bytes => h
                self.power,\            float 4 Bytes => f
                self.tank1Value,\       short 2 Bytes => h
                self.tank2Vvalue,\      short 2 Bytes => h
                self.humidityTemp,\     float 4 Bytes => f
                self.temperatureCTemp,\ float 4 Bytes => f
                self.CMPC,\             bool  1 Byte  => ?
                self.FAN,\              bool  1 Byte  => ?
                self.RO,\               bool  1 Byte  => ?
                self.WP,\               bool  1 Byte  => ?
                self.WH,\               bool  1 Byte  => ?
                sel.RPM,\               short 2 Bytes => h
                '''   
                warna = int.from_bytes(dataRaw[0:2],'little')
                power = unpack('f', dataRaw[2:6])[0]
                tank1Value = int.from_bytes(dataRaw[6:8],'little')
                tank2Value = int.from_bytes(dataRaw[8:10], 'little')
                humidity_temp = unpack('f', dataRaw[10:14])[0]
                if humidity_temp >= 0:
                    saveHum[1] = humidity_temp
                    saveHum[0] = humidity_temp
                else:
                    saveHum[1] = saveHum[0]
                temperature_temp = unpack('f', dataRaw[14:18])[0]
                if temperature_temp >= 0:
                    saveTemp[1] = temperature_temp
                    saveTemp[0] = temperature_temp
                else:
                    saveTemp[1] = saveTemp[0]
                CMPC = bool.from_bytes(dataRaw[18:19], 'little')
                FAN = bool.from_bytes(dataRaw[19:20], 'little')
                RO = bool.from_bytes(dataRaw[20:21], 'little')
                WP = bool.from_bytes(dataRaw[21:22], 'little')
                WH = bool.from_bytes(dataRaw[22:23], 'little')
                RPM = int.from_bytes(dataRaw[23:25], 'little')
                DATE = datetime.now().strftime("%Y-%B-%d,%H:%M:%S")


                #mengumpulkan data
                data_humidity.append(saveHum[1])
                data_temp.append(saveTemp[1])
                data_tank1.append(tank1Value)
                data_tank2.append(tank2Value)
                data_rpm.append(RPM)
                waktu.append(DATE)

                # print(f"warna = {warna} power = {power}, tank1 = {tank1Value}, tank2 = {tank2Value}, hum = {humidity_temp}, temp=  {temperature_temp} CMPC = {CMPC} FAN= {FAN} RO = {RO} WP= {WP} WH={WH} RPM={RPM}")
 
class screen1(QDialog):
    def __init__(self):
        super(screen1, self).__init__()
        loadUi(UI1,self)
        self.pushButton.clicked.connect(self.next)
    def next(self):
        widget.setCurrentIndex(widget.currentIndex()+1)
        
class screen2(QDialog):
    def __init__(self):
        super(screen2, self).__init__()
        loadUi(UI2,self)
    
        self.startSistem_btn.clicked.connect(self.start_sistem)
        self.start_btn.clicked.connect(self.start_handler)
        #self.start_btn.clicked.connect(self.stop_handler)
        self.normal_btn.clicked.connect(self.normal_water)
        self.hot_btn.clicked.connect(self.hot_water)
        self.lock_btn.clicked.connect(self.lock)
        self.save_btn.clicked.connect(self.saveData)
        
        self.serth = SerialThread()
        self.serth.start()
        #init csv file
        self.option = QtWidgets.QFileDialog.Options
        self.fileName, _ = QtWidgets.QFileDialog.getSaveFileName(self, "Save File", "", "CSV File (*.csv)")

        #Qtimer
        self.qTimer = QTimer()
        self.qTimer.setInterval(100) # 1000 ms = 1 s
        # connect timeout signal to signal handler
        self.qTimer.timeout.connect(self.layar)
        # start timer
        self.qTimer.start()
        #self.start_btn.clicked.connect(self.filter_warna)
        #self.start_btn.clicked.connect(self.current_sensor)
        
        #dummy var
        self.start_sistem = False
        self.start_stop = False

        self.lock = False

        self.HOT = False
        self.NORMAL = False
        self.START = False

    def start_sistem(self):
        #UV1
        if(self.start_stop):
            pass
        else:
            self.start_sistem = not self.start_sistem
            

        print(self.start_sistem)

    def start_handler(self):
        if(self.start_sistem):
            self.start_stop = not self.start_stop
        else:
            pass

        if(self.start_stop):
            #send data
            self.START = True
            self.sendData()
        else:
            #send data
            self.START = False
            self.sendData()
            
    def saveData(self):
        if self.fileName:
            with open(self.fileName, mode="w", newline='') as csvFile:
                #csvWriter = csv.writer(csvFile, dialect='excel')
                csvWriter = csv.writer(csvFile, dialect='excel')
                csvWriter.writerow(["Waktu", "Humidity", "Temperature", "Tank1", "Tank2", "RPM"])
                
                for i in range(len(data_humidity)-1):
                    csvWriter.writerow([waktu[i], data_humidity[i], data_temp[i], data_tank1[i], data_tank2[i], data_rpm[i]])
            #    for i in range(len(data_temp)-1):
            #        csvWriter.writerow([data_temp[i]]) #tambahi variable lain
            #    for i in range(len(data_temp)-1):
            #        csvWriter.writerow([waktu[i], "Suhu", data_temp[i]])
            #    for i in range(len(data_tank1)-1):
            #        csvWriter.writerow([waktu[i], "Tinggi Tank 1", data_tank1[i]])
            #    for i in range(len(data_tank2)-1):
            #        csvWriter.writerow([waktu[i], "Tinggi Tank 2", data_tank2[i]])
            #    for i in range(len(data_rpm)-1):
             #       csvWriter.writerow([waktu[i], "RPM", data_rpm[i]])
                    #csvWriter.writerow(["Temperature", temperature_temp])
                    #csvWriter.writerow(["Tinggi Tank 1", tank1Value])
                    #csvWriter.writerow(["Tinggi Tank 2", tank2Value])
                
    def layar(self):
        #fun untuk menampilkan semua data dan kondisi di layar
        if self.start_stop & self.start_sistem:
            self.info_kondisi.setText(format('Sistem menyala'))
            try:
                self.tank_level(tank1Value, tank2Value)
                self.tampilRPM(RPM)
                self.tampilPower(power)
                self.tampilFilter(warna)
                self.tampil_suhu_humidity(saveHum[1], saveTemp[1])
                self.tampil_fan_UV(FAN)
                self.tampil_kompre(CMPC)
                self.tampil_pompa(RO)
                self.receive_pump(WP, WH)
                self.start_btn.setIcon(QtGui.QIcon(STATUS_STATOP_GREEN))
                self.tampilMode()
                #save data
                #self.saveData()
            except:
                self.start_btn.setIcon(QtGui.QIcon(STATUS_STATOP_GREEN))
        
        else:
            self.start_btn.setIcon(QtGui.QIcon(STATUS_STATOP_RED))
            self.Fan_kondisi.setText(format('Non Aktif'))
            self.Kompre_kondisi.setText(format('Non Aktif'))
            self.fan_label.setScaledContents(True)
            self.fan_label.setPixmap(QPixmap(STATUS_FAN_OFF))
            self.arus_label.setPixmap(QPixmap(STATUS_ARUS_OFF))
            self.filter_label.setPixmap(QPixmap(STATUS_FILTER_BERSIH))
            self.leveltank1_label.setPixmap(QPixmap(STATUS_WATER_LEVEL0))
            self.leveltank2_label.setPixmap(QPixmap(STATUS_WATER_LEVEL0))
            self.normal_btn.setIcon(QtGui.QIcon(STATUS_ALAT_NORMAL_OFF))
            self.hot_btn.setIcon(QtGui.QIcon(STATUS_ALAT_PANAS_OFF))
            self.mode_label.setPixmap(QPixmap(STATUS_MODE_OFF))
            self.RPM_result.setText("0")
            self.Power_result.setText('0')
            self.fan_label.setScaledContents(True)
            self.fan_label.setPixmap(QPixmap(STATUS_FAN_OFF))
            self.Fan_kondisi.setText(format('Non Aktif'))
            self.uv1_label.setScaledContents(True)
            self.uv1_label.setPixmap(QPixmap(STATUS_UV_OFF))
            self.uv2_label.setScaledContents(True)
            self.uv2_label.setPixmap(QPixmap(STATUS_UV_OFF))
            self.RH_result.setText('0')
            self.Temperature_result.setText('0')
            self.info_kondisi.setText(format('Sistem Mati'))

    def tank_level(self,TankBawah=12, TankAtas=20):
        #nilai Tank1_value disesuaikan dengan serial Tank1_value dari arduino
        print(TankBawah, TankAtas)

        if TankBawah > 11: 
            self.leveltank2_label.setPixmap(QPixmap(STATUS_WATER_LEVEL1))
        elif TankBawah >= 10 and TankBawah <= 11:
            self.leveltank2_label.setPixmap(QPixmap(STATUS_WATER_LEVEL2))
        elif TankBawah >= 8 and TankBawah <= 9:
            self.leveltank2_label.setPixmap(QPixmap(STATUS_WATER_LEVEL3))
        elif TankBawah > 5 and TankBawah < 8:
            self.leveltank2_label.setPixmap(QPixmap(STATUS_WATER_LEVEL4))
        elif TankBawah <= 5:
            self.leveltank2_label.setPixmap(QPixmap(STATUS_WATER_LEVEL5))
    
        if TankAtas >= 20: 
            self.leveltank1_label.setPixmap(QPixmap(STATUS_WATER_LEVEL1))
        elif TankAtas >= 17 and TankAtas < 20:
            self.leveltank1_label.setPixmap(QPixmap(STATUS_WATER_LEVEL2))
        elif TankAtas >= 13 and TankAtas <= 16:
            self.leveltank1_label.setPixmap(QPixmap(STATUS_WATER_LEVEL3))
        elif TankAtas > 10 and TankAtas < 13:
            self.leveltank1_label.setPixmap(QPixmap(STATUS_WATER_LEVEL4))
        elif TankAtas <= 10:
            self.leveltank1_label.setPixmap(QPixmap(STATUS_WATER_LEVEL5))
    def tampilRPM(self, rpm=0):
        self.fan_label.setPixmap(QPixmap(STATUS_FAN_ON))
        #self.info_kondisi.setText(format('Kipas menyala'))
        self.RPM_result.setText(str(rpm))

    def tampilPower(self,power=0):
        self.arus_label.setPixmap(QPixmap(STATUS_ARUS_ON))
        self.Power_result.setText('{:.2f} W'.format(power)) #dibuat data serial power dari arduino
    
    def tampilFilter(self, warna=0):
        #outputan di interface akan muncul sesuai dengan data serial warna dari arduino
        if warna == 0:
            self.filter_label.setPixmap(QPixmap(STATUS_FILTER_KOTOR))
        elif warna == 1:
            self.filter_label.setPixmap(QPixmap(STATUS_FILTER_SEDANG))
        elif warna == 2 or 3:
            self.filter_label.setPixmap(QPixmap(STATUS_FILTER_BERSIH))

    def tampilMode(self):
        self.mode_label.setPixmap(QPixmap(STATUS_MODE_ON))
    
    def tampil_suhu_humidity(self,humidity = 0, suhu = 0):
        self.RH_result.setText('{:.2f} %'.format(humidity))
        self.Temperature_result.setText('{:.2f}°C'.format(suhu))
    
    def tampil_fan_UV(self, FAN = False):
        if FAN:
            self.fan_label.setScaledContents(True)
            self.fan_label.setPixmap(QPixmap(STATUS_FAN_ON))
            self.Fan_kondisi.setText(format('Aktif'))
            self.uv1_label.setScaledContents(True)
            self.uv1_label.setPixmap(QPixmap(STATUS_UV_ON))
            self.uv2_label.setScaledContents(True)
            self.uv2_label.setPixmap(QPixmap(STATUS_UV_ON))
            self.info_kondisi.setText(format('Proses berjalan'))
            #self.info_kondisi.setText(format('Kipas dan UV menyala'))
        else:
            self.fan_label.setScaledContents(True)
            # self.fan_label.setPixmap(QPixmap(STATUS_FAN_OFF))
            self.Fan_kondisi.setText(format('Non Aktif'))
            self.uv1_label.setScaledContents(True)
            self.uv1_label.setPixmap(QPixmap(STATUS_UV_OFF))
            self.uv2_label.setScaledContents(True)
            self.uv2_label.setPixmap(QPixmap(STATUS_UV_OFF))
            #self.info_kondisi.setText(format('Kipas dan UV mati'))
    
    def tampil_kompre(self, CMPC = False):
        if CMPC:
            self.Kompre_kondisi.setText(format('Aktif'))
            #self.info_kondisi.setText(format('Delay 10 menit'))
        else:
            self.Kompre_kondisi.setText(format('Non Aktif'))
            #self.info_kondisi.setText(format('Sistem berjalan'))

    def tampil_pompa(self, RO = False):
        if RO:  
            self.pompa_kondisi.setText(format('Aktif'))
            #self.info_kondisi.setText(format('Pompa menyala'))
        else:
            self.pompa_kondisi.setText(format('Non Aktif'))
            #self.info_kondisi.setText(format('Pompa mati'))

    def normal_water(self, checked):
        if checked:
            self.NORMAL = True
            self.sendData()
            self.normal_btn.setIcon(QtGui.QIcon(STATUS_ALAT_NORMAL_ON))
            #self.info_kondisi.setText(format('Proses mengeluarkan air nyala'))
            #ketika menyala, mengirim data variabel "statusNormal" = 1
        else:
            self.NORMAL = False
            self.sendData()
            self.normal_btn.setIcon(QtGui.QIcon(STATUS_ALAT_NORMAL_OFF))
            #self.info_kondisi.setText(format('Proses mengeluarkan air berhenti'))
            #ketika mati, mengirim data variabel "statusNormal" = 0

    def receive_pump(self, WP = True, WH = True):
        if WP:
            self.normal_btn.setIcon(QtGui.QIcon(STATUS_ALAT_NORMAL_OFF))

        if WH:
            self.hot_btn.setIcon(QtGui.QIcon(STATUS_ALAT_PANAS_OFF))

    def lock(self, checked):
        if checked:
            self.lock_btn.setIcon(QtGui.QIcon(STATUS_LOCK_ON))
            #self.info_kondisi.setText(format('Pompa air telah dibuka (unlock)'))
        else:
            self.lock_btn.setIcon(QtGui.QIcon(STATUS_LOCK_OFF))
            self.hot_btn.setIcon(QtGui.QIcon(STATUS_ALAT_PANAS_OFF))
            #self.info_kondisi.setText(format('Pompa air telah ditutup (lock)'))
            self.HOT = False
            self.sendData()

    def hot_water(self, checked):
        if self.lock_btn.isChecked():
            if checked:
                self.HOT = True
                self.sendData()
                self.hot_btn.setIcon(QtGui.QIcon(STATUS_ALAT_PANAS_ON))
                self.info_kondisi.setText(format('Proses memanaskan air berjalan'))
                #ketika menyala, mengirim data variabel "statusPanas" = 1
            else:
                self.HOT = False
                self.sendData()
                #sendData(HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH)
                self.hot_btn.setIcon(QtGui.QIcon(STATUS_ALAT_PANAS_OFF))
                self.info_kondisi.setText(format('Proses memanaskan air berhenti'))
                #ketika mati, mengirim data variabel "statusPanas" = 0
    
    def sendData(self):
        dataPacket = pack("???", self.NORMAL, self.HOT, self.START)
        arduinoSerial.write(dataPacket)

#main
while True:
    
    app = QApplication(sys.argv)
    widget = QtWidgets.QStackedWidget()

    widget.addWidget(screen1())
    widget.addWidget(screen2())

    widget.setFixedWidth(1024)
    widget.setFixedHeight(768)
    widget.showFullScreen()

    try:
        sys.exit(app.exec_())
    except:
        print("exiting")
