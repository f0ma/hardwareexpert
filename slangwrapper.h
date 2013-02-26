#ifndef SLANGWRAPPER_H
#define SLANGWRAPPER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QRegExp>
#include <QVector>
#include <QStringList>
#include <QMap>
#include <QFile>
#include <QSettings>
#include <QWaitCondition>
#include <slang.h>
#include <cstdlib>
#include "comportinterface.h"
#include "consoleinterface.h"
#include "memorypagerinterface.h"
#include "lptportinterface.h"
#include "directiointerface.h"
#include "usbinterface.h"
#include "i2cportinterface.h"
#include "slangthread.h"

#include "sys/time.h"


class SLangWrapper : public QObject
{
    Q_OBJECT

    SLangThread * sltr; // Указатель на поток выполнения
    QMap <QString,QString> api; // Список имен обратных вызовов скрипта

    QString currentScript;

public:
    SLangWrapper();

    //Устанавливает указатель на объект через который осуществляется связь скрипта с пользовательским интерфейсом
    void setConsoleInterface(ConsoleInterface * con);

    //Устанавливает указатель на объект через который осуществляется связь скрипта с интерфейсом Hex-редактора
    void setMemoryInterface(MemoryPagerInterface * m);

    //Устанавливает указатель на СOM интерфейс
    void setComPortInterface(ComPortInterface * port);

    //Устанавливает указатель на LPT интерфейс
    void setLptPortInterface(LptPortInterface * port);

    //Устанавливает указатель на I2C интерфейс
    void setI2cPortInterface(I2cPortInterface * port);

    //Устанавливает указатель на DIO интерфейс
    void setDirectIoPortInterface(DirectIOInterface * port);

    //Устанавливает указатель на USB интерфейс
    void setUsbPortInterface(UsbPortInterface * port);

    //Задает размещение скрипта
    void setScriptPath(QString path);

    //Компилирует и выполняет скрипт
    void run(QString script);

    //Вызывает функцию описанную в скрипте
    void execute(QString function);

    //Возвращает информацию об обратных вызовах объявленых в скрипте
    QMap <QString,QString> getApi();

    //прерывает выполнение скрипта
    void stop();

private:

    //освобождает ресурсы после выполнения скрипта
    void finalize();

    //проверяет инициализированность интерфейсов перед началом выполнения
    bool checkIFacePointers();

    //перехватчики сообщений об ошибках интерпритатора
    static void slang_error_hook (char *fmt, va_list ap);
    static void slang_error_hook (char *txt);


signals:
    //сигнал о начале выполнения
    void executionStarted();

    //сигнал об окончании выполнения
    void executionEnded();

private:

    //преобразование массива к внутреннему представлению интерпрететора
    static SLang_Array_Type * arrayFromByteArray(QByteArray a);

    //обратное преобразование
    static QByteArray byteArrayFromArray(SLang_Array_Type * ba);

    //! Блок функций - интриников. Функции с приставкой instr_ реализуют соответсвующие функции API скритов.

    static int instr_setbit(int * m,int * bit,int * val);
    static int instr_getbit(int * m,int * num);

    /// Функции по работе с GUI

    static void instr_print (void);
    static void instr_rmline ();
    static void instr_clear ();

    static void instr_wait(int * time);

    static void instr_progress (int * i);
    static void instr_progmax (int * i);
    static void instr_progwait ();
    static void instr_progclear ();

    static int instr_question(char * tit,char * txt);

    static void instr_optionclear ();
    static void instr_option ();
    static void instr_optiondialog ();
    static void instr_value (char * name);

    /// Функции по работе с массивами

    static char * instr_arrayAsString (SLang_Array_Type * ba);
    static void instr_arrayFromInt(int * v,int * field);
    static void instr_arrayFromHex(char * str);
    static void instr_arrayFromAscii(char * str);

    static void instr_concat(SLang_Array_Type * ba1,SLang_Array_Type * ba2);
    static void instr_copyArray(SLang_Array_Type * ba);
    static void instr_subArray(SLang_Array_Type * ba, int * start,int *len);

    static void instr_fill (void);
    static void instr_load (char * str);

    /// Функции по работе с Hex-редактором

    static void instr_loadPage();
    static void instr_overwritePage(SLang_Array_Type *  ba);
    static void instr_storePage(SLang_Array_Type *  ba, char * name);
    static void instr_deletePage();

    /// Функции по работе с COM портом

    static int instr_ComOpen (int * baudrate, int * partity,int * dataBits, int * stopBits);
    static void instr_ComClose ();
    static void instr_ComWrite (SLang_Array_Type *  ba);
    static void instr_ComRead ();
    static int instr_ComBytesAvailable ();
    static void instr_ComSetDTR (int * e);
    static void instr_ComSetRTS (int * e);
    static void instr_SetTimeout (int * e);

    /// Функции по работе с LPT портом

    static int  instr_LptOpen();
    static void instr_LptSetData(int * d);
    static int instr_LptGetData();
    static void instr_LptSetCtrl(int * d);
    static int instr_LptGetStat();
    static void instr_LptSetDataMode(int * mode);
    static void instr_LptClose();

    /// Функции по работе с I2C портом

    static int instr_I2COpen();
    static void instr_I2CWrite(int * adr,SLang_Array_Type * ba);
    static void instr_I2CRead(int * adr,int * size);
    static void instr_I2CStop();
    static void instr_I2CWait(int * time);
    static int  instr_I2CGetBufferLength();
    static void  instr_I2CGetBuffer();
    static int instr_I2CPreform();
    static void instr_I2CClose();

    static void instr_I2CsetAskCheck(int * a);
    static int instr_I2CisOk();

    static void instr_I2CDirectWrite(int * adr,SLang_Array_Type * ba);
    static void  instr_I2CDirectRead(int * adr,int * size);
    static void  instr_I2CDirectWriteRead(int * adr,SLang_Array_Type * baw,int * size);

    /// Функции по работе с DIO портом

    static int instr_DIOOpen();
    static void instr_DIOClose();
    static void instr_DIOSetPort(int * port,int * val);
    static int instr_DIOGetPort(int * port);
    static void instr_DIOSetPinMode (int * port, int *pin, int *mode);
    static void instr_DIOSetPortMode (int * port, int *mode);

    /// Функции по работе с USB портом

    static int instr_USBOpen (int * vid,int * pid);
    static void instr_USBClose ();
    static void instr_USBReset ();
    static void instr_USBBulkWrite();
    static void instr_USBBulkRead();
    static void instr_USBControlMsg ();

    /// Функции по работе с ini файлом

    static void instr_configLoad (char * str);
    static char * instr_configValue (char * hkey,char * key);

};


#endif // SLANGWRAPPER_H
