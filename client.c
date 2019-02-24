
// VERS. 24/02/2019 22.25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

	client_sock=socket(AF_INET, SOCK_STREAM, 0);
	if(client_sock==-1){
		perror("Error opening socket.\n");
		exit(1);
	}
		
	s.sin_family=AF_INET;
	s.sin_port=htons(11111);
	s.sin_addr.s_addr=inet_addr("127.0.0.1");
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
	//printf("%d\n", esito_formattato);//qui stampa il codice di esito
	if(esito_formattato==-1){
		printf("Terminazione...\n");
	}
	else{
		int esci_dal_ciclo=0;
		while(esci_dal_ciclo==0){			
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			read(client_sock, stringa1, 320);
			printf("%s", stringa1);
			scanf("%d", &scelta);
			write(client_sock, &scelta, sizeof(scelta));//mando la scelta
			
			switch(scelta){
			default:
			read(client_sock, stringa1, 320);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			write(client_sock, "ricev", 10);
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			esci_dal_ciclo=1;
			break;
			case 1:
			read(client_sock, stringa1, 320);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			scanf("%s", stringa2);
			write(client_sock, stringa2, sizeof(stringa2));
			memset(stringa2, 0, sizeof(stringa2));
			read(client_sock, stringa1, 200);
			if(strcmp(stringa1, "Nessun elemento trovato")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			write(client_sock, "r", 4);
			while(fin==0){
				read(client_sock, stringa1, 200);
				if(strcmp(stringa1, "finelista") == 0){		
					fin=1;
					memset(stringa1, 0, sizeof(stringa1));
				}
				else{
					printf("%s\n", stringa1);
					memset(stringa1, 0, sizeof(stringa1));
				}
				write(client_sock, "ricev", 10);
				
			}
			fin=0;
			read(client_sock, stringa1, 90);
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			scanf("%s", stringa2);
			write(client_sock, stringa2, sizeof(stringa2));
			memset(stringa2, 0, sizeof(stringa2));
			read(client_sock, stringa1, 90);
			if(strcmp(stringa1, "Il file inserito non esiste")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			scanf("%s", stringa1);
			write(client_sock, stringa1, sizeof(stringa1));
			read(client_sock, stringa1, 300);
			if(strcmp(stringa1, "La directory inserita non esiste")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			write(client_sock, "ricevuto", 10);
			
			break;
			case 2:
			read(client_sock, stringa1, 320);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			scanf("%s", stringa2);
			write(client_sock, stringa2, sizeof(stringa2));
			memset(stringa2, 0, sizeof(stringa2));
			read(client_sock, stringa1, 200);
			if(strcmp(stringa1, "La directory inserita non esiste")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, strlen(stringa1));
			write(client_sock, "r", 4);
			while(fin==0){
				read(client_sock, stringa1, 200);
				if(strcmp(stringa1, "finelista") == 0){		
					fin=1;
					memset(stringa1, 0, strlen(stringa1));
				}
				else{
					printf("%s\n", stringa1);
					memset(stringa1, 0, strlen(stringa1));
				}
				write(client_sock, "ricev", 10);
				
			}
			fin=0;
			break;
			case 3:
			read(client_sock, stringa1, 320);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			scanf("%s", stringa2);
			write(client_sock, stringa2, sizeof(stringa2));
			memset(stringa2, 0, sizeof(stringa2));
			read(client_sock, stringa1, 200);
			printf("%s\n", stringa1);
			memset(stringa1, 0, strlen(stringa1));
			write(client_sock, "r", 4);
			while(fin==0){
				read(client_sock, stringa1, 200);
				if(strcmp(stringa1, "finelista") == 0){		
					fin=1;
					memset(stringa1, 0, strlen(stringa1));
				}
				else{
					printf("%s\n", stringa1);
					memset(stringa1, 0, strlen(stringa1));
				}
				write(client_sock, "ricev", 10);
				
			}
			fin=0;
			break;

			case 4:
			read(client_sock, stringa1, 320);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			scanf("%s", stringa2);
			write(client_sock, stringa2, sizeof(stringa2));
			memset(stringa2, 0, sizeof(stringa2));
			read(client_sock, stringa1, 200);
			if(strcmp(stringa1, "Nessun elemento trovato")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			write(client_sock, "r", 4);
			while(fin==0){
				read(client_sock, stringa1, 200);
				if(strcmp(stringa1, "finelista") == 0){		
					fin=1;
					memset(stringa1, 0, sizeof(stringa1));
				}
				else{
					printf("%s\n", stringa1);
					memset(stringa1, 0, sizeof(stringa1));
				}
				write(client_sock, "ricev", 10);
				
			}
			fin=0;
			read(client_sock, stringa1, 90);
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			char path10[100];
			scanf("%s", path10);
			write(client_sock, path10, sizeof(path10));
			memset(stringa2, 0, sizeof(stringa2));
			read(client_sock, stringa1, 90);
			if(strcmp(stringa1, "Il file inserito non esiste")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			scanf("%s", stringa1);
			write(client_sock, stringa1, sizeof(stringa1));
			read(client_sock, stringa1, 300);
			if(strcmp(stringa1, "La directory inserita non esiste")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			write(client_sock, "ricevuto", 10);
			read(client_sock, stringa1, 300);//prendo la dir
			
			write(client_sock, "ricevuto", 10);
			memset(stringa2, 0, sizeof(stringa2));
			//copio
			
			char nomefile[200];
			char path_nuovo[200];
			int len=strlen(path10)-1;
			int i=0, j=len;
			
			
			while(path10[j] != '/'){
				nomefile[i] = path10[j];
				i++;
				j--;
			}
			for(i=0, j=strlen(nomefile)-1;i<strlen(nomefile)/2;i++, j--){
				char temp=nomefile[i];
				nomefile[i]=nomefile[j];
				nomefile[j]=temp;
			}
			
			
			strcpy(path_nuovo, stringa1);
			strcat(path_nuovo, "/");
			strcat(path_nuovo, nomefile);
			FILE *nuovo = fopen(path_nuovo, "w");
			
			
			int escii=0;
			while(escii==0) {
		    		read(client_sock, stringa2, 300);
		    		if(strcmp(stringa2, "fine")==0){
		    			
		    			memset(stringa2, 0, sizeof(stringa2));
		    			write(client_sock, "ricevuto", 10);
		    			escii=1;
		    		}
		    		else{
			      		
			      		fprintf(nuovo, "%s", stringa2);
			      		memset(stringa2, 0, sizeof(stringa2));
			    		write(client_sock, "ricevuto", 10);
		    		}
		  	}
			escii=0;
		  	
		  	fclose(nuovo);
			printf("Trasferimento completato.\n\n");
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			
			break;		
			
			case 5:
			read(client_sock, stringa1, 320);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			scanf("%s", stringa2);
			write(client_sock, stringa2, sizeof(stringa2));
			memset(stringa2, 0, sizeof(stringa2));
			read(client_sock, stringa1, 200);
			if(strcmp(stringa1, "Nessun elemento trovato")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			write(client_sock, "r", 4);
			while(fin==0){
				read(client_sock, stringa1, 200);
				if(strcmp(stringa1, "finelista") == 0){		
					fin=1;
					memset(stringa1, 0, sizeof(stringa1));
				}
				else{
					printf("%s\n", stringa1);
					memset(stringa1, 0, sizeof(stringa1));
				}
				write(client_sock, "ricev", 10);
				
			}
			fin=0;
			read(client_sock, stringa1, 90);
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			char path12[100];
			scanf("%s", path12);
			write(client_sock, path12, sizeof(path12));
			memset(stringa2, 0, sizeof(stringa2));
			read(client_sock, stringa1, 90);
			if(strcmp(stringa1, "Il file inserito non esiste")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			scanf("%s", stringa1);
			write(client_sock, stringa1, sizeof(stringa1));
			read(client_sock, stringa1, 300);
			if(strcmp(stringa1, "La directory inserita non esiste")==0){
				printf("%s\n", stringa1);
				write(client_sock, "ricev", 10);
				memset(stringa1, 0, sizeof(stringa1));
				memset(stringa2, 0, sizeof(stringa2));
				break;
			}
			printf("%s\n", stringa1);
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			
			char buff[200];
			char t4[20];
			char *res;
			FILE *esiste = fopen(path12, "r");
			
			int esciii=0;
			while(esciii==0) {
		    		res=fgets(buff, 200, esiste);
		    		if(res==NULL ){
		      			
		      			write(client_sock, "fine", 4);
		      			read(client_sock, t4, 10);
					memset(buff, 0, sizeof(buff));
					memset(t4, 0, sizeof(t4));
		      			esciii=1;
		      		}
		      		else{
			      		
			      		write(client_sock, buff, strlen(buff));
					read(client_sock, t4, 10);
					memset(buff, 0, sizeof(buff));
					memset(t4, 0, sizeof(t4));
				}
		    		
		  	}

		  	fclose(esiste);
			//////////////////////////////
			
			printf("Caricamento completato.\n\n");
			memset(stringa1, 0, sizeof(stringa1));
			memset(stringa2, 0, sizeof(stringa2));
			
			break;
			
			case 6:
			read(client_sock, stringa1, 320);
			printf("%s", stringa1);//stampo l'opzione che riceverò
		 	esci_dal_ciclo=1;
		 	break;
		 	
		 	case 7:
		 	read(client_sock, stringa1, 320);
			printf("%s", stringa1);//stampo l'opzione che riceverò
			esci_dal_ciclo=1;
			break;
			}
		}
		 
	}
	close(client_sock);
	exit(0);
	

}
