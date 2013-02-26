///////////////////////////////////////////////////////////////////////
//HardwareExpert. Wrapper for S-Lang engine.
//Copyright (C) 2011  Stanislav (f0ma) Ivanov
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////

#include "slangwrapper.h"
#include "Timer.h"

#include "qextserialport.h"

namespace SLangWrapperIFaces //указатели на внешние интерфейсы
{
    ComPortInterface * comPort;
    LptPortInterface * lptPort;
    I2cPortInterface * i2cPort;
    UsbPortInterface * usbPort;
    DirectIOInterface * dioPort;

    ConsoleInterface * console;
    MemoryPagerInterface * memtab;

    QVector<ConsoleInterface::optionDialogEntry> optList;

    QString scriptPath;
    Timer hiResTimer;
    QSettings * settingsForScript = NULL;
}

using namespace SLangWrapperIFaces;

namespace SLangWrapperConst //константы, используемые в интерпретаторе
{
    // константы
    int iFALSE = 0;
    int iTRUE = 1;

    // константы COM порта
    int iBAUD9600 = (int)BAUD9600;
    int iBAUD19200 = (int)BAUD19200;
    int iBAUD115200 = (int)BAUD115200;

    int iDATA_5 = (int)DATA_5;
    int iDATA_6 = (int)DATA_6;
    int iDATA_7 = (int)DATA_7;
    int iDATA_8 = (int)DATA_8;

    int iPAR_NONE = (int)PAR_NONE;
    int iPAR_ODD = (int)PAR_ODD;
    int iPAR_EVEN = (int)PAR_EVEN;

    int iSTOP_1 = (int)STOP_1;
    int iSTOP_2 = (int)STOP_2;

    // константы DIO порта
    int iDIO_OUT = 0;
    int iDIO_IN = 1;

    // константы функции print
    char * mod_hex = (char *) "%HEX";
    char * mod_dec = (char *) "%DEC";
    char * mod_bin = (char *) "%BIN";
    char * mod_endl = (char *) "%ENDL";

    // константы опций
    int iOPT_CHK = (int)ConsoleInterface::OPT_CHK;
    int iOPT_DEC = (int)ConsoleInterface::OPT_DEC;
    int iOPT_HEX = (int)ConsoleInterface::OPT_HEX;
    int iOPT_LIST = (int)ConsoleInterface::OPT_LIST;
    int iOPT_STR = (int)ConsoleInterface::OPT_STR;
}

//Макросы проверки числа аргументов.

#define checkByteArray(ba) if(ba->data_type!=SLANG_UCHAR_TYPE  && ba->data_type!=SLANG_INT_TYPE) { SLang_set_error(SL_Data_Error); console->print("Error: Array elements type missmach.\n"); SLang_set_error(SL_InvalidParm_Error); return;}
#define checkByteArray0(ba) if(ba->data_type!=SLANG_UCHAR_TYPE && ba->data_type!=SLANG_INT_TYPE) { SLang_set_error(SL_Data_Error); console->print("Error: Array elements type missmach.\n"); SLang_set_error(SL_InvalidParm_Error); return 0;}

#define checkArgCount(x) if(SLang_Num_Function_Args!=x) {SLang_set_error(SL_NumArgs_Error); return;}
#define checkArgCount0(x) if(SLang_Num_Function_Args!=x) {SLang_set_error(SL_NumArgs_Error); return 0;}

void SLangWrapper::setScriptPath(QString path)
{
    SLangWrapperIFaces::scriptPath = path;
}

QMap <QString,QString> SLangWrapper::getApi()
{
    return api;
}

void SLangWrapper::run(QString script)
{
    //Препроцессор языка

    script.insert(0," ");

    script.replace("\nvar ","\nvariable ");
    script.replace(QRegExp("(\\s|;)var ")," variable ");
    script.replace("\ndef ","\ndefine ");
    script.replace(QRegExp("(\\s|;)def ")," define ");
    script.replace(QRegExp("=\\s*ByteArray\\s*\\["),"= UChar_Type [");

    //Регистрация обратных вызовов

    api.clear();

    QRegExp rx("defapi\\s+(\\w+)\\s*\\(\\s*\\)\\s*:\\s*\"(.*)\"");
    rx.setMinimal(true);
    int pos = 0;

    while ((pos = rx.indexIn(script, pos)) != -1)
        {
            api[rx.cap(1)] = rx.cap(2);
            pos += rx.matchedLength();
        }

    script.replace(rx,"define \\1 ()");

    script.replace("defapi","define");

    //Linux support bin

    QRegExp binrx("0b([01]+)");
    pos = 0;

    while ((pos = binrx.indexIn(script, pos)) != -1)
        {
            QString bs,ds;
            bs = binrx.cap(1);
            ds.setNum(bs.toInt(NULL,2),10);

            script.replace(pos,binrx.matchedLength(),ds);

            pos += binrx.matchedLength();
        }

    script.replace(rx,"define \\1 ()");

    //Конец работы препроцессора

    currentScript = script;

    if(!checkIFacePointers()) return;

    if(sltr!=NULL && sltr->isFinished()) finalize();
    else stop();

    optList.clear();

    sltr = new SLangThread();

    connect(sltr,SIGNAL(finished()),this,SIGNAL(executionEnded()));

    SLang_restart(1);
    SLang_set_error(0);
    sltr->setScript(script);
    emit executionStarted();
    sltr->start(QThread::LowPriority);
}

