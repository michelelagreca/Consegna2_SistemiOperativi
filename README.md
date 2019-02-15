# Consegna 2 - Sistemi Operativi

Questa è la mia versione della consegna 2 data come progetto di Sistemi Operativi.
Descriverò nel modo più veloce e preciso l'implementazione.

SERVER
Per prima cosa descriverò il server. AL primo accesso chiedrà di settare una password per potersi collegare coi client successivi.
Dopo il settaggio, si mette in attesa (accept) di qualche client.
Quando il client si connette inserendo la password corretta, il server chiede se ultimare o meno l'accesso di quel client.
Se viene consentito, il server manda un menu al client con una write per far scegliere un operazione al client:
1-Il server cerca nella home un file scelto dal client
2-Il server stampa a video la lista dei path da una directory in giù
3-Il server stampa a video i file da una directory in giu
4-Il client esce
5-Il client esce e manda un segnale di chiusura anche al server.

CLIENT
Il client all'avvio chiede l'inserimento di username e pass del server per potervi accedere.
Se accede, riceve dal server le opzioni e sceglie quella desiderata.

DUBBI
Non ho ben capito come deve avvenire il trasferimento di file da server  a client e come
dovrei far stampare una lista di file trovati dal server al client.
