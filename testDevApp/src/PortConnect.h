#ifndef PORT_CONNECT_H_
#define PORT_CONNECT_H_

#include <asynPortDriver.h>
#include <asynOctetSyncIO.h>

class PortConnect : public asynPortDriver
{
public:
	PortConnect(const char* portName, const char * serialName, int address);

	virtual asynStatus writeFloat64(asynUser* pasynUser, epicsFloat64 value);
	virtual asynStatus readFloat64(asynUser* pasynUser, epicsFloat64* value);
	virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason);
	virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);

private:
	asynUser * user;
	int addr;	
};

#endif