bool SLangWrapper::checkIFacePointers()
{
    if(comPort==NULL)
        {
            console->print("Invalid configuration COM Port!\n");
            return false;
        }
    if(lptPort==NULL)
        {
            console->print("Invalid configuration LPT Port!\n");
            return false;
        }
    if(i2cPort==NULL)
        {
            console->print("Invalid configuration I2C Port!\n");
            return false;
        }
    if(usbPort==NULL)
        {
//            console->print("Invalid configuration USB Port!\n");
//            return false;
        }
    if(dioPort==NULL)
        {
//            console->print("Invalid configuration DirectIO Port!\n");
//            return false;
        }

    return true;
}

void SLangWrapper::execute(QString function)
{
    if(sltr!=NULL && sltr->isFinished()) finalize();
    else stop();

    if(!checkIFacePointers()) return;

    sltr = new SLangThread();

    connect(sltr,SIGNAL(finished()),this,SIGNAL(executionEnded()));

    SLang_restart(1);
    SLang_set_error(0);
    sltr->setFunction(function);
    emit executionStarted();
    sltr->start(QThread::LowPriority);
}

void SLangWrapper::stop()
{
    if(sltr==NULL)return;
    if(sltr->isFinished())
        {
            finalize();
            return;
        }

    SLang_set_error(SL_UserBreak_Error); //установка ошибки UserBreak
    sltr->wait(2000); //ожидание завершения потока

    SLang_restart(1);
    finalize();
    emit executionEnded();
}

void SLangWrapper::finalize()
{
    delete sltr;
    sltr=NULL;
}

