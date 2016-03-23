#include <stdio.h>
#include <stdlib.h>       
#include <arpa/inet.h>
#include <unistd.h> 
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "socket.h"


char * fgets_or_exit(char * buffer , int size , FILE * stream) {

	if(fgets(buffer, size, stream) == NULL)
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

int analyse_GET(const char * buf, http_request * request) {
	request->method = HTTP_UNSUPPORTED;	
	if(strncmp(buf, "GET ", 4) != 0)
		return -1;
	request->method = HTTP_GET;
	return 0;
}

int trois_mots(const char * buf, http_request * request) {
	
	char * delim;
	char * buffer;
    buffer= strdup(buf);
	if((delim = strtok(buffer," "))==NULL)
		return -1;

    if((delim = strtok(NULL," "))==NULL){	
	return -1;
	}
	request->url = delim;
	printf("%s\n", request->url);



	if((delim = strtok(NULL," "))==NULL)
		return -1;


	return 0;
}

int analyse_HTTP(const char * buf, http_request * request) {

	char * delim;
	char * buffer;
    buffer= strdup(buf);
	delim = strtok(buffer," ");
	delim = strtok(NULL," ");
	delim = strtok(NULL," ");

	if(strcmp(delim, "HTTP/1.1\n") == 0 || strcmp(delim, "HTTP/1.0\n") == 0) {

		if(strcmp(delim, "HTTP/1.0\n") == 0){
			request->major_version = 1;
			request->minor_version = 0;
		}else if(strcmp(delim, "HTTP/1.1\n") == 0){
			request->major_version = 1;
			request->minor_version = 1;
		}

		return 0;
	}
	return -1;
}


int parse_http_request(const char *request_line , http_request *request)
{
	analyse_GET(request_line, request);
 	if(trois_mots(request_line, request)!=0 || analyse_HTTP(request_line, request)!=0)
		return -1;
	return 0;
}

void skip_headers(FILE *client)
{
    char line[255];
	
	while(fgets_or_exit(line, 255, client) != NULL && strcmp(line, "\n") != 0 && strcmp(line, "\r\n") != 0);
}

void send_status(FILE *client , int code , const char *reason_phrase)
{
  fprintf(client, "HTTP/1.1 %d %s\n", code, reason_phrase);
}

void send_response(FILE *client , int code , const  char *reason_phrase , const  char *message_body)
{
  send_status(client, code, reason_phrase);
  fprintf(client, "Content-Length : %d\r\n\r\n", (int)strlen(message_body));
  fprintf(client, message_body);
  fflush(client);  
}

int creer_serveur(int port){

	initialiser_signaux();

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
	
		
		
		socket_client = accept(socket_serveur, NULL, NULL );
		if(socket_client == -1)
		{
			perror ("accept");
			return 1;
			//traitement d’erreur
		}


	//On peut maintenant dialoguer avec le client

	/*char * message_bienvenue="\nBienvenue sur le serveur de Alexandre Dupriez et Bastien Dusart. Pour vous accueillir dans les meilleures conditions, nous vous offrons ce sublime poème:\n\nMa souris.\nMa souris s'est pendue ce matin.\nJe me doutais bien que quelque chose\nNe tournait pas rond. La mollette sans entrain,\nLe pointeur sans vigueur, tout semblait morose.\n\nEn allumant mon écran j'ai vite reconnu\nSon écriture en Gothic (12pts) en gras.\nC'était son testament, sa lettre de pendue\nQui, le clavier à se consoler, aidera.\n\nJe savais que la force de l'unité centrale\nEtait de rester d'une froide logique.\nMais, voyez-vous, ce petit animal\nAvait un cœur de braise, un grand sens de l'éthique.\n\nElle se sentait déconsidérée devant les raccourcis,\nSoutenant pourtant mon regard cathodique\nSon ventre de rubis glissait, si doux sur le tapis,\nMais le temps entamait sa destruction méthodique.\n\nJ'aurais aimé avoir le temps de te dire tout mon amour\nLorsque ma main te guidait doucement.\nJ'aurais aimé te dire que pour toujours\nJe me souviendrais du galbe de ton dos, tel un amant.\n\nTu es morte aujourd'hui, pendue au bout du tiroir.\nJe regarde gourmand, vers une belle remplaçante.\nNe t'inquiète pas, je n'oublierai jamais les soirs,\nOu de mes doigts, je caressais, léger, mon amante.\n\n\n";
	*/

		if(fork()==0){

			    FILE *client = fdopen(socket_client, "w+");	

				char first_line[255];
				fgets_or_exit(first_line, 255, client);
				skip_headers(client);

				http_request request;
				int bad_request = parse_http_request(first_line, &request);				
				
				if(bad_request==-1)
					send_response(client, 400, "Bad Request", "Bad request");
				else if (request.method == HTTP_UNSUPPORTED)
					send_response(client, 405, "Method Not Allowed", "Method Not Allowed");
				else if (strcmp(request.url , "/") == 0)
					send_response(client, 200, "OK", "Bienvenue");
				else
					send_response(client, 404, "Not Found", "Not Found");

		}

		else{
			close(socket_client);
		}
	}


	return socket_serveur;
}



