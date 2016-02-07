#include <stdio.h>
#include <stdlib.h>       
#include <arpa/inet.h>
#include <unistd.h> 
#include <string.h>


int creer_serveur(int port){

int socket_serveur;
socket_serveur = socket (AF_INET,SOCK_STREAM,0);
if(socket_serveur == -1)
{
perror("socket_serveur");
/*traitement de l’erreur*/
}

struct sockaddr_in saddr ;
saddr.sin_family = AF_INET ; /* Socket ipv4 */
saddr.sin_port = htons (port); /* Port d ’ écoute */
saddr.sin_addr.s_addr = INADDR_ANY ; /* écoute sur toutes les interfaces */
if(bind(socket_serveur,(struct sockaddr*)& saddr, sizeof(saddr)) == -1)
{
perror ("bind socker_serveur");
/*traitement de l’erreur */
}

if(listen(socket_serveur, 10) == -1)
{
perror("listen socket_serveur");
/* traitement d ’ erreur */
}

int socket_client ;
socket_client = accept(socket_serveur, NULL, NULL );
if(socket_client == -1)
{
perror ("accept");
/* traitement d ’ erreur */
}
/* On peut maintenant dialoguer avec le client */
/*const char * test = "test \n";
char * recup;

while(1){
sleep(1);
write(socket_client, message_bienvenue, strlen (message_bienvenue));

write(socket_serveur, test, strlen(test));
read(socket_serveur, &recup, strlen(test));
write(socket_client, &recup, strlen(recup));
}*/


while(1){
	int fd;
	char buf[512];
	if((fd = read(socket_client, buf, sizeof(buf)))==-1){

		perror("erreur de lecture");
		return 1;

	} 
	buf[511] = '\0';

	write(socket_client, buf, fd);
}



return socket_serveur;
}


