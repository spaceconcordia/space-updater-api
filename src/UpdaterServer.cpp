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
int main(){
    const char* SERVER_NAME = "Updater-Host";
    const int BUFFER_MAX = 100;
    
    char buffer[BUFFER_MAX];
    int recv_length;
    int yes = 1;
    int sockfd;
    int new_sockfd;
    socklen_t client_length;
    struct sockaddr_un client_addr;
    struct sockaddr_un   host_addr;
    


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
    while(true){
        client_length = sizeof(struct sockaddr);
        if ((new_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_length)) == -1){
            perror("accepting");
            exit(EXIT_FAILURE);
        }
        printf("UpdaterHost : got connection from : %s\n", client_addr.sun_path);

        recv_length = recv(new_sockfd, &buffer, BUFFER_MAX, 0);
        while (recv_length > 0){
            printf("recv : %d bytes\n", recv_length);
            printf("%s\n", buffer);
            recv_length = recv(new_sockfd, &buffer, BUFFER_MAX, 0);
            
            //  here
            const char* path_new      = "/home/jamg85/apps/new";
            const char* path_current  = "/home/jamg85/apps/current";
            const char* path_old      = "/home/jamg85/apps/old";
            const char* path_rollback = "/home/jamg85/apps/rollback";
            Updater* updater = new Updater(path_new, path_current, path_old, path_rollback);

            if (updater->StartRollback(buffer) == true){
                printf("Rollback success : %s\n", buffer);
            }else{
                printf("Rollback failure : %s\n", buffer);
            }
            delete updater;
            //
        }
        close(new_sockfd); 

    }

    close(sockfd);
    //*/
}
