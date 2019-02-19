#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <netinet/in.h>


// STRUTTURA LISTA

typedef struct data {
	char stringa[200];
	struct data *next;
}elemento;

typedef elemento* list;

list lista_generale = NULL;
list lista_file = NULL;
list lista_dei_trovati=NULL;
list lista_directory=NULL;


///////////////////////////////////////////

int cores;

char path[200];				
pthread_mutex_t mn;			// MUTEX PER L'ACCESSO DEL MAIN THREAD ALLA LISTA
pthread_mutex_t mutexs[20];		// DEFINISCO UN ARRAY DI MUTEX DI 20 ELEMENTI (PENSO POSSANO BASTARE)
					// PER L'ACCESSO AI VARI THREAD, MA IN UN SECONDO MOMENTO NE INIZIALIZZERO' SOLO N 
					// QUANTI SONO I CORE DEL CALCOLATORE.

int fine=0;				// VARIBILE CHE SERVIRA' AL MAIN PER CAPIRE CHE i THREAD HANNO FINITO

char utente_client[100];
int modalita;
////////////////////////////////////////////

// PROTOTIPI 

void *thread_function(void *args);
void inserimento(list *lista, char *s);
void stampa_lista(list lista);
void cancella(list *lista, char *s);
int register_users(int client, char *pass_serv, int mode);
int opzioni(int client);
int riempi_lista (char *decisione);
void cerca_elemento(list lista1, list lista2, list *lista3, char *string);
void invia_lista(list lista, int client);
void copia_file(char path_esiste[200], char destinazione[200]);

// MAIN

