#!../../bin/linux-x86_64/testDev

## You may have to change testDev to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/testDev.dbd"
testDev_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadTemplate "db/userHost.substitutions"
#dbLoadRecords "db/dbSubExample.db", "user=lucasHost"
dbLoadRecords("db/devTest.db","PORT=port1, TIMEOUT=5000")

#drvAsynIPPortConfigure("port1", "localhost:6791", 0, 0, 0);

drvAsynIPPortConfigure("test", "localhost:6791", 0, 0, 0);
portConnectConfigure("port1", "test", 0)


## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncExample, "user=lucasHost"
