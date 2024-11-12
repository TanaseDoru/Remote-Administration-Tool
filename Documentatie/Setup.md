# Implementare

- **Verificarea si instalarea dependintelor**: Scriptul va detecta daca toate utilitatile necesare  sunt prezente si le va instala daca lipsesc. Acest lucru se poate face prin comenzi _Bash_ direct in _Makefile_ sau in script, folosind gestiunea de pachete .
    
- **Setarea numelui echipamentului**: In timpul setup-ului, scriptul va intreba utilizatorul pentru un nume personalizat pentru echipament. In cazul in care utilizatorul nu furnizeaza un nume, setup-ul va aloca automat un nume implicit de tipul `PC1`, `PC2`, etc., stocat intr-un fisier de configurare.
    
- **Configuratia pornirii automate**: Scriptul va crea o regula de pornire automata, adaugand fie un fisier de tip `.desktop` in directorul de autostart (pentru sisteme Linux), fie o cheie de registry (pentru Windows), astfel incat aplicatia de monitorizare sa fie lansata automat la fiecare repornire a sistemului.
    
- **Salvarea configuratiei**: La finalul setup-ului, numele echipamentului si alte detalii esentiale vor fi salvate intr-un fisier de configurare (ex: `config.json` sau `.conf`). Acest fisier va putea fi accesat de programul de monitorizare pentru a afisa si utiliza corect datele.

Code snippet:
```bash
#!/bin/bash

#Verifica si instaleaza dependinte
install_dependencies() {
  for package in curl wget python3; do
    if ! command -v $package &> /dev/null; then
      echo "Instalare $package..."
      sudo apt-get install -y $package
    fi
  done
}

#Seteaza numele echipamentului
set_pc_name() {
  read -p "Introduceti un nume pentru echipament (implicit: PC$((RANDOM % 1000 + 1))): " pc_name
  pc_name=${pc_name:-"PC$((RANDOM % 1000 + 1))"}
  echo "Nume echipament: $pc_name"
  echo "$pc_name" > /etc/remote_tool_config.conf
}

# Configuratia pentru pornire automata
setup_autostart() {
  mkdir -p ~/.config/autostart
  cat << EOF > ~/.config/autostart/remote_monitor.desktop
[Desktop Entry]
Type=Application
Exec=/usr/local/bin/remote_monitor
Hidden=false
NoDisplay=false
X-GNOME-Autostart-enabled=true
Name=Remote Monitor
EOF
  echo "Programul de monitorizare va porni automat la fiecare deschidere."
}

# Executa functiile
install_dependencies
set_pc_name
setup_autostart

echo "Setup complet!"

```






























