//******************************************/
//  JAMG @ SPACE CONCORDIA 2013
//
//
//******************************************/
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <Updater.h>
#include <cstring>

int main(int argc, char* argv[]){
    const char* SERVER_NAME = "Updater-Host";
    const int BUFFER_MAX = 100;
    const int MAX_RETRY = 5;
    
    char buffer[BUFFER_MAX];
    const char* rollback_success = "SUCCESS";
    const char* rollback_failure = "FAILURE";
    int recv_length;
    int yes = 1;
    int sockfd;
    int new_sockfd;
    socklen_t client_length;
    struct sockaddr_un client_addr;
    struct sockaddr_un   host_addr;

    const char* path_new;      
    const char* path_current;
    const char* path_old;
    const char* path_rollback;

    if (argc < 5){
    #ifdef allPC
        path_new      = "/home/jamg85/apps/new";
        path_current  = "/home/jamg85/apps/current";
        path_old      = "/home/jamg85/apps/old";
        path_rollback = "/home/jamg85/apps/rollback";
    #endif
    #ifndef allPC                                           // Q6 Paths
        path_new      = "/home/apps/new";
        path_current  = "/home/apps/current";
        path_old      = "/home/apps/old";
        path_rollback = "/home/apps/rollback";
    #endif
    }else if (argc == 5){
        path_new      = argv[1]; 
        path_current  = argv[2]; 
        path_old      = argv[3];
        path_rollback = argv[4]; 
    }

    

    remove(SERVER_NAME);                                                            //Remove socket file to avoid "Already in use" error
    printf("Launching UpdaterServer\n\n");

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror ("error creating socket");   
        exit(EXIT_FAILURE);
    }
    
    bzero((char *)&host_addr, sizeof(host_addr));
    host_addr.sun_family = AF_UNIX;
    strcpy(host_addr.sun_path, SERVER_NAME);

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        perror("error setting SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    if (bind(sockfd, (struct sockaddr*)&host_addr, sizeof(struct sockaddr)) == -1){
        perror ("error binding to socket");
        exit(EXIT_FAILURE);
    }
    if (listen(sockfd, 5) == -1){
        perror("error listening on socket");
        exit(EXIT_FAILURE);
    }

    // Daemon
    while(true){
        client_length = sizeof(struct sockaddr);
        if ((new_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_length)) == -1){
            perror("accepting");
            exit(EXIT_FAILURE);
        }
        printf("UpdaterHost : got connection from : %s\n", client_addr.sun_path);
         
        recv_length = recv(new_sockfd, &buffer, BUFFER_MAX, 0);
//       printf("recv : %d bytes\n", recv_length);
//       printf("%s\n", buffer);

        //*******************************************//
        #ifdef TEST
        if (strncmp(buffer, "TIMEOUT", 7) == 0 ){
            while(1);
        }
        #endif
        //*******************************************//

        int retry = 0; 
        bool isSuccess = false;
        while (isSuccess == false && retry < MAX_RETRY){
            Updater* updater = new Updater(path_new, path_current, path_old, path_rollback);

            if (retry > 0){
                printf("retry : %d\n", retry);
            }

            if (updater->StartRollback(buffer) == true){
                printf("Rollback success : %s, status : %s\n", buffer, rollback_success);
                if (send(new_sockfd, rollback_success, strlen(rollback_success)+1, 0) == -1){
                    perror ("error sending data");   
                    exit(EXIT_FAILURE);
                }

                isSuccess = true;           
            }else{
                printf("Rollback failure : %s, status : %s\n", buffer, rollback_failure);
                if (send(new_sockfd, rollback_failure, strlen(rollback_failure)+1, 0) == -1){
                    perror ("error sending data");   
                    exit(EXIT_FAILURE);
                }
            }

            delete updater;
            retry += 1;
        }

        printf ("Closing socket\n\n");
       close(new_sockfd); 
    }// daemon

    close(sockfd);
}//main
