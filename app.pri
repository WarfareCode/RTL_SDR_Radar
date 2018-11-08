#Переменная DESTDIR указывает путь, в который будет помещен готовый исполняем
DESTDIR = $${BIN_PATH}/
#определяется путь поиска динамических библиотек по умолчанию
QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../../lib.$${OS_SUFFIX}/
QMAKE_LFLAGS_RPATH=
