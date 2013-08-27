#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <callback.h>
#include <recGbl.h>
#include <dbAccess.h>
#include <dbDefs.h>
#include <dbStaticLib.h>
#include <link.h>
#include <epicsPrint.h>
#include <epicsMutex.h>
#include <epicsThread.h>
#include <cantProceed.h>
#include <dbCommon.h>
#include <dbScan.h>
#include <aoRecord.h>
#include <aiRecord.h>
#include <recSup.h>
#include <devSup.h>
#include <epicsExport.h>
#include "asynDriver.h"
#include "asynDrvUser.h"
#include "asynFloat64SyncIO.h"
#include "asynEpicsUtils.h"
#include "asynFloat64.h"
#include "asynOctet.h"
#include "sllp_client.h"
#include "asynOctetSyncIO.h"
#include <epicsExport.h>

#define TIMEOUT 5000
#define SERIAL

typedef struct analogDset{
	long number;
	DEVSUPFUN dev_report;
	DEVSUPFUN init;
	DEVSUPFUN init_record;
	DEVSUPFUN get_ioint_info;
	DEVSUPFUN process;
	DEVSUPFUN special_linconv;
} analogDset;

typedef struct pvtBuffer{
	epicsFloat64 value;
	asynStatus status;
} pvtBuffer;

typedef struct devPvt{
	dbCommon *pr;
	asynUser *pasynUser;
	asynUser *pasynUserSync;
	asynFloat64 *pfloat64;
	void *float64Pvt;
	asynOctet *pasynoctet;
	void *octetPvt;
	int canblock;
	//pvtBuffer *buffer;
	pvtBuffer result;
	char *portName;
	char *userParam;
	int addr;
    //sllp_client_t *sllp;
    //struct sllp_vars_list *vars;	
} devPvt;

uint8_t* buffer;
size_t size;

static long initCommon(dbCommon *pr, DBLINK *plink, userCallback processCallback);
static long initAi(aiRecord *pai);
static long initAo(aoRecord *pao);
static void processCallbackInput(asynUser *pasynUser);
static void processCallbackOutput(asynUser *pasynUser);
int sendSerial(uint8_t *data, uint32_t *count);
int sendTCP(uint8_t *data, uint32_t *count);
int receiveEPICS(uint8_t *data, uint32_t *count);
static long processAi(aiRecord *pr);
static long processAo(aoRecord *pr);


analogDset sllpAi = { 6,0,0, initAi, 0, processAi, 0};
analogDset sllpAo = { 6,0,0, initAo, 0, processAo, 0};

epicsExportAddress(dset, sllpAi);
epicsExportAddress(dset, sllpAo);

static long initCommon(dbCommon *pr, DBLINK *plink, userCallback processCallback)
{	
	printf("initCommon\n");
	devPvt *pPvt;
	asynStatus status;
	asynUser *pasynUser;
	asynInterface *pasynInterface;

	pPvt = callocMustSucceed(1, sizeof(*pPvt), "analogdevsllp::initCommon");
	pr->dpvt = pPvt;		
	pPvt->pr = pr;

	pasynUser = pasynManager->createAsynUser(processCallback, 0);	
	pasynUser->userPvt = pPvt;
	pPvt->pasynUser = pasynUser;

	status = pasynEpicsUtils->parseLink(pasynUser, plink, &pPvt->portName, &pPvt->addr, &pPvt->userParam);
	if(status != asynSuccess) goto fail;

	status = pasynManager->connectDevice(pasynUser, pPvt->portName, pPvt->addr);
	if(status != asynSuccess) goto fail;	

	status = pasynManager->canBlock(pPvt->pasynUser, &pPvt->canblock);
	if(status != asynSuccess) goto fail;	

	pasynInterface = pasynManager->findInterface(pasynUser, asynDrvUserType, 1);	
	/*if(pasynInterface && pPvt->userParam)
	{
		asynDrvUser *pasynDrvUser;
		void *drvPvt;

		pasynDrvUser = (asynDrvUser *) pasynInterface->pinterface;		
		drvPvt = pasynInterface->drvPvt;		
		status = pasynDrvUser->create(drvPvt, pasynUser, pPvt->userParam,0,0);		

		if(status != asynSuccess) goto fail;
	}*/

	printf("UserParam = %s, id= %d\n", pPvt->userParam, pPvt->addr);
	
	//pasynInterface = pasynManager->findInterface(pasynUser, asynFloat64Type, 1);
	//if(!pasynInterface) goto fail;

	pasynInterface = pasynManager->findInterface(pasynUser, asynOctetType, 1);
	if(!pasynInterface) goto fail;

	pPvt->pasynoctet = pasynInterface->pinterface;
	pPvt->octetPvt = pasynInterface->drvPvt;

	//pPvt->pfloat64 = pasynInterface->pinterface;
	//pPvt->float64Pvt = pasynInterface->drvPvt;

	//status = pasynOctetSyncIO->connect(pPvt->portName, -1, &pPvt->pasynUserSync, pPvt->userParam);

	//scanIoInit(&pPvt->ioScanPvt);


	return 0;

fail:
	printf("fail\n");
	pr->pact=1;
	return -1;
}