SLangWrapper::SLangWrapper()
{
    //Инициализация интерпретатора
    SLang_init_slang ();
    SLutf8_enable (1);
    SLinterp_utf8_enable (1);

    SLang_restart (1);
    SLang_set_error (0);
    SLang_Exit_Error_Hook = slang_error_hook;
    SLang_Error_Hook = slang_error_hook;
    SLang_Dump_Routine = slang_error_hook;

    //Регистрация и функций констант

    SLadd_intrinsic_variable ((char *) "TRUE",& SLangWrapperConst::iTRUE, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "FALSE",& SLangWrapperConst::iFALSE, SLANG_INT_TYPE, 1);

    SLadd_intrinsic_function ((char *) "setBit", (FVOID_STAR) instr_setbit, SLANG_INT_TYPE,3,SLANG_INT_TYPE,SLANG_INT_TYPE,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "getBit", (FVOID_STAR) instr_getbit, SLANG_INT_TYPE,2,SLANG_INT_TYPE,SLANG_INT_TYPE);

    SLadd_intrinsic_function ((char *) "wait", (FVOID_STAR) instr_wait, SLANG_VOID_TYPE,1,SLANG_INT_TYPE);

    SLadd_intrinsic_function ((char *) "clear", (FVOID_STAR) instr_clear,SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "cr", (FVOID_STAR) instr_rmline,SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "print", (FVOID_STAR) instr_print,SLANG_VOID_TYPE,0);

    SLadd_intrinsic_function ((char *) "progValue", (FVOID_STAR) instr_progress,SLANG_VOID_TYPE,1,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "progMax", (FVOID_STAR) instr_progmax,SLANG_VOID_TYPE,1,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "progWait", (FVOID_STAR) instr_progwait,SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "progClear", (FVOID_STAR) instr_progclear,SLANG_VOID_TYPE,0);

    SLadd_intrinsic_variable ((char *) "OPT_CHK",& SLangWrapperConst::iOPT_CHK, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "OPT_DEC",& SLangWrapperConst::iOPT_DEC, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "OPT_HEX",& SLangWrapperConst::iOPT_HEX, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "OPT_LIST",& SLangWrapperConst::iOPT_LIST, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "OPT_STR",& SLangWrapperConst::iOPT_STR, SLANG_INT_TYPE, 1);

    SLadd_intrinsic_function ((char *) "optClear", (FVOID_STAR) instr_optionclear,SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "optOption", (FVOID_STAR) instr_option,SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "optDialog", (FVOID_STAR) instr_optiondialog,SLANG_VOID_TYPE,0);

    SLadd_intrinsic_function ((char *) "ask", (FVOID_STAR) instr_question,SLANG_INT_TYPE,2,SLANG_STRING_TYPE,SLANG_STRING_TYPE);

    SLadd_intrinsic_function ((char *) "fromInt", (FVOID_STAR) instr_arrayFromInt, SLANG_VOID_TYPE,2,SLANG_INT_TYPE,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "fromHex", (FVOID_STAR) instr_arrayFromHex, SLANG_VOID_TYPE,1,SLANG_STRING_TYPE);
    SLadd_intrinsic_function ((char *) "fromAscii", (FVOID_STAR) instr_arrayFromAscii, SLANG_VOID_TYPE,1,SLANG_STRING_TYPE);
    SLadd_intrinsic_function ((char *) "concat", (FVOID_STAR) instr_concat, SLANG_VOID_TYPE,2,SLANG_ARRAY_TYPE,SLANG_ARRAY_TYPE);
    SLadd_intrinsic_function ((char *) "subArray", (FVOID_STAR) instr_subArray, SLANG_VOID_TYPE,3,SLANG_ARRAY_TYPE,SLANG_INT_TYPE,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "toString", (FVOID_STAR) instr_arrayAsString, SLANG_STRING_TYPE,1,SLANG_ARRAY_TYPE);
    SLadd_intrinsic_function ((char *) "copy", (FVOID_STAR) instr_copyArray, SLANG_VOID_TYPE,1,SLANG_ARRAY_TYPE);
    SLadd_intrinsic_function ((char *) "fill", (FVOID_STAR) instr_fill, SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "load", (FVOID_STAR) instr_load, SLANG_VOID_TYPE,1,SLANG_STRING_TYPE);

    SLadd_intrinsic_function ((char *) "pageLoad", (FVOID_STAR) instr_loadPage, SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "pageOverwrite", (FVOID_STAR) instr_overwritePage, SLANG_VOID_TYPE,1,SLANG_ARRAY_TYPE);
    SLadd_intrinsic_function ((char *) "pageStore", (FVOID_STAR) instr_storePage,SLANG_VOID_TYPE,2,SLANG_ARRAY_TYPE,SLANG_STRING_TYPE);
    SLadd_intrinsic_function ((char *) "pageDelete", (FVOID_STAR) instr_deletePage, SLANG_VOID_TYPE,0);

    SLadd_intrinsic_variable ((char *) "HEX",&SLangWrapperConst::mod_hex, SLANG_STRING_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "DEC",&SLangWrapperConst::mod_dec, SLANG_STRING_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "BIN",&SLangWrapperConst::mod_bin, SLANG_STRING_TYPE, 1);

    SLadd_intrinsic_variable ((char *) "BAUD_9600",& SLangWrapperConst::iBAUD9600, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "BAUD_19200",& SLangWrapperConst::iBAUD19200, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "BAUD_115200",& SLangWrapperConst::iBAUD115200, SLANG_INT_TYPE, 1);

    SLadd_intrinsic_variable ((char *) "DATA_5",& SLangWrapperConst::iDATA_5, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "DATA_6",& SLangWrapperConst::iDATA_6, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "DATA_7",& SLangWrapperConst::iDATA_7, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "DATA_8",& SLangWrapperConst::iDATA_8, SLANG_INT_TYPE, 1);

    SLadd_intrinsic_variable ((char *) "PAR_NONE",& SLangWrapperConst::iPAR_NONE, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "PAR_ODD",& SLangWrapperConst::iPAR_ODD, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "PAR_EVEN",& SLangWrapperConst::iPAR_EVEN, SLANG_INT_TYPE, 1);

    SLadd_intrinsic_variable ((char *) "STOP_1",& SLangWrapperConst::iSTOP_1, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "STOP_2",& SLangWrapperConst::iSTOP_2, SLANG_INT_TYPE, 1);


    SLadd_intrinsic_function ((char *) "comOpen", (FVOID_STAR) instr_ComOpen, SLANG_INT_TYPE,4, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "comClose", (FVOID_STAR) instr_ComClose, SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "comWrite", (FVOID_STAR) instr_ComWrite, SLANG_VOID_TYPE,1,SLANG_ARRAY_TYPE);
    SLadd_intrinsic_function ((char *) "comRead", (FVOID_STAR) instr_ComRead, SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "comRecived", (FVOID_STAR) instr_ComBytesAvailable, SLANG_INT_TYPE,0);
    SLadd_intrinsic_function ((char *) "comSetDtr", (FVOID_STAR) instr_ComSetDTR, SLANG_VOID_TYPE,1,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "comSetRts", (FVOID_STAR) instr_ComSetRTS, SLANG_VOID_TYPE,1,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "comSetTimeout", (FVOID_STAR) instr_SetTimeout, SLANG_VOID_TYPE,1,SLANG_INT_TYPE);

    SLadd_intrinsic_function ((char *) "lptOpen", (FVOID_STAR) instr_LptOpen, SLANG_INT_TYPE,0);
    SLadd_intrinsic_function ((char *) "lptSetData", (FVOID_STAR) instr_LptSetData, SLANG_VOID_TYPE,1,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "lptGetData", (FVOID_STAR) instr_LptGetData, SLANG_INT_TYPE,0);
    SLadd_intrinsic_function ((char *) "lptSetCtrl", (FVOID_STAR) instr_LptSetCtrl, SLANG_VOID_TYPE,1,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "lptGetStat", (FVOID_STAR) instr_LptGetStat, SLANG_INT_TYPE,0);
    SLadd_intrinsic_function ((char *) "lptSetMode", (FVOID_STAR) instr_LptSetDataMode, SLANG_VOID_TYPE,1,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "lptClose", (FVOID_STAR) instr_LptClose, SLANG_VOID_TYPE,0);


    SLadd_intrinsic_function ((char *) "i2cOpen", (FVOID_STAR) instr_I2COpen, SLANG_INT_TYPE,0);
    SLadd_intrinsic_function ((char *) "i2cAddWrite", (FVOID_STAR) instr_I2CWrite, SLANG_VOID_TYPE,2,SLANG_INT_TYPE,SLANG_ARRAY_TYPE);
    SLadd_intrinsic_function ((char *) "i2cAddRead", (FVOID_STAR) instr_I2CRead, SLANG_VOID_TYPE,2,SLANG_INT_TYPE,SLANG_ARRAY_TYPE);
    SLadd_intrinsic_function ((char *) "i2cAddStop", (FVOID_STAR) instr_I2CStop, SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "i2cAddWait", (FVOID_STAR) instr_I2CWrite, SLANG_INT_TYPE,1,SLANG_ARRAY_TYPE);


    SLadd_intrinsic_function ((char *) "i2cWrite", (FVOID_STAR) instr_I2CDirectWrite, SLANG_VOID_TYPE,2,SLANG_INT_TYPE,SLANG_ARRAY_TYPE);
    SLadd_intrinsic_function ((char *) "i2cRead", (FVOID_STAR) instr_I2CDirectRead, SLANG_VOID_TYPE,2,SLANG_INT_TYPE,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "i2cWriteRead", (FVOID_STAR) instr_I2CDirectWriteRead, SLANG_VOID_TYPE,3,SLANG_INT_TYPE,SLANG_ARRAY_TYPE,SLANG_INT_TYPE);

    SLadd_intrinsic_function ((char *) "i2cAskCheck", (FVOID_STAR) instr_I2CsetAskCheck, SLANG_VOID_TYPE,1,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "i2cIsOk", (FVOID_STAR) instr_I2CisOk, SLANG_INT_TYPE,0);

    SLadd_intrinsic_function ((char *) "i2cGetBuffer", (FVOID_STAR) instr_I2CGetBuffer, SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "i2cGetBufferLength", (FVOID_STAR) instr_I2CGetBufferLength, SLANG_INT_TYPE,0);

    SLadd_intrinsic_function ((char *) "i2cPreform", (FVOID_STAR) instr_I2CPreform, SLANG_INT_TYPE,0);
    SLadd_intrinsic_function ((char *) "i2cClose", (FVOID_STAR) instr_I2CClose, SLANG_VOID_TYPE,0);


    SLadd_intrinsic_function ((char *) "dioOpen", (FVOID_STAR) instr_DIOOpen, SLANG_INT_TYPE,0);
    SLadd_intrinsic_function ((char *) "dioSetPort", (FVOID_STAR) instr_DIOSetPort, SLANG_VOID_TYPE,2,SLANG_INT_TYPE,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "dioGetPort", (FVOID_STAR) instr_DIOGetPort, SLANG_INT_TYPE,1,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "dioSetPinMode", (FVOID_STAR) instr_DIOSetPinMode, SLANG_VOID_TYPE,3,SLANG_INT_TYPE,SLANG_INT_TYPE,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "dioSetPortMode", (FVOID_STAR) instr_DIOSetPortMode, SLANG_VOID_TYPE,2,SLANG_INT_TYPE,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "dioClose", (FVOID_STAR) instr_DIOClose, SLANG_VOID_TYPE,0);

    SLadd_intrinsic_variable ((char *) "OUT",& SLangWrapperConst::iDIO_OUT, SLANG_INT_TYPE, 1);
    SLadd_intrinsic_variable ((char *) "IN",& SLangWrapperConst::iDIO_IN, SLANG_INT_TYPE, 1);


    SLadd_intrinsic_function ((char *) "usbOpen", (FVOID_STAR) instr_USBOpen, SLANG_INT_TYPE,2,SLANG_INT_TYPE,SLANG_INT_TYPE);
    SLadd_intrinsic_function ((char *) "usbClose", (FVOID_STAR) instr_USBClose, SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "usbReset", (FVOID_STAR) instr_USBReset, SLANG_VOID_TYPE,0);

    SLadd_intrinsic_function ((char *) "usbBulkRead", (FVOID_STAR) instr_USBBulkRead, SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "usbBulkWrite", (FVOID_STAR) instr_USBBulkWrite, SLANG_VOID_TYPE,0);
    SLadd_intrinsic_function ((char *) "usbControlMsg", (FVOID_STAR) instr_USBControlMsg, SLANG_VOID_TYPE,0);


    SLadd_intrinsic_function ((char *) "configLoad", (FVOID_STAR) instr_configLoad, SLANG_VOID_TYPE,1,SLANG_STRING_TYPE);
    SLadd_intrinsic_function ((char *) "configValue", (FVOID_STAR) instr_configValue, SLANG_STRING_TYPE,2,SLANG_STRING_TYPE,SLANG_STRING_TYPE);

    //Обнуление

    sltr=NULL;

    comPort=NULL;
    lptPort=NULL;
    i2cPort=NULL;
    usbPort=NULL;
    dioPort=NULL;

}

