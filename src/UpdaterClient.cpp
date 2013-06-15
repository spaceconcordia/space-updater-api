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
    remove(socketName);                                                 // Remove socket file to avoid "Already in use" error
    
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror ("error creating socket");   
        return -1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        perror("error setting SO_REUSEADDR");
        return -1;
    }

    struct timeval tv;
    tv.tv_sec = TIMEOUT_DELAY;
    tv.tv_usec = 0;
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *) &tv, sizeof(struct timeval)) == -1){
        perror ("error setting SO_RCVTIMEO");
        return -1;
    }

    if (bind(sockfd, (struct sockaddr*)&client_addr, sizeof(struct sockaddr)) == -1){
        perror ("error binding to socket");
        return -1;
    }   

    if (connect(sockfd, (struct sockaddr *) &host_addr, sizeof(struct sockaddr)) < 0){
        perror ("error connecting to socket");   
        return -1;
    }

    printf("Client connection success : %s\n", socketName);
    return 0;
}
//----------------------------------------------
//  Disconnect
//----------------------------------------------
int UpdaterClient::Disconnect(){
    printf ("Closing : %s\n", socketName);
    close(sockfd);
    remove(socketName);
}
//----------------------------------------------
//  Rollback
//----------------------------------------------
int UpdaterClient::Rollback(string applicationName){
    bool isSuccess = false;
    int BUFFER_MAX = 100;
    char report[BUFFER_MAX];
    char buffer[applicationName.length() + 1];
    strcpy (buffer, applicationName.c_str());
    printf("To Rollback : %s\n", buffer);

    if(send(sockfd, buffer, strlen(buffer)+1, 0) == -1){
        perror ("error sending data");   
        exit(EXIT_FAILURE);
    }
    
    int retry  = 0;
    while (isSuccess == false && retry < MAX_RETRY){
        if (retry > 0){
            printf ("retry : %d\n", retry);
        }

        int recv_length = recv(sockfd, &report, 8, 0); 
//        printf("recv : %d bytes\n", recv_length);
//        printf("report : %s\n", report);
        if (strncmp(report, "SUCCESS", 7) == 0){
            isSuccess = true;
        }
        if (recv_length < 0){
            printf("TimeOut... ");
        }

        retry += 1;
    }
    if (isSuccess == true){
        printf("Rollback SUCCESS\n");
        return 0;
    }else{
        printf("Rollback FAILURE\n");
        return -1;
    }
}
