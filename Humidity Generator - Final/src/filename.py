#!/usr/bin/python3

#UI
from typing import Pattern

#golbal variable
STATE = False

#serial
portname = 'COM8'
baudrate = 9600

PATH_DIR = 'D:\[]REDESMA\Humidity Generator - Final'
UI1 = PATH_DIR+'//UI//screen1.ui'
UI2 = PATH_DIR+'//UI//screen2.ui'

#STATUS ALAT
STATUS_ALAT_NORMAL_OFF = PATH_DIR+'//assets//Gb-AlatNormal-off.png'
STATUS_ALAT_NORMAL_ON = PATH_DIR+'//assets//Gb-AlatNormal-on.png'
STATUS_ALAT_PANAS_OFF = PATH_DIR+'//assets//Gb-AlatPanas-off.png'
STATUS_ALAT_PANAS_ON = PATH_DIR+'//assets//Gb-AlatPanas-on.png'
STATUS_ARUS_OFF = PATH_DIR+'//assets//Gb-arus-off.png'
STATUS_ARUS_ON = PATH_DIR+'//assets//Gb-arus-on.png'
STATUS_FAN_OFF = PATH_DIR+'//assets//Gb-Fan-off.png'
STATUS_FAN_ON = PATH_DIR+'//assets//Gb-Fan-on.png'
STATUS_FILTER_BERSIH = PATH_DIR+'//assets//Gb-Fltr-brsh.png'
STATUS_FILTER_SEDANG = PATH_DIR+'//assets//Gb-Fltr-sdg.png'
STATUS_FILTER_KOTOR = PATH_DIR+'//assets//Gb-Fltr-ktr.png'
STATUS_LOCK_OFF = PATH_DIR+'//assets//Gb-Lock-off.png'
STATUS_LOCK_ON = PATH_DIR+'//assets//Gb-Lock-on.png'
STATUS_MODE_OFF = PATH_DIR+'//assets//Gb-Mode-off.png'
STATUS_MODE_ON = PATH_DIR+'//assets//Gb-Mode-on.png'
STATUS_STATOP_GREEN = PATH_DIR+'//assets//Gb-Statop-grn.png'
STATUS_STATOP_OFF = PATH_DIR+'//assets//Gb-Statop-off.png'
STATUS_STATOP_RED = PATH_DIR+'//assets//Gb-Statop-red.png'
STATUS_UV_OFF = PATH_DIR+'//assets//Gb-UV-off.png'
STATUS_UV_ON = PATH_DIR+'//assets//Gb-Uv-on.png'
STATUS_WATER_LEVEL0 = PATH_DIR+'//assets//Gb-WtrLvl0.png'
STATUS_WATER_LEVEL1 = PATH_DIR+'//assets//Gb-WtrLvl1.png'
STATUS_WATER_LEVEL2 = PATH_DIR+'//assets//Gb-WtrLvl2.png'
STATUS_WATER_LEVEL3 = PATH_DIR+'//assets//Gb-WtrLvl3.png'
STATUS_WATER_LEVEL4 = PATH_DIR+'//assets//Gb-WtrLvl4.png'
STATUS_WATER_LEVEL5 = PATH_DIR+'//assets//Gb-WtrLvl5.png'
STATUS_RH = PATH_DIR+'//assets//Gb-RH.png'
STATUS_SISTEM = PATH_DIR+'//assets//Gb-Sistem.png'
STATUS_SUHU = PATH_DIR+'//assets//Gb-Suhu.png'