void SLangWrapper::setConsoleInterface(ConsoleInterface * con)
{
    console=con;
}

void SLangWrapper::setI2cPortInterface(I2cPortInterface * port)
{
    i2cPort=port;
}

void SLangWrapper::setComPortInterface(ComPortInterface * port)
{
    comPort=port;
}

void SLangWrapper::setLptPortInterface(LptPortInterface * port)
{
    lptPort=port;
}

void SLangWrapper::setDirectIoPortInterface(DirectIOInterface * port)
{
    dioPort=port;
}

void SLangWrapper::setUsbPortInterface(UsbPortInterface * port)
{
    usbPort=port;
}

void SLangWrapper::setMemoryInterface(MemoryPagerInterface * m)
{
    memtab=m;
}

int SLangWrapper::instr_setbit(int * m,int * bit,int * val)
{
    checkArgCount0(3);
    int r =*m;
    if ((*val)==0)
        {
            r=r & (0xFFFFFFFF - ((int)1<<(*bit)));
        }
    else
        {
            r=r | ((int)1<<(*bit));
        }
    return r;
}

int SLangWrapper::instr_getbit(int * m,int * num)
{
    checkArgCount0(2);
    return (*m & (1<<(*num))) >> (*num);
}

void SLangWrapper::slang_error_hook (char *fmt, va_list ap)
{
    char buffer[256];
    vsprintf (buffer,fmt, ap);

    slang_error_hook (buffer);

};

void SLangWrapper::slang_error_hook (char *txt)
{
    QString a(txt);
    if(a=="block context stack underflow")return;

    if(a.startsWith("***string***"))
        {
            QStringList el = a.split(":");
            if (el[2]=="<top-level>")el[2]="";
            console->print(QObject::tr("\n Error at line: ")+el[1]+" : " + el[2] + " " + el[3]);
            return;
        }

    console->print("\n"+a);
};