static long initAi(aiRecord *pai)
{
	asynStatus status;
	printf("initAi\n");
	status = initCommon((dbCommon *)pai, &pai->inp, processCallbackInput);
	if(status != asynSuccess) return 0;

	return(0);
}

static void processCallbackInput(asynUser *pasynUser)
{
	//if(pr->pact) callbackRequestProcessCallback(&pPvt->callback,pr->prio,pr);
}

static void processCallbackOutput(asynUser *pasynUser)
{
	
	devPvt *pPvt = (devPvt *) pasynUser->userPvt;
	//dbCommon *pr = pPvt->pr;
	//int id = 0; //TODO: Get correct id
	printf("processCallbackOutput begin\n");
	//printf("Count = %d\n", pPvt->vars->count);
	//struct sllp_var_info * var = &pPvt->vars->list[id];
	

	unsigned int raw = (unsigned int) (((double)pPvt->result.value+10)*262143)/20.0;
	uint8_t * buf;
	buf = (uint8_t*) malloc(3*sizeof(uint8_t));
	int i;

	for(i = 2; i >= 0; i--)
	{
		buf[i] = raw & 0xFF;
		raw = raw >> 8;
	}
	
	//sllp_write_var(pPvt->sllp, var, buf);

	size_t writeBytes;
	pPvt->result.status = pPvt->pasynoctet->write(pPvt->octetPvt, pPvt->pasynUser, (char *) buf, size, &writeBytes);

	//if(pr->pact) callbackRequestProcessCallback(&pPvt->callback,pr->prio,pr);

	printf("processCallbackOutput end\n");
}

static long initAo(aoRecord *pao)
{
	//asynStatus status;
	//devPvt *pPvt;	
	//epicsFloat64 value;
	//uint8_t packet[17000];
	//int eomReason;
	//size_t bread;

	//status = initCommon((dbCommon *)pao, &pao->out, processCallbackOutput);
	printf("initAo\n");
	initCommon((dbCommon *)pao, &pao->out, processCallbackOutput);
	
	//pPvt = pao->dpvt;

	//pao->val = 0;
	//pao->udf = 0;

	return 2;
}

/*int receiveEPICS(uint8_t *data, uint32_t *count)
{
	return 0; //Return exit success
}

int sendSerial(uint8_t *data, uint32_t *count)
{
	uint8_t packet[17000];
	uint32_t packet_size = *count+3;
	uint8_t *csum = &packet[packet_size-1];

	*csum = 0;

	uint8_t address = 0x05; //TODO: Get address;

	packet[0] = address;
	packet[1] = 0;

	*csum -= address;

	unsigned int i;
	for(i = 0; i < *count; i++)
	{
		packet[i+2] = data[i];
		*csum -= data[i];
	}

	buffer = (uint8_t*) malloc((packet_size+3)*sizeof(uint8_t));
	memcpy(buffer, packet, packet_size+3);

	size = packet_size+3;

	return 0; //Return EXIT_SUCCESS
}

int sendTCP(uint8_t *data, uint32_t *count)
{
	return 0;
}*/

static long processAi(aiRecord *pr)
{
	return 0;
}

static long processAo(aoRecord *pr)
{
	devPvt *pPvt = (devPvt *) pr->dpvt;
	asynStatus status;

	pPvt->result.value = pr->oval;
	if(pPvt->canblock) pr->pact = 1;
	status = pasynManager->queueRequest(pPvt->pasynUser, 0, 0);

	printf("processAo begin\n");

	if((status == asynSuccess) && pPvt->canblock) return 0;		
	if(pPvt->canblock) pr->pact = 0;
	if(status != asynSuccess) pPvt->result.status = status;

	printf("processAo end\n");

	return 0;
}