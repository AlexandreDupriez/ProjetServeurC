#include <stdio.h>
#include <stdlib.h>

int creer_serveur(int port){

int socket_serveur;
socket_serveur = socket (AF_INET,SOCK_STREAM,0);
if (socket_serveur == -1)
{
perror("socket_serveur");
/* traitement de l ’ erreur */
}






}