void SLangWrapper::instr_fill (void)
{
    int st,len,num = SLang_Num_Function_Args;
    char val;
    SLang_Array_Type * at;
    QByteArray rs;

    if(num!=2 && num!=3  && num!=4)
        {
            SLang_set_error(SL_NumArgs_Error);
            return;
        }

    if(num==2)
        {
            if (SLang_pop_char(&val)) return;
            if (SLang_pop_array(&at,false)) return;

            checkByteArray(at);

            rs = byteArrayFromArray(at);

            rs.fill(val);

            SLang_push_array(arrayFromByteArray(rs),true);

        }

    if(num==3)
        {
            if (SLang_pop_int(&len)) return;
            if (SLang_pop_char(&val)) return;
            if (SLang_pop_array(&at,false)) return;

            checkByteArray(at);

            if((unsigned int)len>at->num_elements)len=at->num_elements;

            rs = byteArrayFromArray(at);

            for(unsigned int i=0; i<(unsigned int)len; i++)rs[i]=val;

            SLang_push_array(arrayFromByteArray(rs),true);


            return;
        }

    if(num==4)
        {
            if (SLang_pop_int(&len)) return;
            if (SLang_pop_int(&st)) return;
            if (SLang_pop_char(&val)) return;
            if (SLang_pop_array(&at,false)) return;

            checkByteArray(at);

            if((unsigned int)st>at->num_elements)return;

            unsigned int end=len+st;

            rs = byteArrayFromArray(at);

            if(end>at->num_elements)end=at->num_elements;


            for(unsigned int i=st; i<end; i++)rs[i]=val;

            SLang_push_array(arrayFromByteArray(rs),true);

            return;
        }
}

struct argum //структура описывает элемент вывода функции print
{
    enum {STR,NUM} type;
    QString str;
    int num;
};

void SLangWrapper::instr_print (void)
{
    char *strptr;
    int val;
    unsigned char d;
    SLang_Array_Type * at;
    unsigned int num = SLang_Num_Function_Args;
    argum arg;
    QList<argum> arl;

    for(; num>0; num--)
        {
            if (SLANG_ARRAY_TYPE == SLang_peek_at_stack ())
                {
                    if (SLang_pop_array(&at,0)==-1) return;

                    arg.type=argum::STR;
                    QString ns;
                    QByteArray sr = byteArrayFromArray(at);
                    ns.setNum(sr.count());
                    arg.str="["+sr.toHex().toUpper()+"|"+ns+"]";
                    arl.prepend(arg);
                    continue;
                }

            if (SLANG_STRING_TYPE == SLang_peek_at_stack ())
                {
                    if (SLang_pop_slstring(&strptr)==-1) return;

                    arg.type=argum::STR;
                    arg.str=arg.str.fromUtf8(strptr);
                    arl.prepend(arg);
                    continue;
                }


            if (SLANG_INT_TYPE == SLang_peek_at_stack ())
                {
                    if (SLang_pop_integer(&val)) return;

                    arg.type=argum::NUM;
                    arg.num=val;
                    arl.prepend(arg);
                    continue;
                }

            if (SLANG_UCHAR_TYPE == SLang_peek_at_stack ())
                {
                    if (SLang_pop_uchar(&d)) return;

                    arg.type=argum::NUM;
                    arg.num=(unsigned char)d;
                    arl.prepend(arg);
                    continue;
                }


        }

    enum {HEX,DEC,BIN,ARR} print_as;
    print_as=HEX;

    for(int i=0; i<arl.size(); i++)
        {
            if(arl.at(i).type==argum::STR)
                {
                    if(arl.at(i).str=="%HEX")
                        {
                            print_as=HEX;
                            continue;
                        }
                    if(arl.at(i).str=="%DEC")
                        {
                            print_as=DEC;
                            continue;
                        }
                    if(arl.at(i).str=="%BIN")
                        {
                            print_as=BIN;
                            continue;
                        }
                    console->print(arl.at(i).str);
                }

            if(arl.at(i).type==argum::NUM)
                {
                    QString s;

                    if(print_as==HEX)
                        {
                            s.setNum(arl.at(i).num,16);
                            if(abs(arl.at(i).num)<0x10) s="0"+s.right(1);
                            if(abs(arl.at(i).num)<0x100) s=s.right(2);

                            if(s.size()%2==1)s="0"+s;
                            if(abs(arl.at(i).num)>arl.at(i).num)s="-"+s;

                            console->print(s.toUpper());
                        }
                    if(print_as==DEC)
                        {
                            s.setNum(arl.at(i).num,10);
                            console->print(s);
                            print_as=HEX;
                        }
                    if(print_as==BIN)
                        {
                            s.setNum(arl.at(i).num,2);
                            while(s.length()%8 != 0)s.prepend("0");
                            console->print(s);
                            print_as=HEX;
                        }
                }
        }
}


void SLangWrapper::instr_rmline ()
{
    checkArgCount(0);
    console->rmline();
}

void SLangWrapper::instr_clear ()
{
    checkArgCount(0);
    console->clear();
}

void SLangWrapper::instr_wait(int * time)
{
    checkArgCount(1);

    hiResTimer.start();

    while(hiResTimer.getElapsedTimeInMicroSec()<*time && SLang_get_error() == 0){}

    hiResTimer.stop();

}


void SLangWrapper::instr_progress (int * i)
{
    checkArgCount(1);
    console->setProgressValue(*i);
}


void SLangWrapper::instr_progmax (int * i)
{
    checkArgCount(1);
    console->setProgressMax(*i);
}


void SLangWrapper::instr_progwait ()
{
    checkArgCount(0);
    console->setProgressWait();
}

void SLangWrapper::instr_progclear ()
{
    checkArgCount(0);
    console->clearProgress();
}

int SLangWrapper::instr_ComOpen (int*  baudrate, int* partity,int* dataBits, int* stopBits)
{
    checkArgCount0(4);
    if(!comPort->open(*baudrate,FLOW_OFF,*partity,*dataBits,*stopBits))
        {
            console->print(QObject::tr("Can't open COM port\n"));
            return -1;
        }
    return 0;
}

void SLangWrapper::instr_ComClose ()
{
    checkArgCount(0);
    comPort->close();
}

void SLangWrapper::instr_ComWrite (SLang_Array_Type * ba)
{
    checkArgCount(1);

    comPort->write(byteArrayFromArray(ba));

}

void SLangWrapper::instr_ComRead ()
{
    checkArgCount(0);
    SLang_push_array(arrayFromByteArray(comPort->read()),true);
}

