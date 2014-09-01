CXX = g++
MICROCC=microblazeel-xilinx-linux-gnu-g++
BB = arm-linux-gnueabi-g++
CPPUTEST_HOME = ../CppUTest
UPDATER_PATH  = ../space-updater
UTLS_DIR = 	../space-lib/utls
UPDATER_API_PATH = ../space-updater-api
SPACE_LIB = ../space-lib

CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall

INCLUDE = -I$(UPDATER_API_PATH)/include -I$(UPDATER_PATH)/include -I$(SPACE_LIB)/include -I$(SPACE_LIB)/utls/include -I$(SPACE_LIB)/shakespeare/inc
LIB_PATH =  -L$(SPACE_LIB)/shakespeare/lib -L$(SPACE_LIB)/utls/lib

#
# conveniently create the bin directory
#
make_dir:
	mkdir -p bin 

#
#	Compilation for PC
#
LIBS = -lshakespeare -lcs1_utls

buildBin : make_dir bin/fileIO.o bin/ProcessUpdater.o bin/Updater.o bin/UpdaterClient.o bin/UpdaterServer bin/Client

#
# 	Compilation for CppUTest
#

test : make_dir bin/fileIO.o bin/ProcessUpdater.o bin/Updater.o bin/UpdaterClient.o bin/AllTests bin/UpdaterServer

bin/AllTests: src/AllTests.cpp tests/Updater-API-test.cpp bin/fileIO.o bin/ProcessUpdater.o bin/Updater.o bin/UpdaterClient.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) -o $@ $^ $(LD_LIBRARIES) $(LIB_PATH) $(LIBS)

bin/fileIO.o: $(UPDATER_PATH)/src/fileIO.cpp $(UPDATER_PATH)/include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@

bin/ProcessUpdater.o : $(UPDATER_PATH)/src/ProcessUpdater.cpp $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@

bin/Updater.o : $(UPDATER_PATH)/src/Updater.cpp $(UPDATER_PATH)/include/Updater.h  $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@

bin/UpdaterServer : src/UpdaterServer.cpp bin/Updater.o bin/fileIO.o bin/ProcessUpdater.o
	$(CXX) $(INCLUDE) $^ -DTEST -DallPC -o $@ $(LIB_PATH) -lshakespeare $(LIBS)

bin/UpdaterClient.o : src/UpdaterClient.cpp include/UpdaterClient.h
	$(CXX) $(INCLUDE) -c $< -o $@

bin/Client : src/Client.cpp ./bin/UpdaterClient.o
	$(CXX) $(INCLUDE) $^ -o $@

#
#	Compilation for the Q6. Microblaze.
#
#
LIBS_Q6 =  -lshakespeare-mbcc -lcs1_utlsQ6

buildQ6 : make_dir fileIO-Q6.o ProcessUpdater-Q6.o Updater-Q6.o UpdaterClient-Q6.o UpdaterServer-Q6 Client-Q6

fileIO-Q6.o: $(UPDATER_PATH)/src/fileIO.cpp $(UPDATER_PATH)/include/fileIO.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) -c $< -o ./bin/$@ $(LIBS_Q6)

ProcessUpdater-Q6.o :  $(UPDATER_PATH)/src/ProcessUpdater.cpp $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) $(LIB_PATH) -c $< -o ./bin/$@ $(LIBS_Q6)

Updater-Q6.o :$(UPDATER_PATH)/src/Updater.cpp $(UPDATER_PATH)/include/Updater.h  $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) $(LIB_PATH) -c $< -o ./bin/$@ $(LIBS_Q6)

UpdaterServer-Q6 : src/UpdaterServer.cpp ./bin/Updater-Q6.o ./bin/fileIO-Q6.o ./bin/ProcessUpdater-Q6.o
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) $(LIB_PATH) $^ -o ./bin/$@  $(LIBS_Q6) 

UpdaterClient-Q6.o : src/UpdaterClient.cpp include/UpdaterClient.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) $(LIB_PATH) -c $< -o ./bin/$@ $(LIBS_Q6) 

Client-Q6 : src/Client.cpp ./bin/UpdaterClient-Q6.o
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) $(LIB_PATH) $^ -o ./bin/$@ $(LIBS_Q6) 

#
#	Compilation for Beaglebone Black
#

buildBB : make_dir fileIO-bb.o ProcessUpdater-bb.o Updater-bb.o UpdaterClient-bb.o UpdaterServer-bb Client-bb

#
# 	Compilation for CppUTest
#

fileIO-bb.o: $(UPDATER_PATH)/src/fileIO.cpp $(UPDATER_PATH)/include/fileIO.h
	$(BB) $(INCLUDE) -c $< -o ./bin/$@

ProcessUpdater-bb.o : $(UPDATER_PATH)/src/ProcessUpdater.cpp $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(BB) $(INCLUDE) -c $< -o ./bin/$@

Updater-bb.o : $(UPDATER_PATH)/src/Updater.cpp $(UPDATER_PATH)/include/Updater.h  $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(BB) $(INCLUDE) -c $< -o ./bin/$@

UpdaterServer-bb : src/UpdaterServer.cpp ./bin/Updater-bb.o ./bin/fileIO-bb.o ./bin/ProcessUpdater-bb.o
	$(BB) $(INCLUDE) $^ -o ./bin/$@ $(LIB_PATH) -lshakespeare-BB

UpdaterClient-bb.o : src/UpdaterClient.cpp include/UpdaterClient.h
	$(BB) $(INCLUDE) -c $< -o ./bin/$@

Client-bb : src/Client.cpp ./bin/UpdaterClient-bb.o
	$(BB) $(INCLUDE) $^ -o ./bin/$@

clean :
	rm -f *.o *~ ./bin/*.o

cleanAll :
	rm -f *.o *~ ./bin/*.o
	rm -f ./bin/testApp* ./bin/Updater-Host ./bin/UpdaterServer ./bin/AllTests ./bin/Client ./bin/UpdaterServer-Q6 ./bin/Client-Q6
