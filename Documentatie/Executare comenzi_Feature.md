### 1. Primirea Comenzii de Catre Client

Atunci cand un client primeste un semnal de tip `C` de la server, acesta initiaza executia instructiunii specificate de server. Comanda poate include o gama larga de operatiuni – de la setari si actualizari de sistem la actiuni de diagnosticare si reparare – iar clientul o va executa imediat ce o primeste, in conformitate cu permisiunile sale.

### 3. Feedback-ul Clientului catre Server

Clientul va intoarce output-ul comenzii la server, iar server-ul va pune acea comanda intr-un fisier din directorul aferent clientului








































- In momentul in care clientul primeste un semnal te tip `COMMAND`. Acesta va executa comanda data de la server.
- Serverul, pentru a asigura executarea comenzii pe mai multe dispozitive, va avea drept parametru si numele calculatoarelor la care se vrea executarea comenzii.
- Clientul dupa ce executa comanda, va transmite servarului astfel:
	- Daca comada a fost executata cu succes, clientul va transmite serverului un mesaj de succes
	- Daca comanda a generat o eroare, clientul va transmite serverului mesajul de eroare