int SLangWrapper::instr_ComBytesAvailable ()
{
    checkArgCount0(0);
    return comPort->bytesAvailable();
}

void SLangWrapper::instr_ComSetDTR (int * e)
{
    checkArgCount(1);
    return comPort->setDTR(*e);
}

void SLangWrapper::instr_ComSetRTS (int * e)
{
    checkArgCount(1);
    return comPort->setRTS(*e);
}

SLang_Array_Type * SLangWrapper::arrayFromByteArray(QByteArray a)
{
    int novl=a.size();

    char * arr = (char *)SLmalloc(novl);

    memcpy(arr,a.data(),a.size());

    SLang_Array_Type *r =SLang_create_array (SLANG_UCHAR_TYPE, novl, arr, (SLindex_Type*)(&novl), 1);
    r->flags=SLARR_DERIVED_FROM_SCALAR;

    return r;
}

QByteArray SLangWrapper::byteArrayFromArray(SLang_Array_Type * ba)
{
    checkByteArray0(ba);

    QByteArray r;

    if(ba->data_type==SLANG_INT_TYPE)
    {
        int* arr = (int*)ba->data;
        r.fill(0,ba->num_elements);
        for(unsigned int i=0;i<ba->num_elements;i++)
            r[i]=arr[i];
    }

    if(ba->data_type==SLANG_UCHAR_TYPE)
    {
        r.append((char*)ba->data,ba->num_elements);
    }

    return r;
}

void SLangWrapper::instr_copyArray(SLang_Array_Type * ba)
{
    checkArgCount(1);

    int novl=ba->num_elements;

    int msize=novl;

    char * arr = (char *)SLmalloc(msize);

    SLang_Array_Type *r =SLang_create_array (SLANG_UCHAR_TYPE, msize, arr, (SLindex_Type*)(&novl), 1);


    for(unsigned int i=0; i<ba->num_elements; i++)
        {
            arr[i]= ((char*)ba->data)[i];
        }

    SLang_push_array(r,true);
}

void SLangWrapper::instr_concat(SLang_Array_Type * ba1,SLang_Array_Type * ba2)
{
    checkArgCount(2);

    int novl=(ba1->num_elements+ba2->num_elements);

    int msize=novl;

    char * arr = (char *)SLmalloc(msize);

    SLang_Array_Type *r =SLang_create_array (SLANG_UCHAR_TYPE, msize, arr, (SLindex_Type*)(&novl), 1);


    for(unsigned int i=0; i<ba1->num_elements; i++)
        {
            arr[i]= ((char*)ba1->data)[i];
        }
    for(unsigned int i=ba1->num_elements; i<ba2->num_elements+ba1->num_elements; i++)
        {
            arr[i]= ((char*)ba2->data)[i-ba1->num_elements];
        }

    SLang_push_array(r,true);
}



void SLangWrapper::instr_loadPage()
{
    checkArgCount(0);

    QByteArray  n;

    n.append(memtab->getCurrentPage());

    if(n.size()==0)
        {
            console->print(QObject::tr("Warning: current page empty or not exist.\n"));
            SLang_set_error(SL_Data_Error);
            return;
        }

    SLang_push_array(arrayFromByteArray(n),true);
}

void SLangWrapper::instr_overwritePage(SLang_Array_Type * ba)
{
    checkArgCount(1);
    QByteArray * a = new QByteArray();
    *a = byteArrayFromArray(ba);
    memtab->setCurrentPage(a);

}

void SLangWrapper::instr_storePage(SLang_Array_Type * ba, char * name)
{
    checkArgCount(2);
    memtab->addPage(byteArrayFromArray(ba),name);
}

void SLangWrapper::instr_deletePage()
{
    checkArgCount(0);
    memtab->deleteCurrentPage();
}

int  SLangWrapper::instr_LptOpen()
{
    checkArgCount0(0);
    if(lptPort->open())return 0;
    return -1;
}

void SLangWrapper::instr_LptSetData(int * d)
{
    checkArgCount(1);
    lptPort->setDataPins((unsigned char)(*d));
}

int SLangWrapper::instr_LptGetData()
{
    checkArgCount0(0);
    return lptPort->getDataPins();
}

void SLangWrapper::instr_LptSetCtrl(int * d)
{
    checkArgCount(1);
    lptPort->setCtrlPins((unsigned char)(*d));
}

int SLangWrapper::instr_LptGetStat()
{
    checkArgCount0(0);
    return lptPort->getStatPins();
}

void SLangWrapper::instr_LptSetDataMode(int * mode)
{
    checkArgCount(1);
    if(SLangWrapperConst::iDIO_IN==(*mode))lptPort->setDataModeIn(true);
    if(SLangWrapperConst::iDIO_OUT==(*mode))lptPort->setDataModeIn(false);
}

void SLangWrapper::instr_LptClose()
{
    checkArgCount(0);
    lptPort->close();
}

int SLangWrapper::instr_I2COpen()
{
    checkArgCount0(0);
    return i2cPort->open();
}

void SLangWrapper::instr_I2CWrite(int * adr,SLang_Array_Type * ba)
{
    checkArgCount(2);
    i2cPort->addWrite(*adr,byteArrayFromArray(ba));
}

void SLangWrapper::instr_I2CRead(int * adr,int * size)
{
    checkArgCount(2);
    i2cPort->addRead(*adr,*size);
}

int SLangWrapper::instr_I2CPreform()
{
    checkArgCount0(0);
    return i2cPort->preform();
}

void SLangWrapper::instr_I2CStop()
{
    checkArgCount(0);
    i2cPort->addStop();
}

void SLangWrapper::instr_I2CClose()
{
    checkArgCount(0);
    i2cPort->close();
}

