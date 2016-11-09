#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T17:40:58
#
#-------------------------------------------------

QT       += core gui sql network script
CONFIG  += qaxcontainer

LIBS += -lwsock32
TARGET = officepark
TEMPLATE = app


SOURCES += main.cpp\
        officepark.cpp \
    dropshadowwidget.cpp \
    mypushbutton.cpp \
    personnel.cpp \
    staff.cpp \
    dimission.cpp \
    department.cpp \
    company.cpp \
    mysql.cpp \
    attendanceset.cpp \
    attendance.cpp \
    equipment.cpp \
    original.cpp \
    monthly.cpp \
    detail.cpp \
    day.cpp \
    basicwindows.cpp \
    udpserver.cpp \
    smartcontrol.cpp \
    log.cpp \
    loading.cpp \
    find.cpp \
    delete.cpp \
    user.cpp \
    system.cpp \
    information.cpp \
    leavestaff.cpp \
    askforleave.cpp \
    deal.cpp \
    pseudomonas.cpp \
    dragwindows.cpp \
    dragsmartcontorl.cpp \
    udpsend.cpp \
    about.cpp


HEADERS  += officepark.h \
    dropshadowwidget.h \
    mypushbutton.h \
    personnel.h \
    staff.h \
    dimission.h \
    department.h \
    company.h \
    mysql.h \
    struct.h \
    attendanceset.h \
    attendance.h \
    equipment.h \
    original.h \
    monthly.h \
    detail.h \
    day.h \
    basicwindows.h \
    udpserver.h \
    smartcontrol.h \
    log.h \
    loading.h \
    find.h \
    delete.h \
    Item.h \
    user.h \
    system.h \
    information.h \
    leavestaff.h \
    askforleave.h \
    deal.h \
    pseudomonas.h \
    dragwindows.h \
    dragsmartcontorl.h \
    udpsend.h \
    about.h

FORMS    += officepark.ui \
    personnel.ui \
    staff.ui \
    dimission.ui \
    department.ui \
    company.ui \
    attendanceset.ui \
    attendance.ui \
    equipment.ui \
    original.ui \
    monthly.ui \
    detail.ui \
    day.ui \
    basicwindows.ui \
    smartcontrol.ui \
    loading.ui \
    find.ui \
    delete.ui \
    user.ui \
    system.ui \
    information.ui \
    leavestaff.ui \
    askforleave.ui \
    deal.ui \
    pseudomonas.ui \
    dragwindows.ui \
    dragsmartcontorl.ui \
    about.ui


RESOURCES += \
    image.qrc

OTHER_FILES += \
    test.qss \
    zx.rc

RC_FILE = zx.rc
