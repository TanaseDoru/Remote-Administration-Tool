### 1. Primirea Comenzii de Catre Client

Atunci cand un client primeste un semnal de tip `COMMAND` de la server, acesta initiaza executia instructiunii specificate de server. Comanda poate include o gama larga de operatiuni – de la setari si actualizari de sistem la actiuni de diagnosticare si reparare – iar clientul o va executa imediat ce o primeste, in conformitate cu permisiunile sale.

### 2. Distribuirea Comenzii pe Multiple Dispozitive

Pentru a permite executarea simultana pe mai multe dispozitive, serverul primeste, pe langa comanda propriu-zisa, o lista de nume de calculatoare (sau alte identificatoare unice) care reprezinta dispozitivele tinta. Serverul initiaza apoi conexiuni catre fiecare dispozitiv din lista, transmitand comanda si asigurand astfel ca toate dispozitivele desemnate vor primi si executa instructiunea. Acest mecanism este esential pentru a oferi un control centralizat, mai ales in retele mari sau in medii distribuite.

### 3. Feedback-ul Clientului catre Server

Dupa executarea comenzii, clientul trebuie sa ofere serverului un feedback referitor la rezultat, astfel incat administratorul sa poata monitoriza statusul executiei:

- **Mesaj de succes**: Daca executia comenzii a fost finalizata cu succes, clientul va transmite serverului un mesaj de confirmare, indicand ca totul a decurs conform asteptarilor.
    
- **Mesaj de eroare**: In cazul in care comanda a generat o eroare (de exemplu, din cauza unor permisiuni insuficiente, a unei instructiuni invalide sau a altor probleme neasteptate), clientul va trimite un mesaj catre server care contine detalii despre eroare. Acest mesaj poate include codul erorii si un mesaj descriptiv, pentru a permite administratorului sa inteleaga cauza problemei si, eventual, sa ia masuri suplimentare.






































- In momentul in care clientul primeste un semnal te tip `COMMAND`. Acesta va executa comanda data de la server.
- Serverul, pentru a asigura executarea comenzii pe mai multe dispozitive, va avea drept parametru si numele calculatoarelor la care se vrea executarea comenzii.
- Clientul dupa ce executa comanda, va transmite servarului astfel:
	- Daca comada a fost executata cu succes, clientul va transmite serverului un mesaj de succes
	- Daca comanda a generat o eroare, clientul va transmite serverului mesajul de eroare
