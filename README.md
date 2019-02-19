# Consegna 2 - Sistemi Operativi

Questa è la mia versione della consegna 2 data come progetto di Sistemi Operativi.
Descriverò nel modo più veloce e preciso l'implementazione.

SERVER
Per prima cosa descriverò il server. AL primo accesso chiedrà di settare una password per potersi collegare coi client successivi.
Dopo il settaggio, si mette in attesa (accept) di qualche client.
Quando il client si connette inserendo la password corretta.
Ho implementato uanche la possibilità di scegliere il funzionamento del server:
- Manual mode, ossia la possibilita di approvare manualmente la richiesta di connessione da parte di un client (inserendo manul_mode come secondo parametro)
- Default, ossia il server accetta qualsiasi tentativo di connessione da parte dei client (non inserendo nessun secondo       argomento)
Quando la connessione è avvenuta, il server manda un menu al client con una write per far scegliere un operazione al client:
1-Il server cerca nella home un file scelto dal client, di qualsiasi estensione, e manda una lista dei trovati al client.
Il client puo decidere anche di scaricare uno dei file trovati in una qualsiasi cartella (solo per i file .txt)
2-Il server stampa a video la lista dei path da una directory in giù
3-Il server stampa a video i file da una directory in giu
4-Il client esce
5-Il client esce e manda un segnale di chiusura anche al server.

CLIENT
Il client all'avvio chiede l'inserimento di username e pass del server per potervi accedere.
Se accede, riceve dal server le opzioni e sceglie quella desiderata.

PRECISAZIONI
1-Non sono riuscito a risolvere il problema del SIGINT che killa anche il server
2-Il nuovo file .txt che verrà scaricato della cartella scelta dal client ha un nome non compatibile e pieno di caratteri non compatibili, anche se il corpo del file è identico a quello dell'originale.

