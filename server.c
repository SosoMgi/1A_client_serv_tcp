#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "utile.h"
#include <strings.h>

int main(int argc, char **argv) {

    // verification du nombre d'arguments sur la ligne de commande
    if (argc != 2) {
        printf("Usage: %s port_local\n", argv[0]);
        exit(-1);
    }

    int sockfd; // descripteur de socket
    char buf[TAILLE_LIGNE];
    char bufi[TAILLE_LIGNE + 31];
    char buf2[TAILLE_LIGNE]; // espace necessaire pour stocker le message recu
    fd_set readfds;
    fd_set readfds2;
    int nb;
    int sockclient[MAX_C];
    char user[MAX_C][TAILLE_NOM];
    char** message;  // utilisé dans decoupeChaine
    int indice = 0 ; // indice de la position de la socket liée au nom
    int first_mess[MAX_C] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; // Indique si c'est le premier message du client
    // taille d'une structure sockaddr_in utile pour la fonction recvfrom
    socklen_t fromlen = sizeof(struct sockaddr_in);
    struct sockaddr_in my_addr; // structure d'adresse qui contiendra les param
                                // reseaux du recepteur
    struct sockaddr_in client[MAX_C]; // structure d'adresse qui contiendra les
                                   // param reseaux de l'expediteur
    // creation de la socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);



    // initialisation de la structure d'adresse du recepteur (pg local)

    // famille d'adresse
    my_addr.sin_family = AF_INET;
    // recuperation du port du recepteur
    my_addr.sin_port = htons(atoi(argv[1]));
    // adresse IPv4 du recepteur
    inet_aton("127.0.0.1", &(my_addr.sin_addr));
    // association de la socket et des param reseaux du recepteur
    if (bind(sockfd, (void *)&my_addr, sizeof(struct sockaddr_in)) != 0) {
        perror("erreur lors de l'appel a bind -> ");
        exit(-2);
    }
    listen(sockfd, 5);

    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    FD_SET(sockfd, &readfds);
    int taille = 0;
    readfds2 = readfds;

    while (1) {

        // reception de la chaine de caracteres
        memset(buf, '\0', TAILLE_LIGNE);
        memset(buf2, '\0', TAILLE_LIGNE);
        memset(bufi, '\0', TAILLE_LIGNE+TAILLE_NOM);
        readfds2 = readfds;
        nb = max(sockclient) + 1;

        select(nb, &readfds2, 0, 0, 0);

        // accepte la connexion du client
        if (FD_ISSET(sockfd, &readfds2)) {
            sockclient[taille] =
                accept(sockfd, (void *)&client[taille], &fromlen);
            FD_SET(sockclient[taille], &readfds);
            taille++;
        }

        for (int j = 0; j < taille; j++) {

            if (FD_ISSET(sockclient[j], &readfds2)) {
                // Si premier message, alors c'est le pseudonyme à stocker
                if (first_mess[j]){
                    if (recv(sockclient[j], buf, TAILLE_LIGNE, 0) == -1) {
                        perror("erreur de reception -> ");
                        exit(-3);
                    }
                    // mise à jours de first_mess et on previent les autres clients 
                    // de la nouvelle connexion
                    strcpy(user[j], buf);
                    first_mess[j] = 0;
                    printf("%s a rejoint la discussion\n",user[j]);
                    fflush(stdout);
                    strcat(buf," vient de se connecter\n");
                    for (int t=0;t<taille;t++){
                        if (send(sockclient[j], buf, strlen(buf) + 1, 0) == -1) {
                                perror("erreur a l'appel de la fonction send -> ");
                                exit(-2);
                        }
                    }


                } else {
                    if (recv(sockclient[j], buf, TAILLE_LIGNE, 0) == -1) {
                        perror("erreur de reception -> ");
                        exit(-3);
                    }
                    
                    // Si le client veut se deconnecter
                    if (strncmp(buf, "/quit", 5) == 0) {
                        printf("%s a quitté la discussion\n", user[j]);
                        fflush(stdout);
                        if (send(sockclient[j], buf, strlen(buf) + 1, 0) == -1) {
                            perror("erreur a l'appel de la fonction send -> ");
                            exit(-2);
                        }
                        // On previent les autres clients
                        strcat(bufi, user[j]);
                        strcat(bufi, " sort la discussion\n");
                        for (int u=0; u<taille; u++){
                            if (u != j){
                                if (send(sockclient[u], bufi, strlen(bufi) + 1, 0) == -1) {
                                    perror("erreur a l'appel de la fonction send -> ");
                                    exit(-2);
                                }
                            }
                        }
                        

                        // Mise à jour des tableaux
                        
                        strcpy(user[j],user[taille-1]);
                        sockclient[j] = sockclient[taille-1];
                        first_mess[j] = first_mess[taille-1];
                        client[j] = client[taille-1]; 
                        first_mess[taille-1]=1;
                        taille--;
                        
                    
                      // Si un client veut parler en broadcast
                    } else {
                        if(strstr(buf, "::") == NULL){
                            strcat(bufi,user[j]);
                            strcat(bufi,":");
                            strcat(bufi,buf);                        
                            for (int i = 0; i < taille; i++) {
                                if (j != i){
                                    if (send(sockclient[i], bufi, strlen(bufi) + 1, 0) ==
                                        -1) {
                                        perror("erreur a l'appel de la fonction send -> ");
                                        exit(-2);
                                    }
                                }
                            }
                        }
                        // Si un client veut parler en privé
                        else{
                            message = decoupeChaine(buf, "::");
                            fflush(stdout);
                            for (int k=0; k<taille; k++){
                                if (strcmp(user[k], message[0]) == 0){
                                    indice = k;
                                }
                            }
                            strcat(bufi,user[j]);
                            strcat(bufi,":");
                            strcat(bufi,message[1]);
                            if (send(sockclient[indice], bufi, strlen(bufi) + 1, 0) ==-1) {
                                    perror("erreur a l'appel de la fonction send -> ");
                                    exit(-2);

                            }
                        }
                    // affichage de la chaine de caracteres recue
                    }
                }
            }
        }
        if (FD_ISSET(0, &readfds2)) {
            //Si le serveur souhaite se deconnecter
            read(0, buf2, sizeof(buf2));
            if(strcmp(buf2, "/quit")){
                printf("deconnexion serveur\n");
                for(int i=0; i<taille; i++){
                    if (send(sockclient[i], "/quit", 5, 0) == -1) {
                            perror("erreur a l'appel de la fonction send -> ");
                            exit(-2);
                    }
                }
                close(sockfd);
                return 0;
            }
        }
    }
    // fermeture de la socket
    close(sockfd);
    for (int k = 0; k < taille; k++)
        close(sockclient[k]);
    return 0;
}
