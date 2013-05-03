//******************************************/
//  JAMG @ SPACE CONCORDIA 2013
//
//
//******************************************/
#include <UpdaterClient.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
const char* UpdaterClient::SERVER_NAME = "Updater-Host";
//----------------------------------------------
//  Constructor
//----------------------------------------------
UpdaterClient::UpdaterClient(const char* socketName){
    bzero((char *)&host_addr, sizeof(host_addr));
    host_addr.sun_family = AF_UNIX;
    strcpy(host_addr.sun_path, SERVER_NAME);
   
    bzero((char*) &client_addr, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, socketName); 

    this->socketName = socketName; 
}
//----------------------------------------------
//  Connect
//----------------------------------------------
int UpdaterClient::Connect(){
    int yes = 1;                                                        // to set SO_REUSEADDR
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror ("error creating socket");   
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        perror("error setting SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    if (bind(sockfd, (struct sockaddr*)&client_addr, sizeof(struct sockaddr)) == -1){
        perror ("error binding to socket");
        exit(EXIT_FAILURE);
    }   
    if (connect(sockfd, (struct sockaddr *) &host_addr, sizeof(struct sockaddr)) < 0){
        perror ("error creating socket");   
        exit(EXIT_FAILURE);
    }

    printf("Client succes\n");
    return 0;
}
//----------------------------------------------
//  Disconnect
//----------------------------------------------
int UpdaterClient::Disconnect(){
    close(sockfd);
}
//----------------------------------------------
//  Rollback
//----------------------------------------------
int UpdaterClient::Rollback(string applicationName){
    char buffer[applicationName.length() + 1];
    strcpy (buffer, applicationName.c_str());
    printf("%s\n", buffer);
    if(send(sockfd, buffer, strlen(buffer)+1, 0) == -1){
        perror ("error creating socket");   
        exit(EXIT_FAILURE);
    }
}
