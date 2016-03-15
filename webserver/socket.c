#include <stdio.h>
#include <stdlib.h>       
#include <arpa/inet.h>
#include <unistd.h> 
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int analyse_entete(const char * message){
	
	int nb_mots=0;
	unsigned int i = 0;
	char * delim;
	char * buf;

	buf= strdup(message);
	delim = strtok(buf," ");

	if(strncmp(delim, "GET", 4) != 0)
		return -1;

	while(i<strlen(message)){
		if(message[i]== ' ')
			nb_mots++;
		i++;
    }
		if(nb_mots!=2)
			return -1;

		delim = strtok(NULL," ");
		if(strcmp("/",delim)!=0)
			return 404;

		delim = strtok(NULL," ");
	if((strcmp(delim, "HTTP/1.0\n") != 0) && (strcmp(delim, "HTTP/1.1\n") != 0))	
		return -1;

	return 0;

}

void envoie_reponse(FILE * fclient, const char * phrase) {
	fprintf(fclient, "<Alakazam> %s\n", phrase);
}

char * recois_requete(char * buffer , int size , FILE * stream) {

	int i;
	for(i = 0; i<size; ++i) {
		buffer[i] = '\0';
	}

	fgets(buffer, size, stream); 
	fflush(stdout);

	if(buffer[0] == '\0')
		exit(1);

	return buffer;
}

void traitement_signal ( int sig ){
int statut;
	printf("Signal %d reçu \n " , sig );

	if(sig ==SIGCHLD){
		while(waitpid(-1,&statut,WNOHANG)>0);

	}
}

void initialiser_signaux(void){

	if (signal(SIGPIPE,SIG_IGN)==SIG_ERR)
	{
		perror("signal");
	}

	struct sigaction sa ;
	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask );
	sa.sa_flags = SA_RESTART ;
	if(sigaction(SIGCHLD,&sa,NULL) == -1)
	{
		perror ("sigaction(SIGCHLD)");
	}

}

int creer_serveur(int port){

	int socket_serveur;
	socket_serveur = socket (AF_INET,SOCK_STREAM,0);
	if(socket_serveur == -1)
	{
		perror("socket_serveur");
		/*traitement de l’erreur*/
	}

	int optval = 1;
	if (setsockopt(socket_serveur,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int)) == -1){
		perror("Can not set SO_REUSEADDR option" );
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

	

	while(1){

		int socket_client ;
		FILE * fclient;
		initialiser_signaux();
		socket_client = accept(socket_serveur, NULL, NULL );
		if(socket_client == -1)
		{
			perror ("accept");
			/* traitement d’erreur */
		}
	//On peut maintenant dialoguer avec le client

	char * message_bienvenue="\nBienvenue sur le serveur de Alexandre Dupriez et Bastien Dusart. Pour vous accueillir dans les meilleures conditions, nous vous offrons ce sublime poème:\n\nMa souris.\nMa souris s'est pendue ce matin.\nJe me doutais bien que quelque chose\nNe tournait pas rond. La mollette sans entrain,\nLe pointeur sans vigueur, tout semblait morose.\n\nEn allumant mon écran j'ai vite reconnu\nSon écriture en Gothic (12pts) en gras.\nC'était son testament, sa lettre de pendue\nQui, le clavier à se consoler, aidera.\n\nJe savais que la force de l'unité centrale\nEtait de rester d'une froide logique.\nMais, voyez-vous, ce petit animal\nAvait un cœur de braise, un grand sens de l'éthique.\n\nElle se sentait déconsidérée devant les raccourcis,\nSoutenant pourtant mon regard cathodique\nSon ventre de rubis glissait, si doux sur le tapis,\nMais le temps entamait sa destruction méthodique.\n\nJ'aurais aimé avoir le temps de te dire tout mon amour\nLorsque ma main te guidait doucement.\nJ'aurais aimé te dire que pour toujours\nJe me souviendrais du galbe de ton dos, tel un amant.\n\nTu es morte aujourd'hui, pendue au bout du tiroir.\nJe regarde gourmand, vers une belle remplaçante.\nNe t'inquiète pas, je n'oublierai jamais les soirs,\nOu de mes doigts, je caressais, léger, mon amante.\n\n\n";
	char * message_400="\nHTTP/1.1 400 Bad Request\n\rConnection: close\n\rContent-Length: 17 \n\n\r400 Bad request\n\r";
	//char * message_404="\nHTTP/1.1 404 Not Found\n\rConnection: close\n\rContent-Length: 17 \n\n\r404 Not Found\n\r";
	char * message_ok="\nHTTP/1.1 200 OK\n\rContent-Length: ";
	int tour = 0;
	int error400 = 1;
	//int error404 = 0;

		if(fork()==0){

			char buf[512];
			fclient = fdopen(socket_client, "w+");
			char * message;
			while((message=recois_requete(buf, sizeof(buf)/sizeof(buf[0]), fclient)))
			{


						/*if(error404==1 && ((strcmp(message,"\r\n")==0)||(strcmp(message,"\n")==0))){
							envoie_reponse(fclient,message_404);
							tour=0;
							error404=0;
						}*/
						if(error400==1 && ((strcmp(message,"\r\n")==0)||(strcmp(message,"\n")==0))){
							envoie_reponse(fclient,message_400);
							tour=0;
						}
						else if(error400==0 && ((strcmp(message,"\r\n")==0)||(strcmp(message,"\n")==0))){
							char str[15];
							sprintf(str, "%d", (int)strlen(message_bienvenue));
							char * message_end = concat(str,"\n\n\r200 OK\n\r");
							message_ok = concat(message_ok,message_end);
							envoie_reponse(fclient,message_ok);
							message_ok="\nHTTP/1.1 200 OK\n\rContent-Length: ";
							error400=1;
							tour=0;
							envoie_reponse(fclient,message_bienvenue);
						}

						if(analyse_entete(message)==0 && tour==0){
							error400=0;
						}
						/*else if(analyse_entete(message)==404 && tour==0){
							error404=1;
						}*/

						if(message && ((strcmp(message,"\r\n")!=0)&&(strcmp(message,"\n")!=0))){
						tour++;
						}
			}
		}
	}

	//close(socket_client);
	//close(socket_serveur);

	return socket_serveur;
}