int  SLangWrapper::instr_I2CGetBufferLength()
{
    checkArgCount0(0);
    return (int) i2cPort->getBufferLength();
}

void SLangWrapper::instr_I2CGetBuffer()
{
    checkArgCount(0);
    SLang_push_array(arrayFromByteArray(i2cPort->getBuffer()),true);
}

void SLangWrapper::instr_I2CWait(int * time)
{
    checkArgCount(1);
    i2cPort->addWait(*time);
}

void SLangWrapper::instr_I2CDirectWrite(int * adr,SLang_Array_Type * ba)
{
    checkArgCount(2);
    i2cPort->writeNow(*adr,byteArrayFromArray(ba));

}

void SLangWrapper::instr_I2CDirectRead(int * adr,int * size)
{
    checkArgCount(2);
    SLang_push_array(arrayFromByteArray(i2cPort->readNow(*adr,*size)),true);
}

void SLangWrapper::instr_I2CDirectWriteRead(int * adr,SLang_Array_Type * baw,int * size)
{
    checkArgCount(3);
    SLang_push_array(arrayFromByteArray(i2cPort->writeReadNow(*adr,byteArrayFromArray(baw),*size)),true);
}

void SLangWrapper::instr_I2CsetAskCheck(int * a)
{
    checkArgCount(1);
    if(*a == 0)i2cPort->checkAsk(false);
    else i2cPort->checkAsk(true);
}

int SLangWrapper::instr_I2CisOk()
{
    checkArgCount0(0);
    return i2cPort->isOk();
}

int SLangWrapper::instr_question(char * tit,char * txt)
{
    checkArgCount0(2);
    int r = console->quest(tit,txt);
    if(r==-1)
        {
            SLang_set_error(SL_UserBreak_Error);
            return 0;
        }
    return r;
}



char * SLangWrapper::instr_configValue (char * hkey,char * key)
{
    checkArgCount0(2);
    if (settingsForScript == NULL)
    {
        console->print(QString("Settings file not loaded. Call settingsLoad() first.\n"));
        return NULL;
    }

    QString str = settingsForScript->value(QString(hkey)+"/"+QString(key),"").toString();
    QByteArray ba = str.toAscii();

    char * data = SLmalloc (ba.size()+1);
    memcpy(data, ba.data(),ba.size());
    data[ba.size()]='\0';

    return data;
}


void SLangWrapper::instr_configLoad (char * str)
{
    checkArgCount(1);
    delete settingsForScript;
    QFile f;
    f.setFileName(SLangWrapperIFaces::scriptPath + "/" + str);
    f.open(QFile::ReadOnly);
    if(f.isOpen())
    {
        f.close();
        settingsForScript = new QSettings(SLangWrapperIFaces::scriptPath + "/" + str,QSettings::IniFormat);
    }
    else
    {
        settingsForScript = NULL;
        console->print(QString("File ")+str+QString(" not found!\n"));
    }
}


void SLangWrapper::instr_load (char * str)
{
    checkArgCount(1);
    QFile f;
    f.setFileName(SLangWrapperIFaces::scriptPath + "/" + str);
    f.open(QFile::ReadOnly);
    QByteArray ba;
    if(f.isOpen())
    {

        ba = f.readAll();
    }
    else
    {
        console->print(QString("File ")+str+QString(" not found!\n"));
    }
    SLang_push_array(arrayFromByteArray(ba),true);
}

void SLangWrapper::instr_arrayFromHex(char * str)
{
    checkArgCount(1);
    SLang_push_array(arrayFromByteArray(QByteArray::fromHex(str)),true);
}

void SLangWrapper::instr_arrayFromAscii(char * str)
{
    checkArgCount(1);
    SLang_push_array(arrayFromByteArray(QByteArray::fromRawData(str,strlen(str))),true);
}

void SLangWrapper::instr_subArray(SLang_Array_Type * ba, int *start,int *len)
{
    checkArgCount(3);
    QByteArray r;

    r = byteArrayFromArray(ba);

    if(*start + *len <= r.length())

    r.remove(0,*start);
    r.remove(*len,r.size()-(*len));

    SLang_push_array(arrayFromByteArray(r),true);
}


void SLangWrapper::instr_arrayFromInt(int * v,int * field)
{
    checkArgCount(2);
    QString str;
    str.setNum(*v,16);

    QByteArray n;
    n.append(str.toAscii());

    if(n.size()>(*field))
        {
            n.remove(0,n.size()-(*field));
        }
    if(n.size()<(*field))
        {
            for(int i=0; i<=(*field)-n.size()+1; i++)n.prepend('0');
        }
    SLang_push_array(arrayFromByteArray(n),true);

}

char * SLangWrapper::instr_arrayAsString (SLang_Array_Type * ba)
{
    checkArgCount0(1);

    QByteArray d;
    d.append(byteArrayFromArray(ba));


    char * data = SLmalloc (d.size()+1);
    memcpy(data, d.data(),d.size());
    data[d.size()]='\0';

    return data;
}

void SLangWrapper::instr_optionclear ()
{
    optList.clear();
}

