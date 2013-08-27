TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
# Build the IOC support library

LIBRARY_IOC += testDevSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
testDevSupport_SRCS += xxxRecord.c
testDevSupport_SRCS += devXxxSoft.c
testDevSupport_SRCS += analogdevsllp.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
testDevSupport_SRCS += dbSubExample.c
testDevSupport_SRCS += testDevHello.c
testDevSupport_SRCS += initTrace.c


testDevSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = testDev
# testDev.dbd will be created and installed
DBD += testDev.dbd

# testDev.dbd will be made up from these files:
testDev_DBD += base.dbd
testDev_DBD += xxxSupport.dbd
testDev_DBD += dbSubExample.dbd
testDev_DBD += testDevHello.dbd
testDev_DBD += initTrace.dbd
testDev_DBD += analogdevsllp.dbd
testDev_DBD += connections.dbd
testDev_DBD += portInclude.dbd

# testDev_registerRecordDeviceDriver.cpp derives from testDev.dbd
testDev_SRCS += testDev_registerRecordDeviceDriver.cpp
testDev_SRCS += sllp_client.c
testDev_SRCS += analogdevsllp.c
testDev_SRCS += PortConnect.cpp

# Build the main IOC entry point on workstation OSs.
testDev_SRCS_DEFAULT += testDevMain.cpp
testDev_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#testDev_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
testDev_LIBS += testDevSupport

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into testDevSupport
    sncExample_SNCFLAGS += +r
    testDev_DBD += sncExample.dbd
    testDevSupport_SRCS += sncExample.stt
    testDevSupport_LIBS += seq pv
    testDev_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
testDev_LIBS += $(EPICS_BASE_IOC_LIBS)
testDev_LIBS += asyn

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

