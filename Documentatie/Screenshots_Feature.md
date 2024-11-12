Screenshot-urile vor fi facute folosind utilitarul *scrot*(implementarea in `c` va fi folosind functia **system()**)
- Mai intai server-ul va face o cerere clientului de screenshot, iar clientul va face aceasta comanda si o va trimite server-ului.
- Utilitarul **scrot** va face screenshot la intreg ecranul clientului, iar print comanda *--file* se va specifica numele fisierului.
- Dupa transmiterea imaginii, programul va sterge poza de pe dispozitivul clientului
- Cererea de la server la client se va face folosind un **timer** de notificari la o periodata fixata de timp

>[!WARNING]
> utilitarul **scrot** trebuie instalat #install_util 