int main(int argc, char* argv[]){

	printf("\n---SERVER---\n\n");
	// All'inizio, controllo i numeri degli argomenti per decidere la modalita di utilizzo del server

	if(argc==1){
		printf("Modalita' di utilizzo: Connessione con qualsiasi client (default).\n\n");
		printf("\nNB. 	Le modalita' di utilizzo del server vengono settate:\n");
		printf("        ° Inserendo ' manual_mode ' come secondo argomento dopo ' /nome_file ' (Conferma manuale della connessione con i client)\n");
		printf("        ° Lasciando vuoto il secondo argomento dopo ' /nome_file ' (Connessione con qualsiasi client)\n\n\n");
		modalita=0;
	}
	if(argc==2){
		if(strcmp(argv[1], "manual_mode")==0){
			printf("Modalita' di utilizzo: Conferma manuale della connessione con i client.\n\n");
			printf("\nNB. 	Le modalita' di utilizzo del server vengono settate:\n");
			printf("        ° Inserendo ' manual_mode ' come secondo argomento dopo ' /nome_file ' (Conferma manuale della connessione con i client)\n");
			printf("        ° Lasciando vuoto il secondo argomento dopo ' /nome_file ' (Connessione con qualsiasi client)\n\n\n");
			modalita=1;
		}
		else{
			printf("Modalita' di utilizzo: Connessione con qualsiasi client (default).\n\n");
			printf("Warnings: Il secondo argomento inserito non coincide a nessuna modalita' di utilizzo!\n");
			printf("\nNB. 	Le modalita' di utilizzo del server vengono settate:\n");
			printf("        ° Inserendo ' manual_mode ' come secondo argomento dopo ' /nome_file ' (Conferma manuale della connessione con i client)\n");
			printf("        ° Lasciando vuoto il secondo argomento dopo ' /nome_file ' (Connessione con qualsiasi client)\n\n\n");
			modalita=0;
		}
	}
	
	
	
	int serv_sock, client_sock, len;
	int time_to_exit1=0, time_to_exit2=0, esci_dal_ciclo=0;
	struct sockaddr_in s, c;
	char password_server1[100];
	char password_server2[100];
	char temp[200];
	int esito, scelta_client;
	int temp1;
	char home[10]="/home";
	int h, w;
	
	printf("Inserisci una password. Con questa password i client potranno connettersi.\n");
	while(time_to_exit1==0){
		printf("Nuova password: ");
		scanf("%s", password_server1);
		printf("Conferma password: ");
		scanf("%s", password_server2);
		if(strcmp(password_server1, password_server2)==0){
			printf("Password '%s' confermata.\n\n", password_server1);
			time_to_exit1=1;
		}else
			printf("Le due password non coincidono. Riprova.\n");
	}
	serv_sock=socket(AF_INET, SOCK_STREAM, 0);
	if(serv_sock==-1){
		perror("Error creating socket.\n");
		exit(1);
	}
	s.sin_family=AF_INET;
	s.sin_port=htons(11111);
	s.sin_addr.s_addr=INADDR_ANY;
	if(bind(serv_sock, (struct sockaddr*)&s, sizeof(s))==-1){
		perror("Errore bind.\n");
		exit(1);
	}
	listen(serv_sock, 5);
	
	while(time_to_exit2==0){
		len=sizeof(c);
		esci_dal_ciclo=0;
		printf("\nAttendo connessione da qualche client...\n");
		client_sock=accept(serv_sock, (struct sockaddr*)&c, &len);
		if(client_sock==-1){
			perror("Errore accept.\n");
			exit(1);
		}
		printf("\nConnessione stabilita col client.\nFase di identificazione...\n");
		sleep(1);
		
		esito=register_users(client_sock, password_server1, modalita);
		if(esito==-1){}
		else{
			
			while(esci_dal_ciclo==0){
				scelta_client=0;
				printf("Mostro le opzioni al client...\n\n");
				scelta_client=opzioni(client_sock);
				printf("Scelta client: %d\n\n", scelta_client);
				switch(scelta_client){
				default:
				write(client_sock, "Il comando inserito non corrisponde a nessuna azione.\n", 100);
				read(client_sock, temp, 10);
				break;
				
				case 1:
				write(client_sock, "Hai scelto: Cerca file nella home.\n\nInserisci il nome del file da cercare: ", 100);
				read(client_sock, temp, 100);
				printf("Cerco '%s' nella home...\n", temp);
				
				temp1=riempi_lista(home);
				if(temp1==-1)
					break;
				char vuota[3]="\0";
				cerca_elemento(lista_generale, lista_file, &lista_dei_trovati, temp);
				cancella(&lista_dei_trovati, vuota); 
				if(lista_dei_trovati==NULL){
					printf("Nessun elemento trovato.\n\n");
					write(client_sock, "Nessun elemento trovato", 23);
					read(client_sock, temp, 10);
					memset(temp, 0, sizeof(temp));
					lista_generale = NULL;
					lista_file = NULL;
					lista_dei_trovati=NULL;
					lista_directory=NULL;
					break;
				}
				printf("\nInvio la lista dei path trovati al client...\n");
				write(client_sock, "\nLista dei path del file cercato:\n", 40);
				read(client_sock, temp, 10);
				memset(temp, 0, sizeof(temp));
				invia_lista(lista_dei_trovati, client_sock);
				
				printf("\nLista inviata.\n\n");	
				write(client_sock, "\nInserisci il path del file da scaricare tra quelli trovati (solo con estensioni .txt): ", 90);
				char path_da_scaricare[100];
				read(client_sock, path_da_scaricare, 130);
				list l1=lista_dei_trovati;
				h=0;
				w=0;
				while(h==0){
					while(l1!=NULL){
						if(strcmp(l1->stringa, path_da_scaricare)!=0){
							l1=l1->next;
							h=1;
							w=1;
						}
						else{
							l1=NULL;
							h=1;
							w=0;
						}
					}
					
				}
				if(w==1){
					printf("Il file inserito non esiste.\n\n");
					write(client_sock, "Il file inserito non esiste", 27);
					read(client_sock, temp, 20);
					memset(temp, 0, sizeof(temp));
					memset(path_da_scaricare, 0, sizeof(path_da_scaricare));
						
					lista_generale = NULL;
					lista_file = NULL;
					lista_dei_trovati=NULL;
					lista_directory=NULL;
					l1==NULL;
					break;
				}
				
				h=0;
				w=0;
				write(client_sock, "\nInserisci la directory dove scaricare il file: ", 70);
				char directory[100];
				read(client_sock, directory, 130);
				list l2=lista_directory;
				
				h=0;
				w=0;
				while(h==0){
					while(l2!=NULL){
						
						if(strcmp(l2->stringa, directory)!=0){
							l2=l2->next;
							h=1;
							w=1;
							
						}
						else{
							l2=NULL;
							h=1;
							w=0;
							
						}
					}
					
				}
				if(w==1){
					printf("La directory inserita non esiste.\n\n");
					write(client_sock, "La directory inserita non esiste", 32);
					read(client_sock, temp, 20);
					memset(temp, 0, sizeof(temp));
					memset(path_da_scaricare, 0, sizeof(path_da_scaricare));
					memset(directory, 0, sizeof(directory));
						
					lista_generale = NULL;
					lista_file = NULL;
					lista_dei_trovati=NULL;
					lista_directory=NULL;
					l1==NULL;
					l2==NULL;
					break;
				}
				h=0;
				w=0;
				char temp2[100];
				strcpy(temp2, "\nScaricamento di ' ");
				strcat(temp2, path_da_scaricare);
				strcat(temp2, " ' nella cartella ' ");
				strcat(temp2, directory);
				strcat(temp2, " ' ...");
				write(client_sock, temp2, sizeof(temp2));
				read(client_sock, temp, 20);
				memset(temp, 0, sizeof(temp));
				printf("\nTrasferimento del file ' %s ' in ' %s ' ...\n\n", path_da_scaricare, directory);
				copia_file(path_da_scaricare, directory);
				
				printf("\nTrasferimento completato\n\n");
				memset(temp, 0, sizeof(temp));
				memset(temp2, 0, sizeof(temp2));
				memset(path_da_scaricare, 0, sizeof(path_da_scaricare));
				memset(directory, 0, sizeof(directory));
				
				lista_generale = NULL;
				lista_file = NULL;
				lista_dei_trovati=NULL;
				lista_directory=NULL;
				l1==NULL;
				l2==NULL;
				break;
				
				case 2:
				write(client_sock, "Hai scelto: Visualizza i path dei file da una directory in giu'\n\nInserisci il path della directory: ", 150);
				read(client_sock, temp, 100);
				
				temp1=riempi_lista(temp);
				if(temp1==-1)
					break;
				
				printf("\nInvio la lista al client...\n");
				write(client_sock, "\nLista ricevuta dal server:\n\n", 30);
				read(client_sock, temp, 10);
				memset(temp, 0, sizeof(temp));
				invia_lista(lista_generale, client_sock);
				printf("\nLista inviata.\n\n");
				lista_generale = NULL;
				lista_file = NULL;
				lista_dei_trovati=NULL;
				lista_directory=NULL;
				break;
				
				case 3:
				write(client_sock, "Hai scelto: Visualizza i file da una directory in giu'.\n\nInserisci il path della directory: ", 150);
				read(client_sock, temp, 100);
				temp1=riempi_lista(temp);
				if(temp1==-1)
					break;
				printf("\nInvio la lista al client...\n");
				write(client_sock, "\nLista ricevuta dal server:\n\n", 30);
				read(client_sock, temp, 10);
				memset(temp, 0, sizeof(temp));
				invia_lista(lista_file, client_sock);
				printf("\nLista inviata.\n\n");
				lista_generale = NULL;
				lista_file = NULL;
				lista_dei_trovati=NULL;
				lista_directory=NULL;
				break;
				
				case 4:
				write(client_sock, "Hai scelto: Esci\n", 100);
				esci_dal_ciclo=1;
				
				break;
				
				case 5:
				write(client_sock, "Hai scelto: Chiudi Server ed esci\n", 100);
				printf("Il client '%s' ha le autorizzazioni necessarie per chiudere il server? [Y/N]: ", utente_client);
				scanf("%s", temp);
				if(strcmp(temp, "Y")==0 || strcmp(temp, "y")==0){
					printf("Chiusura permessa.\nTermino...\n");
					esci_dal_ciclo=1;
					time_to_exit2=1;
				}
				else{
					printf("Chiusura Negata.\n\n");
					esci_dal_ciclo=1;
				}
				
				break;
				}
			}
		}
		
		
	
	}
	close(serv_sock);
	close(client_sock);
	exit(0);
	
	
	
	
	
	
}

