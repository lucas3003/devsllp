#include <iocsh.h>
#include <epicsExport.h>
#include "PortConnect.h"
#include <stdio.h>

PortConnect::PortConnect(const char* portName, const char * serialName, int address) : asynPortDriver(portName,0,6, asynInt32Mask | asynOctetMask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask | asynDrvUserMask, 0, 0, 1, 0, 0)
{
	printf("new instance!\n");
	asynStatus status = pasynOctetSyncIO->connect(serialName, 0, &user, NULL);
	if(status) printf("Error: connect to port\n");
	
	addr = address;

	pasynOctetSyncIO->flush(user);
}

asynStatus PortConnect :: readFloat64(asynUser* pasynUser, epicsFloat64* value)
{
	printf("ReadFloat64!\n");
	return asynSuccess;
}

asynStatus PortConnect :: writeFloat64(asynUser * pasynUser, epicsFloat64 value)
{
	printf("writeFloat64!\n");	
	return asynSuccess;
}

asynStatus PortConnect :: readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason)
{
	printf("readOctet!\n");
	return asynSuccess;
}

asynStatus PortConnect :: writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual)
{
	printf("writeOctet!\n");
	return asynSuccess;
}

extern "C"
{

	int portConnectConfigure(const char * portName, const char * serialName, int address)
	{
		new PortConnect(portName, serialName, address);
		return(asynSuccess);
	}

	/* EPICS iocsh shell */

	static const iocshArg initArg0 = {"portName", iocshArgString};
	static const iocshArg initArg1 = {"serialName", iocshArgString};
	static const iocshArg initArg2 = {"address", iocshArgString};
	static const iocshArg * const initArgs [] = {&initArg0, &initArg1, &initArg2};
	static const iocshFuncDef initFuncDef = {"portConnectConfigure", 3, initArgs};

	static void initCallFunc(const iocshArgBuf *args)
	{
		portConnectConfigure(args[0].sval, args[1].sval, args[2].ival);
	}

	void portConnectRegister(void)
	{
		iocshRegister(&initFuncDef, initCallFunc);
	}

	epicsExportRegistrar(portConnectRegister);
}