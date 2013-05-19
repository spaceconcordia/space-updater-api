CXX = g++
MICRO_BLAZE = microblaze-unknown-linux-gnu-g++
CPPUTEST_HOME = /home/jamg85/CppUTest
UPDATER_PATH  = /home/jamg85/Concordia/Space/Updater
UPDATER_API_PATH = /home/jamg85/Concordia/Space/Updater-API

CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
MICRO_FLAGS = -mcpu=v8.10.a -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -ffixed-r31 --sysroot /usr/local/lib/mbgcc/microblaze-unknown-linux-gnu/sys-root
INCLUDE = -I$(UPDATER_API_PATH)/include -I$(UPDATER_PATH)/include

#
#	Compilation for PC
#

allPC : Client UpdaterServer
	rm -f *.o

#
# 	Compilation for CppUTest
#

test : AllTests UpdaterServer
	rm -f *.o

AllTests: src/AllTests.cpp tests/Updater-API-test.cpp fileIO.o ProcessUpdater.o Updater.o UpdaterClient.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) -o $@ $^ $(LD_LIBRARIES) 


fileIO.o: $(UPDATER_PATH)/src/fileIO.cpp $(UPDATER_PATH)/include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@

ProcessUpdater.o : $(UPDATER_PATH)/src/ProcessUpdater.cpp $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@
		
Updater.o : $(UPDATER_PATH)/src/Updater.cpp $(UPDATER_PATH)/include/Updater.h  $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@
	
UpdaterServer : src/UpdaterServer.cpp Updater.o fileIO.o ProcessUpdater.o
	$(CXX) $(INCLUDE) $^ -o $@ -DTEST -DallPC
	
UpdaterClient.o : src/UpdaterClient.cpp include/UpdaterClient.h
	$(CXX) $(INCLUDE) -c $< -o $@

Client : src/Client.cpp UpdaterClient.o
	$(CXX) $(INCLUDE) $^ -o $@

#
#	Compilation for the Q6. Microblaze.
#
#

Q6 : UpdaterServer-Q6 Client-Q6
	rm -f *.o

fileIO-Q6.o: $(UPDATER_PATH)/src/fileIO.cpp $(UPDATER_PATH)/include/fileIO.h
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) -c $< -o $@

ProcessUpdater-Q6.o :  $(UPDATER_PATH)/src/ProcessUpdater.cpp $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) -c $< -o $@
		
Updater-Q6.o :$(UPDATER_PATH)/src/Updater.cpp $(UPDATER_PATH)/include/Updater.h  $(UPDATER_PATH)/include/ProcessUpdater.h $(UPDATER_PATH)/include/fileIO.h
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) -c $< -o $@

UpdaterServer-Q6 : src/UpdaterServer.cpp Updater-Q6.o fileIO-Q6.o ProcessUpdater-Q6.o
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) $^ -o $@
	rm -f *.o

UpdaterClient-Q6.o : src/UpdaterClient.cpp include/UpdaterClient.h
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) -c $< -o $@

Client-Q6 : src/Client.cpp UpdaterClient-Q6.o
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) $^ -o $@
	rm -f *.o	

clean :
	rm -f *.o *~

cleanAll :
	rm -f *.o *~
	rm -f testApp* Updater-Host UpdaterServer AllTests Client UpdaterServer-Q6 Client-Q6