// FUNZIONI

void inserimento(list *lista, char *s){
	list aux, a, b;
	
	aux=(list)malloc(sizeof(elemento));
	if(aux==NULL){
		perror("Errore allocazione memoria.\n");
		exit(1);
	}
	strcpy(aux->stringa, s);
	
	a=*lista;
	b=NULL;
	while(a != NULL){
		b = a;
		a = a->next;
	}

	aux->next = a;

	if(b != NULL){
		b->next = aux;
		
	}
	else
		*lista=aux;
	
}

void stampa_lista(list lista){
	printf("\n");
	while(lista!=NULL){
		printf("%s\n", lista->stringa);
		lista=lista->next;
	}
	printf("\n");
}

void cancella(list *lista, char *s){
	list paux;
	if((*lista) != NULL)
	if(strcmp((*lista)->stringa, s)==0){
		paux = (*lista);
		(*lista) = (*lista)->next;
		free(paux);
		
	}
	else
		cancella(&(*lista)->next, s);
}

int register_users(int client, char *pass_serv, int mode){
	
	int m=mode;
	char password_client[100];
	char st1[100]="Inserisci un username: ";
	char st2[100]="Inserisci la password del server: ";
	char temp[3];
	write(client, st1, sizeof(st1));
	write(client, st2, sizeof(st2));
	read(client, utente_client, 100);//ricevo us e pass
	read(client, password_client, 100);
	if(m==1){ // se la modalita è quella con approvazione entri nel ciclo
		printf("Richiesta di accesso da parte dell'utente '%s'.\nAccettare la connessione? [Y/N]: ", utente_client);
		scanf("%s", temp);
		if((strcmp(temp, "Y")==0)||(strcmp(temp, "y")==0)){
			printf("Connessione permessa.\n");
			if(strcmp(pass_serv, password_client)!=0){
				printf("La password inserita dal client risulta errata.\n\n");
				write(client, "Connessione rifiutata: password errata.", 39);
				sleep(1);
				write(client, "-1", 2);
				return -1;
			}
			else{
				printf("La password inserita dal client risulta corretta.\nUtente '%s' connesso al server.\n\n", utente_client);
				write(client, "Connessione accettata.", 22);
				sleep(1);
				write(client, "0", 1);	
				return 0;
			}
		}
		else{
			printf("Connessione non permessa.\n");
			write(client, "Connessione non permessa.", 39);
			sleep(1);
			write(client, "-1", 2);
			return -1;
		}
	}
	if(m==0){ // se la modalita è quella di default
		
		if(strcmp(pass_serv, password_client)!=0){
			printf("La password inserita dal client risulta errata.\n\n");
			write(client, "Connessione rifiutata: password errata.", 39);
			sleep(1);
			write(client, "-1", 2);
			return -1;
		}
		else{
			printf("La password inserita dal client risulta corretta.\nUtente '%s' connesso al server.\n\n", utente_client);
			write(client, "Connessione accettata.", 22);
			sleep(1);
			write(client, "0", 1);	
			return 0;
		}
		
		
	}
}


