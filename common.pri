# фиксируется путь к корневому каталогу проекта, относительно которого будем определять все остальные пути:
PROJECT_ROOT_PATH = $${PWD}

# Далее определяем то, под какой ОС происходит сборка
# и устанавливаем соответствующим образом значение суффикса OS_SUFFIX.
win32: OS_SUFFIX = win32
linux-g++: OS_SUFFIX = linux

# в зависимости от режима сборки (debug или release)
# определяется значение BUILD_FLAG, которое будет указывать
# на версию используемого подкаталога в bin/ и build/:
# определяется вспомогательный суффикс LIB_SUFFIX.
# чтобы к именам библиотек в отладочном режиме присоединялся символ d.
# За счет этого мы можем иметь единый каталог для библиотек и не допускать конфликтов имен
CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
    LIB_SUFFIX = _d
} else {
    BUILD_FLAG = release
}

#определяются пути к библиотекам lib.*/
LIBS_PATH = $${PROJECT_ROOT_PATH}/lib.$${OS_SUFFIX}

#к открытым заголовочным файлам include/,
INC_PATH = $${PROJECT_ROOT_PATH}/include/

#к импортируемым заголовочным файлам import/
IMPORT_PATH = $${PROJECT_ROOT_PATH}/import/

#и путь к каталогу с бинарниками bin/:
BIN_PATH = $${PROJECT_ROOT_PATH}/bin/$${BUILD_FLAG}/$${TARGET}

BUILD_PATH = $${PROJECT_ROOT_PATH}/build/$${BUILD_FLAG}/$${TARGET}

#задает пути сборки для файлов ресурсов rcc,
RCC_DIR = $${BUILD_PATH}/rcc/

#файлов графического интерфейса ui,
UI_DIR = $${BUILD_PATH}/ui/

#МОК-файлов moc
MOC_DIR = $${BUILD_PATH}/moc/

#объектных файлов obj:
OBJECTS_DIR = $${BUILD_PATH}/obj/

# пути к библиотекам
LIBS += -L$${LIBS_PATH}/
INCLUDEPATH += $${INC_PATH}
INCLUDEPATH += $${IMPORT_PATH}
INCLUDEPATH += $${PROJECT_ROOT_PATH}/src/include/

QMAKE_CFLAGS -= -O
QMAKE_CFLAGS -= -O1
QMAKE_CFLAGS -= -O2

QMAKE_CFLAGS += -O3

QMAKE_CXXFLAGS += -std=c++11
CONFIG += c++11



