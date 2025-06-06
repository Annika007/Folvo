QT       += core gui
QT+=widgets
QT+=network
win32:CONFIG+=consloe
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AIService.cpp \
    AnalysisDialog.cpp \
    DataModel.cpp \
    FinancialAnalyzer.cpp \
    SavingsPlanAdapter.cpp \
    TransactionViewWindow.cpp \
    main.cpp \
    mainwindow.cpp \
    myprofilewindow.cpp \
    splashscreen.cpp

HEADERS += \
    AIService.h \
    AnalysisDialog.h \
    DataModel.h \
    FinancialAnalyzer.h \
    SavingsPlanAdapter.h \
    Transaction.h \
    TransactionViewWindow.h \
    mainwindow.h \
    myprofilewindow.h \
    splashscreen.h

FORMS += \
    mainwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
