#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


// STRUTTURA LISTA

typedef struct data {
	char stringa[200];
	struct data *next;
}elemento;

typedef elemento* list;

list lista_generale = NULL;

int main(){
	
	int client_sock;
	struct sockaddr_in s;	
	char stringa1[100];
	char stringa2[100];
	char username[100];
	char password[100];
	char esito[100];
	char esito_num[10];
	int esito_formattato;
	int scelta;
	int n;
	int fin=0;
	int esci_dal_ciclo=0;

	client_sock=socket(AF_INET, SOCK_STREAM, 0);
	if(client_sock==-1){
		perror("Error opening socket.\n");
		exit(1);
	}
		
	s.sin_family=AF_INET;
	s.sin_port=htons(11111);
	s.sin_addr.s_addr=INADDR_ANY;
	if(connect(client_sock, (struct sockaddr*)&s, sizeof(s))==-1){
		perror("Errore connect.\n");
		exit(1);
	}
	printf("\nConnessione stabilita col server 11111.\n");
	read(client_sock, stringa1, 100);//username
	read(client_sock, stringa2, 100);//password

	printf("%s", stringa1);//li inserisco
	scanf("%s", username);
	printf("%s", stringa2);
	scanf("%s", password);
	write(client_sock, username, sizeof(username));//li mando
	write(client_sock, password, sizeof(password));
	read(client_sock, esito, 100);//esito connessione(riuscita o no)
	printf("%s\n", esito);//qui stampa se la connessione è avvenuta o no
	read(client_sock, esito_num, 2);
	sscanf(esito_num, "%d", &esito_formattato);
	if(esito_formattato==-1){
		printf("Terminazione...\n");
	}
	else{
		
		while(esci_dal_ciclo==0){
			read(client_sock, stringa1, 300);
			printf("%s", stringa1);
			scanf("%d", &scelta);
			write(client_sock, &scelta, sizeof(scelta));//mando la scelta
			
			switch(scelta){
			default:
			printf("La scelta effettuata non corrisponde a nessuna opzione del server.\n");
			break;
			case 1:
			read(client_sock, stringa1, 300);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			memset(stringa1, 0, sizeof(stringa1));
			scanf("%s", stringa2);
			write(client_sock, stringa2, sizeof(stringa2));
			memset(stringa2, 0, sizeof(stringa2));
			break;

			case 2:
			read(client_sock, stringa1, 300);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			scanf("%s", stringa2);
			write(client_sock, stringa2, sizeof(stringa2));
			memset(stringa2, 0, sizeof(stringa2));
			break;

			case 3:
			read(client_sock, stringa1, 300);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			scanf("%s", stringa2);
			write(client_sock, stringa2, sizeof(stringa2));
			break;

			case 4:
			read(client_sock, stringa1, 300);
			printf("%s", stringa1);//stampo l'opzione che riceverò
		 	esci_dal_ciclo=1;
		 	break;
		 	
		 	case 5:
		 	read(client_sock, stringa1, 300);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			esci_dal_ciclo=1;
			break;
			}
		}
		 
	}
	close(client_sock);
	exit(0);
	

}