void SLangWrapper::instr_option ()
{
    int num = SLang_Num_Function_Args;
    SLstr_Type * name;
    SLstr_Type * str;
    int type;

    ConsoleInterface::optionDialogEntry entry;

    if(num!=1 && num!=4)
    {
        SLang_set_error(SL_NumArgs_Error);
        return;
    }

    if(num==1)
    {
        if (SLang_pop_slstring(&name)) return;


        for (int i = 0; i< optList.size();i++)
        {
            if(optList[i].name == name)
            {
                if((optList[i].type == ConsoleInterface::OPT_CHK)||
                (optList[i].type == ConsoleInterface::OPT_LIST)||
                (optList[i].type == ConsoleInterface::OPT_HEX)||
                (optList[i].type == ConsoleInterface::OPT_DEC))
                    SLang_push_int(optList[i].valueInt);

                if(optList[i].type == ConsoleInterface::OPT_STR)
                {
                    QByteArray rest = optList[i].valueStr.toUtf8();
                    int s = rest.size();
                    char * data = SLmalloc (s+1);
                    memcpy(data, rest.data(),s);
                    data[s] = '\0';
                    SLang_push_string(data);
                }
                return;
            }
        }

        SLang_set_error(SL_Index_Error);

        return;
    }

    if(num==4)
    {
        if (SLang_pop_int(&type)) return;

        entry.type=(ConsoleInterface::OPT_TYPE) type;

        SLstr_Type * strval;
        SLang_Array_Type * arrval;

        if((type == ConsoleInterface::OPT_CHK)||
           (type == ConsoleInterface::OPT_HEX)||
           (type == ConsoleInterface::OPT_DEC))
        {
        if (SLang_pop_int(&entry.valueInt)) return;
        if (SLang_pop_slstring(&str)) return;
        if (SLang_pop_slstring(&name)) return;

            entry.name=name;
            entry.text=str;
        }

        if(type == ConsoleInterface::OPT_STR)
        {
        if (SLang_pop_slstring(&strval)) return;
            entry.valueStr = strval;

        if (SLang_pop_slstring(&str)) return;
        if (SLang_pop_slstring(&name)) return;

            entry.name=name;
            entry.text=str;

        }

        if(type == ConsoleInterface::OPT_LIST)
        {
                SLang_pop_array(&arrval,0);
                entry.variants.clear();
                for(unsigned int i=0;i<arrval->num_elements;i++)
                {
                    entry.variants.append(((char**)(arrval->data))[i]);
                }

                SLang_pop_slstring(&str);
                SLang_pop_slstring(&name);

                entry.name=name;
                entry.text=str;
                entry.valueInt=0;
        }

        optList.append(entry);

    }

}

void SLangWrapper::instr_optiondialog ()
{
    console->optionDialog(&optList);
}

int SLangWrapper::instr_DIOOpen()
{
    checkArgCount0(0);
    return dioPort->open();
}

void SLangWrapper::instr_DIOClose()
{
    checkArgCount(0);
    dioPort->close();
}

void SLangWrapper::instr_DIOSetPort(int * port,int * val)
{
    checkArgCount(2);
    dioPort->setPortValue(*port,*val);
}

int SLangWrapper::instr_DIOGetPort(int * port)
{
    checkArgCount0(1);
    return dioPort->getPortValue(*port);
}

void SLangWrapper::instr_DIOSetPinMode (int * port, int * pin, int *mode)
{
    checkArgCount(2);
    if(*mode == SLangWrapperConst::iDIO_IN) dioPort->setPinMode(*port,*pin,DirectIOInterface::IO_MODE_INPUT);
    else  dioPort->setPinMode(*port,*pin,DirectIOInterface::IO_MODE_OUTPUT);
}

void SLangWrapper::instr_DIOSetPortMode (int * port, int *mode)
{
    checkArgCount(2);
    if(*mode == SLangWrapperConst::iDIO_IN) dioPort->setPortMode(*port,DirectIOInterface::IO_MODE_INPUT);
    else  dioPort->setPortMode(*port,DirectIOInterface::IO_MODE_OUTPUT);
}

int SLangWrapper::instr_USBOpen (int * vid,int * pid)
{
    return usbPort->open(*vid,*pid);
}

void SLangWrapper::instr_USBClose ()
{
    usbPort->close();
}

void SLangWrapper::instr_USBReset ()
{
    usbPort->reset();
}

void SLangWrapper::instr_USBBulkWrite()
{
    int num = SLang_Num_Function_Args;

    SLang_Array_Type * data;
    int endpoint = 0; // endpoint по умолчанию
    int timeout = 0; // таймаут по умолчанию


    if(num<1 || num>4)
    {
        SLang_set_error(SL_NumArgs_Error);
        return;
    }

    SLang_pop_array(&data,0);

    if(num>1) SLang_pop_int(&endpoint);

    if(num>2) SLang_pop_int(&timeout);

    usbPort->bulk_write(byteArrayFromArray(data),endpoint,timeout);

}

void SLangWrapper::instr_USBBulkRead()
{
    int num = SLang_Num_Function_Args;

    int len;
    int endpoint = 0; // endpoint по умолчанию
    int timeout = 0; // таймаут по умолчанию


    if(num<1 || num>4)
    {
        SLang_set_error(SL_NumArgs_Error);
        return;
    }

    SLang_pop_int(&len);

    if(num>1) SLang_pop_int(&endpoint);

    if(num>2) SLang_pop_int(&timeout);


    SLang_push_array(arrayFromByteArray(usbPort->bulk_read(len,endpoint,timeout)),true);
}

void SLangWrapper::instr_USBControlMsg ()
{
    int num = SLang_Num_Function_Args;

    int len = 0; // длинна ответа
    int iface = 0; // endpoint по умолчанию
    int cmsgtype;
    int cmsgrequest;

    if(num<2 || num>4)
    {
        SLang_set_error(SL_NumArgs_Error);
        return;
    }

    SLang_pop_int(&cmsgtype);
    SLang_pop_int(&cmsgrequest);

    if(num>2) SLang_pop_int(&iface);

    if(num>3) SLang_pop_int(&len);


    QByteArray data = usbPort->control_msg(cmsgtype,cmsgrequest,iface,len);

    if(num>3)SLang_push_array(arrayFromByteArray(data),true);
}


void SLangWrapper::instr_SetTimeout (int * e)
{
    comPort->setTimeout(*e);
}
