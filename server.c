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


///////////////////////////////////////////

int cores;

char path[200];				
pthread_mutex_t mn;			// MUTEX PER L'ACCESSO DEL MAIN THREAD ALLA LISTA
pthread_mutex_t mutexs[20];		// DEFINISCO UN ARRAY DI MUTEX DI 20 ELEMENTI (PENSO POSSANO BASTARE)
					// PER L'ACCESSO AI VARI THREAD, MA IN UN SECONDO MOMENTO NE INIZIALIZZERO' SOLO N 
					// QUANTI SONO I CORE DEL CALCOLATORE.

int fine=0;				// VARIBILE CHE SERVIRA' AL MAIN PER CAPIRE CHE i THREAD HANNO FINITO

char utente_client[100];
////////////////////////////////////////////

// PROTOTIPI 

void *thread_function(void *args);
void inserimento(list *lista, char *s);
void stampa_lista(list lista);
void cancella(list *lista, char *s);
int register_users(int client, char *pass_serv);
int opzioni(int client);
int riempi_lista (char *decisione);
void cerca_elemento(list lista1, list lista2, list *lista3, char *string);

// MAIN

int main(){
	
	printf("\n---SERVER---\n\n");
	
	int serv_sock, client_sock, len;
	int time_to_exit1=0, time_to_exit2=0, esci_dal_ciclo=0;
	struct sockaddr_in s, c;
	char password_server1[100];
	char password_server2[100];
	char temp[200];
	int esito, scelta_client;
	int temp1;
	
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
	listen(serv_sock, 10);
	
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
		
		esito=register_users(client_sock, password_server1);
		if(esito==-1){}
		else{
			
			while(esci_dal_ciclo==0){
				printf("Mostro le opzioni al client...\n\n");
				scelta_client=opzioni(client_sock);
				printf("Scelta client: %d\n\n", scelta_client);
				switch(scelta_client){
				default:
				write(client_sock, "Il comando inserito non corrisponde a nessuna azione.\n", 100);
				break;
				
				case 1:
				write(client_sock, "Hai scelto: Cerca file nella home.\n\nInserisci il nome del file da cercare: ", 100);
				read(client_sock, temp, 100);
				printf("Cerco '%s' nella home...\n", temp);
				char temporan[100]="/home";
				temp1=riempi_lista(temporan);
				if(temp1==-1)
					break;
				printf("Ho trovato i seguenti path name del file cercato:\n\n");
				
				cerca_elemento(lista_generale, lista_file, &lista_dei_trovati, temp);
				stampa_lista(lista_dei_trovati);		
				lista_generale = NULL;
				lista_file = NULL;
				lista_dei_trovati=NULL;
				break;
				
				case 2:
				write(client_sock, "Hai scelto: Visualizza contenuto di una directory.\n\nInserisci il path della directory: ", 100);
				read(client_sock, temp, 100);
				temp1=riempi_lista(temporan);
				if(temp1==-1)
					break;
				printf("\nLISTA:\n\n");
				stampa_lista(lista_generale);
				lista_generale = NULL;
				lista_file = NULL;
				
				break;
				
				case 3:
				write(client_sock, "Hai scelto: Visualizza tutti i file dato un path.\n\nInserisci il path della directory: ", 100);
				read(client_sock, temp, 100);
				temp1=riempi_lista(temporan);
				if(temp1==-1)
					break;
				printf("\nLISTA FILE:\n\n");
				stampa_lista(lista_file);
				lista_generale = NULL;
				lista_file = NULL;
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
					exit(0);
				}
				else{
					printf("Chiusura Negata.\n\n");
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

int register_users(int client, char *pass_serv){
	
	
	char password_client[100];
	char st1[100]="Inserisci un username: ";
	char st2[100]="Inserisci la password del server: ";
	char temp[3];
	write(client, st1, sizeof(st1));
	write(client, st2, sizeof(st2));
	read(client, utente_client, 100);//ricevo us e pass
	read(client, password_client, 100);
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


int opzioni(int client){
	int scelta;
	write(client, "\n\n\nBenvenuto del menu' principale del server.\n\n1 - Cerca file nella home\n2 - Visualizza contenuto di una directory\n3 - Visualizza tutti i file dato un path\n4 - Esci\n5 - Chiudi Server ed esci\n\nInserisci il comando da eseguire: ", 300);
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

int riempi_lista (char *decisione){
	
	struct dirent *d;
	
	strcpy(path, decisione);
	
	char momentanea[200];
	strcpy(momentanea, path);
	strcat(momentanea, "/");
	
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
			strcpy(momentanea, path);
			strcat(momentanea, "/");
			
		}
	}
	//closedir(open);
	
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
	int ex=0;
	char m[200];
	char m_2[200];
	struct dirent *dir;
	while(1){
		
		pthread_mutex_lock(&(mutexs[x]));
		lista=lista_generale;
		listafile=lista_file;
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
						}
					}
					ex=1;
					lista=NULL;
					listafile=NULL;
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
				//closedir(open);
			}
			
		}
		pthread_mutex_unlock(&mn);
	}
	pthread_exit(NULL);
}