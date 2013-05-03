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
        - Return : 0 if successful and -1 if an error has occurred.



*/
using namespace std;
int NamedSocket(const char *name);

class UpdaterClient{
    private :
        const char* socketName;
        int sockfd;
        struct sockaddr_un host_addr;
        struct sockaddr_un client_addr;
        static const char* SERVER_NAME; 

    public :
        UpdaterClient(const char*);
        int Connect();
        int Disconnect();
        int Rollback(string applicationName);


};

#endif
