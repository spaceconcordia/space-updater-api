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
INCLUDE = -I$(UPDATER_API_PATH)/include -I$(UPDATER_PATH)

allPC : Client UpdaterServer

fileIO.o: $(UPDATER_PATH)/fileIO.cpp $(UPDATER_PATH)/fileIO.h
	$(CXX) -c $< -o $@

ProcessUpdater.o : $(UPDATER_PATH)/ProcessUpdater.cpp $(UPDATER_PATH)/ProcessUpdater.h $(UPDATER_PATH)/fileIO.h
	$(CXX) -c $< -o $@
		
Updater.o : $(UPDATER_PATH)/Updater.cpp $(UPDATER_PATH)/Updater.h  $(UPDATER_PATH)/ProcessUpdater.h $(UPDATER_PATH)/fileIO.h
	$(CXX) -c $< -o $@
	
UpdaterServer : src/UpdaterServer.cpp Updater.o fileIO.o ProcessUpdater.o
	$(CXX) $(INCLUDE) $^ -o $@
	
UpdaterClient.o : src/UpdaterClient.cpp include/UpdaterClient.h
	$(CXX) $(INCLUDE) -c $< -o $@

Client : src/Client.cpp UpdaterClient.o
	$(CXX) $(INCLUDE) $^ -o $@
	
clean :
	rm -f *.o
