#проект будет состоять из набора модулей-подпроектов
TEMPLATE = subdirs

SUBDIRS += \
    src/MyLib/RTL_SDR_RadarLib \
    src/MyApp/RadarApp

CONFIG += ordered
