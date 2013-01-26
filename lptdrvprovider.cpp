///////////////////////////////////////////////////////////////////////
//HardwareExpert. LptDrv library provider.
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

#include "lptdrvprovider.h"

LptDrvProvider::LptDrvProvider()
{
    lib.setFileName("lptdrv.dll");
}


bool LptDrvProvider::load()
{
    lib.load();
    if(!lib.isLoaded()) return false;

    LptDrvCheckVersion  = (PrototypeUintStr) lib.resolve("LptDrvCheckVersion");
    LptDrvClearError  = (PrototypeUintVoid) lib.resolve("LptDrvClearError");
    LptDrvClose  = (PrototypeUintVoid) lib.resolve("LptDrvClose");
    LptDrvGetDllVersion  = (PrototypeUintVoid) lib.resolve("LptDrvGetDllVersion");
    LptDrvGetError  = (PrototypeUintVoid) lib.resolve("LptDrvGetError");
    LptDrvGetErrorString = (PrototypeUintUint) lib.resolve("LptDrvGetErrorString");
    LptDrvGetNumPorts  = (PrototypeUintVoid) lib.resolve("LptDrvGetNumPorts");
    LptDrvGetOpenedPort  = (PrototypeUintVoid) lib.resolve("LptDrvGetOpenedPort");
    LptDrvGetStatus  = (PrototypeUintVoid) lib.resolve("LptDrvGetStatus");
    LptDrvGetVersion  = (PrototypeUintVoid) lib.resolve("LptDrvGetVersion");

    //LptDrvGetVersionString is buggy & not need
    LptDrvGetVersionString = (PrototypeUintStrDword) lib.resolve("LptDrvGetVersionString");

    LptDrvOpen = (PrototypeUintUintDword) lib.resolve("LptDrvOpen");
    LptDrvReset = (PrototypeUintVoid) lib.resolve("LptDrvReset");
    LptDrvSetControl = (PrototypeUintDword) lib.resolve("LptDrvSetControl");
    LptDrvSetData = (PrototypeUintDword) lib.resolve("LptDrvSetData");
    LptDrvSetDelay = (PrototypeUintDword) lib.resolve("LptDrvSetDelay");
    LptDrvSetTimeout = (PrototypeUintDword) lib.resolve("LptDrvSetTimeout");


    LptI2cCheckAdapter = (PrototypeUintVoid) lib.resolve("LptI2cCheckAdapter");
    LptI2cEnableAckCheck = (PrototypeUintDword) lib.resolve("LptI2cEnableAckCheck");
    LptI2cOpen = (PrototypeUintDwordDword) lib.resolve("LptI2cOpen");
    LptI2cRead = (PrototypeUintDwordDwordDwordDwordDword) lib.resolve("LptI2cRead");
    LptI2cReadByte = (PrototypeUintDwordDword) lib.resolve("LptI2cReadByte");
    LptI2cReadSubByte = (PrototypeUintDwordDwordDword) lib.resolve("LptI2cReadSubByte");
    LptI2cReadSubWord = (PrototypeUintDwordDwordDword) lib.resolve("LptI2cReadSubWord");
    LptI2cReadWord = (PrototypeUintDwordDword) lib.resolve("LptI2cReadWord");
    LptI2cWrite = (PrototypeUintDwordDwordDwordDwordDword) lib.resolve("LptI2cWrite");
    LptI2cWriteByte = (PrototypeUintDwordDword) lib.resolve("LptI2cWriteByte");
    LptI2cWriteSubByte = (PrototypeUintDwordDwordDword) lib.resolve("LptI2cWriteSubByte");
    LptI2cWriteSubWord = (PrototypeUintDwordDwordDword) lib.resolve("LptI2cWriteSubWord");
    LptI2cWriteWord = (PrototypeUintDwordDword) lib.resolve("LptI2cWriteWord");

    LptSpiSetInvOut = (PrototypeUintDword) lib.resolve("LptSpiSetInvOut");
    LptSpiWrite = (PrototypeUintDwordDwordDword ) lib.resolve("LptSpiWrite");
    LptSpiWriteWord = (PrototypeUintDword) lib.resolve("PrototypeUintDword ");

    return true;
}