int opzioni(int client){
	int scelta;
	write(client, "\n\n\nBenvenuto del menu' principale del server.\n\n1 - Cerca file nella home\n2 - Visualizza i path dei file da una directory in giu'\n3 - Visualizza i file da una directory in giu'\n4 - Esci\n5 - Chiudi Server ed esci\n\nInserisci il comando da eseguire: ", 300);
	read(client, &scelta, 10);
	return scelta;
	
}

void cerca_elemento(list lista1, list lista2, list *lista3, char *string){	
	list l1=lista1;
	list l2=lista2;
	list l3=NULL;
	l3=(list)malloc(sizeof(elemento));
	while(l1!=NULL&&l2!=NULL){
		if(strcmp(string, l2->stringa)==0){
			inserimento(&l3, l1->stringa);
		}
		
		l1=l1->next;
		l2=l2->next;
	}
	*lista3=l3;	
}

void invia_lista(list lista, int client){
	
	char stringa30[20];
	list l1=lista;
	
	while(l1 != NULL){
		write(client, l1->stringa, strlen(l1->stringa));
		read(client, stringa30, 19);
		memset(stringa30, 0, sizeof(stringa30));
		l1=l1->next;
	}
	write(client, "finelista", 20);
	read(client, stringa30, 19);
	memset(stringa30, 0, sizeof(stringa30));
	l1=NULL;
}

void copia_file(char path_esiste[200], char destinazione[200]){
	
	char buff[200];
	char *res;
	FILE *esiste = fopen(path_esiste, "r");
	
	char nomefile[200];
	char path_nuovo[200];
	int len=strlen(path_esiste)-1;
	int i=0, j=len;
	
	
	while(path_esiste[j] != '/'){
		nomefile[i] = path_esiste[j];
		i++;
		j--;
	}
	for(i=0, j=strlen(nomefile)-1;i<strlen(nomefile)/2;i++, j--){
		char temp=nomefile[i];
		nomefile[i]=nomefile[j];
		nomefile[j]=temp;
	}
	
	
	strcpy(path_nuovo, destinazione);
	strcat(path_nuovo, "/");
	strcat(path_nuovo, nomefile);
	FILE *nuovo = fopen(path_nuovo, "w");
	
	while(1) {
    		res=fgets(buff, 200, esiste);
    		if(res==NULL )
      			break;
      		fprintf(nuovo, "%s", buff);
    		
  	}

  	fclose(esiste);
  	fclose(nuovo);
	
}

