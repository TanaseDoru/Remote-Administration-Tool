## Descriere
Functia de Remote Admin Tool va fi implementata prin doua programe si anume:
- **Server**: Pe care se vor face monitorizari asupra calculatorului, dar se vor si da comenzi/cereri clientului pentru a face administrare
- **Client**: Acesta va rula in permanenta pe calculatorul celui caruia se doreste sa se faca administrarea/monitorizarea si va transmite datele necesare catre server pentru functionarea corecta a programului
- **Datele**: Datele vor fi transmise folosind string-uri, printr-o structura care va fi fomrata confrom cerintelor: un *int* pentru dimensiunea cererii; un *octet* pentru codul cererii, precum si un *sir de caratere* pentru mesajul efectiv

## Tehnologii folosite:
- **Multi-threading** pentru conectarea mai multor clienti la server
- **Socket**: Folosirea unei liste de socket-uri pentru conevtarea clientilor la server si stabilirea unei conexiuni continue
- **Lucru cu fisiere**: Stocarea in diverse fisiere a datelor despre clienti si organizarea eficienta a folosirii descriptorilor de fisiere
- **Makefile**: folosirea de Makefile pentru partea de setup la instalarea aplicatiei pe un nou dispozitiv
- **Timere**: folosirea de timere pentru a face screenshot-uri la o anumita perioada de timp
## Feature-uri:
- [ ] [[Setup|Setup pentru utilitare]]
- [ ] [[KeyLogger_Feature|KeyLogger]]
- [ ] [[Background exec|Rulare in background ca proces daemon]]
- [ ] [[Screenshots_Feature|Screenshots]] 
- [ ] [[Executare comenzi_Feature|Executare de comenzi pe mai multe calculatoare]]
- [ ] [[Site Blocking_Feature|Alerte Accesare site-uri]]

# To Do
- [x] De facut multi-Threading pentru server
- [ ] Setat nume diferite pentru fiecare fisier de keylogger
- [ ] De facut un setup pentru instalarea utilitarelor necesare (un Makefile)
- [ ] Procesul trebuie facut daemon pentru a rula in permanenta in bg
- [ ] Sa ruleze in momentul cand se da restart
- [ ] KeyLogger va afisa caracterele speciale intr-un format anume(ex: **ESC** va afisa \<*ESC*\> in fisier)
- [ ] De inchis conexiunile cand se avem anumite erori: fisiere, conexiuni etc
- [ ] 

# Utilitare/Biblioteci instalate folosite
Aici vor fi puse utilitarele/Librariile care trebuiesc instalate. Utilitarele instalate vor fi marcate cu #install_util in documentatie cand sunt folosite
- sudo apt install **scrot**
- sudo apt update; sudo apt install **libpcap-dev**
- 

> [!Important]
> Socket-ul serverului pe care se primesc mesajele de la client ar trebui blocat folosind comanda `lock`, astfel putand sa se puna toate cererile de la clienti intr-o coada de asteptare si sa nu fie toate puse gramada.





