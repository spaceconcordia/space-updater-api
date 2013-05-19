//******************************************/
//  JAMG @ SPACE CONCORDIA 2013
//
//
//******************************************/
#ifndef UPDATER_API
#define UPDATER_API
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
/*
    Connect()
        - Connects to the Updater-API
        - Returns : 0 if success and -1 if an error has occurred.

    Rollback(string applicationName)
        - Ask Updater-Host to rollback 'applicationName'.
        - Returns : 0 on success, -1 on failure.

*/
using namespace std;

int NamedSocket(const char *name);

class UpdaterClient{
    private :
        static const char* SERVER_NAME; 
        static const int TIMEOUT_DELAY = 4;
        static const int MAX_RETRY = 5;
        
        const char* socketName;
        
        int sockfd;
        struct sockaddr_un host_addr;
        struct sockaddr_un client_addr;

    public :
        UpdaterClient(const char*);
        int Connect();
        int Disconnect();
        int Rollback(string applicationName);
};

#endif
