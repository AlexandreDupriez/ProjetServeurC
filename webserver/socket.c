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

int optval = 1;
if ( setsockopt(socket_serveur,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int)) == -1){
perror ( " Can not set SO_REUSEADDR option " );
}

struct sockaddr_in saddr ;
saddr.sin_family = AF_INET ; /* Socket ipv4 */
saddr.sin_port = htons (port); /* Port d ’ écoute */
saddr.sin_addr.s_addr = INADDR_ANY ; /* écoute sur toutes les interfaces */
if(bind(socket_serveur,(struct sockaddr*)& saddr, sizeof(saddr)) == -1)
{
perror ("bind socket_serveur");
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
//On peut maintenant dialoguer avec le client

char * message_bienvenue="Bienvenue sur le serveur de Alexandre Dupriez et Bastien Dusart. Pour vous accueillir dans les meilleures conditions, nous vous offrons ce sublime poème:\n\nMa souris.\nMa souris s'est pendue ce matin.\nJe me doutais bien que quelque chose\nNe tournait pas rond. La mollette sans entrain,\nLe pointeur sans vigueur, tout semblait morose.\n\nEn allumant mon écran j'ai vite reconnu\nSon écriture en Gothic (12pts) en gras.\nC'était son testament, sa lettre de pendue\nQui, le clavier à se consoler, aidera.\n\nJe savais que la force de l'unité centrale\nEtait de rester d'une froide logique.\nMais, voyez-vous, ce petit animal\nAvait un cœur de braise, un grand sens de l'éthique.\n\nElle se sentait déconsidérée devant les raccourcis,\nSoutenant pourtant mon regard cathodique\nSon ventre de rubis glissait, si doux sur le tapis,\nMais le temps entamait sa destruction méthodique.\n\nJ'aurais aimé avoir le temps de te dire tout mon amour\nLorsque ma main te guidait doucement.\nJ'aurais aimé te dire que pour toujours\nJe me souviendrais du galbe de ton dos, tel un amant.\n\nTu es morte aujourd'hui, pendue au bout du tiroir.\nJe regarde gourmand, vers une belle remplaçante.\nNe t'inquiète pas, je n'oublierai jamais les soirs,\nOu de mes doigts, je caressais, léger, mon amante.\n\n\n";


write(socket_client, message_bienvenue, strlen (message_bienvenue));

	
char buf[512];
int fd;
if((fd = read(socket_client, buf, sizeof(buf)))==-1){

		perror("erreur de lecture");
		return 1;

	} 
write(socket_client, buf, fd);

while(fd!=0){
	if((fd = read(socket_client, buf, sizeof(buf)))==-1){

		perror("erreur de lecture");
		return 1;

	} 
	buf[511] = '\0';

write(socket_client, buf, fd);
}

close(socket_client);
close(socket_serveur);

return socket_serveur;
}