int riempi_lista (char *decisione){
	
	struct dirent *d;
	
	strcpy(path, decisione);
	
	char momentanea[200];
	strcpy(momentanea, path);
	strcat(momentanea, "/");
	//inserimento(&lista_directory, path);
	DIR *open = opendir(path);
	if(open==NULL){
		perror("Errore apertura directory.\n");
		return -1;
	}
	while((d=readdir(open))!=NULL){
		if(strncmp(d->d_name, ".", 1)!=0){
			inserimento(&lista_file, d->d_name);
			strcat(momentanea, d->d_name);
			inserimento(&lista_generale, momentanea);
			if(opendir(momentanea)!=NULL){
				inserimento(&lista_directory, momentanea);
			}
			strcpy(momentanea, path);
			strcat(momentanea, "/");
			
		}
	}
	
	
	int i, controllo;
	int cores = get_nprocs_conf();
	
	pthread_t threads[cores];
	
	// Inizializzazione mutex
	
	controllo=pthread_mutex_init(&mn, NULL);
	if(controllo!=0){
		perror("Errore inizializzazione mutex.\n");
		exit(1);
	}
	for(i=0;i<cores;i++){
		controllo=pthread_mutex_init(&(mutexs[i]), NULL);
		if(controllo!=0){
			perror("Errore inizializzazione mutex.\n");
			exit(1);
		}
	}
	
	// Blocco Mutex
	
	pthread_mutex_lock(&mn);
	for(i=0;i<cores;i++){
		pthread_mutex_lock(&(mutexs[i]));
	}
	
	
	
	
	// Creazione thread
	
	for(i=0;i<cores;i++){
		controllo=pthread_create(&threads[i], NULL, thread_function, (void *)(intptr_t)i);
		if(controllo!=0){
			perror("Errore creazione thread.\n");
			exit(1);
		}	
	}
	i=0;
	while(fine==0){
		pthread_mutex_unlock(&(mutexs[i]));
		pthread_mutex_lock(&mn);
		if(fine==1){}
		else{
			i++;
			if(i==5){
				i=0;
			}
		}	
	}
	
	pthread_mutex_destroy(&mn);
	for(i=0;i<cores;i++){
		pthread_mutex_destroy(&(mutexs[i]));
	}
	
	for(i=0;i<cores;i++){
		pthread_cancel(threads[i]);
	}
	fine=0;
	
	return 0;

}



void *thread_function(void *args){
	int x = (intptr_t)args;
	list lista;
	list listafile;
	list listadirectory;
	int ex=0;
	char m[200];
	char m_2[200];
	struct dirent *dir;
	while(1){
		
		pthread_mutex_lock(&(mutexs[x]));
		lista=lista_generale;
		listafile=lista_file;
		listadirectory=lista_directory;
		ex=0;
		
		while(lista!=NULL&&listafile!=NULL){
			while(ex==0){
				DIR *open = opendir(lista->stringa);
				if(open!=NULL){
					strcpy(m_2, lista->stringa);
					cancella(&lista_generale, lista->stringa);
					cancella(&lista_file, listafile->stringa);
					while((dir=readdir(open))!=NULL){
						if(strncmp(dir->d_name, ".", 1)!=0){
							inserimento(&lista_file, dir->d_name);
							strcpy(m, m_2);
							strcat(m, "/");
							strcat(m, dir->d_name);
							inserimento(&lista_generale, m);
							if(opendir(m)!=NULL){
								inserimento(&lista_directory, m);
							}
						}
					}
					ex=1;
					lista=NULL;
					listafile=NULL;
					listadirectory=NULL;
				}
				else{
					lista=lista->next;
					listafile=listafile->next;
					
					if(lista==NULL&&listafile==NULL){
						fine=1;
						ex=1;
						pthread_mutex_unlock(&mn);
						pthread_exit(NULL);
					}
				}
				
			}
			
		}
		pthread_mutex_unlock(&mn);
	}
	pthread_exit(NULL);
}
