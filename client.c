#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "utile.h"

int main(int argc, char **argv) {

     // verification du nombre d'arguments sur la ligne de commande
    if (argc != 3) {
        printf("Usage : %s @dest num_port \n", argv[0]);
        exit(-1);
    }

    int sockfd;              // descripteur de socket
    struct sockaddr_in dest; // structure d'adresse qui contiendra les
                             // parametres reseaux du destinataire
    char buf2[1024];
    char buf[1024];
    fd_set readfds;
    fd_set readfds2;
    FD_ZERO(&readfds);
    // creation de la socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);


    // initialisation de la structure d'adresse du destinataire :

    // famille d'adresse
    dest.sin_family = AF_INET;
    // adresse IPv4 du destinataire
    inet_aton(argv[1], &(dest.sin_addr));
    // port du destinataire
    dest.sin_port = htons(atoi(argv[2]));
    // demande une connexion
    connect(sockfd, (void *)&dest, sizeof(struct sockaddr_in));

    FD_SET(sockfd, &readfds);
    FD_SET(0, &readfds);

    // demande à l'utilisateur de rentrer son pseudonyme 
    memset(buf2, '\0', 1024);
    printf("user name : \n");
    read(0, buf2, sizeof(buf2));
    if (send(sockfd, buf2, strlen(buf2) -1, 0) == -1) {
        perror("erreur a l'appel de la fonction send -> ");
        exit(-2);
    }


    // envoi de la chaine
    while (1) {
        // mise à zero des buffers
        memset(buf, '\0', 1024);
        memset(buf2, '\0', 1024);
        readfds2 = readfds;

        select(sockfd + 1, &readfds2, 0, 0, 0);

        // reception de la chaine de caracteres
        if (FD_ISSET(sockfd, &readfds2)) {
            if (recv(sockfd, buf, 1024, 0) == -1) {
                perror("erreur de reception -> ");
                exit(-3);
            }

            // Se deconnecte si le server le lui demande
            // Cette demande est une reponse affirmative à la demande de 
            // deconnexion du client au serveur
            if (strncmp(buf, "/quit", 5) == 0) {
                close(sockfd);
                exit(EXIT_SUCCESS);
            }
            // affichage de la chaine de caracteres recue
            printf("%s", buf);
            fflush(stdout);
        }

        if (FD_ISSET(0, &readfds2)) {
            read(0, buf2, sizeof(buf2));
            // demande au serveur de se deconnecter
            if (strncmp(buf2, "/quit", 5) == 0) {
                if (send(sockfd, buf2, strlen(buf2) + 1, 0) == -1) {
                    perror("erreur a l'appel de la fonction send -> ");
                    exit(-2);
                }
            } else {    // message autre que /quit
                if (send(sockfd, buf2, strlen(buf2) + 1, 0) == -1) {
                    perror("erreur a l'appel de la fonction send -> ");
                    exit(-2);
                }
            }
        }
    }
    // fermeture de la socket
    close(sockfd);
    return 0;
}







