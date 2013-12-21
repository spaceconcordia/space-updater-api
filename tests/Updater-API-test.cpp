//******************************************/
//  JAMG @ SPACE CONCORDIA 2013
//
//
//******************************************/
#include "CppUTest/TestHarness.h"
#include <sys/stat.h>
#include <unistd.h>     // rmdir()
#include <dirent.h>     // DIR
#include <stdlib.h>
#include <stdio.h>
#include <UpdaterClient.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fileIO.h>
//************************************************************
//************************************************************
//              UpdaterAPITestGroup
//************************************************************
//************************************************************
TEST_GROUP(UpdaterAPITestGroup){
    int status;
    pid_t pid; 
    
    void setup(){
        mkdir("test-new", S_IRWXU);
        mkdir("test-current", S_IRWXU);
        mkdir("test-old", S_IRWXU);
        mkdir("test-rollback", S_IRWXU);
        mkdir("test-logs", S_IRWXU);

        pid = fork();                                                                       // returns pid of the child
        if (pid == 0){                                                                      // in child
            if(execl("UpdaterServer", "UpdaterServer", "test-new", "test-current", 
                                                       "test-old", "test-rollback", "test-logs", NULL) == -1){
                perror("error execl in CHILD");   
            }
        }else if (pid > 0){                                                                 // in parent
            usleep(100000);                                                                 // Wait (100 ms) for the daemon to start (in microsecond)
        }else{ 
            perror ("fork failed.");
            exit(1);
        }
    }
    void teardown(){
        kill(pid, 9);
        DeleteDirectoryContent("test-new");
        DeleteDirectoryContent("test-current");
        DeleteDirectoryContent("test-old");
        DeleteDirectoryContent("test-rollback");
        DeleteDirectoryContent("test-logs");
        rmdir("test-new");                                                                  // rmdir() : the directory HAS to be empty.
        rmdir("test-old");
        rmdir("test-current");
        rmdir("test-rollback");
        rmdir("test-logs");
    }

};
TEST(UpdaterAPITestGroup, Connection_Returns_0){                        // UPDTRAPI-01
    UpdaterClient client("testApp1");  
    CHECK(client.Connect() == 0); 
    client.Disconnect();
}
TEST(UpdaterAPITestGroup, Connection_Daemon_off_Returns_minus_1){       // UPDTRAPI-02
    kill(pid,9);
    usleep(100000);
    UpdaterClient client("testApp2");
    CHECK(client.Connect() == -1); 
    client.Disconnect();
}
TEST(UpdaterAPITestGroup, Connection_Rollback_Returns_0){               // UPDTRAPI-03
    mkdir("test-current/DirToRollBack", S_IRWXU);
    mkdir("test-old/DirToRollBack", S_IRWXU);
    FILE* file = fopen("test-old/DirToRollBack/file.txt", "w+");
    fprintf(file,"some text to test");
    fclose(file);
    
    UpdaterClient client("testApp3");
    CHECK(client.Connect() == 0);
    CHECK(client.Rollback("DirToRollBack") == 0);
    
    file = fopen("test-current/DirToRollBack/file.txt", "r");
    CHECK(file != NULL);
    fclose(file);

    file = fopen("test-old/DirToRollBack/file.txt", "r");
    CHECK(file != NULL);
    fclose(file);
    
    client.Disconnect();
}

TEST(UpdaterAPITestGroup, Connection_Rollback_nonExistingApp_Returns_minus_1){      //  UPDTRAPI-05
    UpdaterClient client("testApp4");
    CHECK(client.Connect() == 0);
    CHECK(client.Rollback("DirToRollBack") == -1);
    client.Disconnect();
}
TEST(UpdaterAPITestGroup, DeamonCrash_API_timeOut_Returns_minus_1){     // UPDTRAPI-06
    UpdaterClient client("testApp5");
    CHECK(client.Connect() == 0);
    CHECK(client.Rollback("TIMEOUT") == -1);
    client.Disconnect();
}
