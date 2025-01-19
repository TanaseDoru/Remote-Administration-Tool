# Implementare

- **Verificarea si instalarea dependintelor**: Scriptul va detecta daca toate utilitatile necesare  sunt prezente si le va instala daca lipsesc. Acest lucru se poate face prin comenzi _Bash_ direct in script, folosind gestiunea de pachete .
    
- **Setarea numelui echipamentului**: In timpul setup-ului, scriptul va intreba utilizatorul pentru un nume personalizat pentru echipament. In cazul in care utilizatorul nu furnizeaza un nume, setup-ul va aloca automat un nume implicit de tipul `PC1`, `PC2`, etc., stocat intr-un fisier de configurare.
    
- **Configuratia pornirii automate**: Scriptul va crea o regula de pornire automata, creand un fisier de autostart pentru acesta
    
- **Salvarea configuratiei**: La finalul setup-ului, numele echipamentului si alte detalii esentiale vor fi salvate intr-un fisier de configurare (`.conf`). Acest fisier va putea fi accesat de programul de monitorizare pentru a afisa si utiliza corect datele.

Code snippet:
```bash
#!/bin/bash

  

check_if_installed() {

if [ -f /tmp/remote_tool_config.conf ]; then

echo "Aplicația Remote Monitor este deja instalată."

exit 0

fi

}

  

install_dependencies() {

echo "Verific și instalez dependențele necesare..."

for package in curl wget python3 scrot libpcap-dev; do

if ! command -v $package &> /dev/null; then

echo "Instalare $package..."

sudo apt-get install -y $package

else

echo "$package este deja instalat."

fi

done

}

  

set_pc_info() {

read -p "Introduceți un nume pentru echipament (Implicit Nume Aleator): " pc_name

pc_name=${pc_name:-"PC$((RANDOM % 1000 + 1))"}

echo "Nume echipament: $pc_name"

  

os_info=$(uname -a)

  

keyboard_device=$(grep -E "Handlers|EV=" /proc/bus/input/devices | \

grep -B1 "EV=120013" | \

grep -Eo "event[0-9]+" | \

head -n1)

  

if [[ -n $keyboard_device ]]; then

input_device="/dev/input/$keyboard_device"

else

echo "Nu a fost găsit un dispozitiv de intrare pentru tastatură. Verificați manual."

exit 1

fi

config_file="/tmp/remote_tool_config.conf"

cat << EOF > $config_file

Nume_Echipament=$pc_name

Keyboard_Input=$input_device

OS_Info=$os_info

EOF

echo "Informațiile sistemului au fost salvate temporar în $config_file."

}

  

setup_autostart_terminal() {

echo "Configurare pornire automată a unui terminal..."

mkdir -p ~/.config/autostart

cat << EOF > ~/.config/autostart/remote_monitor_terminal.desktop

[Desktop Entry]

Type=Application

Exec=gnome-terminal -- bash -c "/usr/local/bin/ask_sudo_password.sh; bash"

Hidden=false

NoDisplay=false

X-GNOME-Autostart-enabled=true

Name=Remote Monitor Terminal

Comment=Solicită parola sudo la autentificare pentru Remote Monitor.

EOF

echo "Un terminal va porni automat la autentificare pentru a solicita parola sudo."

}

  

setup_sudo_prompt_script() {

echo "Configurare script pentru cererea parolei sudo..."

sudo bash -c "cat << 'EOF' > /usr/local/bin/ask_sudo_password.sh

#!/bin/bash

  
  

if [ \"\$EUID\" -ne 0 ]; then

echo \"Acest script necesită permisiuni de root. Introduceți parola pentru sudo:\"

sudo bash -c \"echo 'Permisiuni sudo acordate. Scriptul continuă...'\"

if [ \$? -ne 0 ]; then

echo \"Parola sudo incorectă sau utilizatorul a anulat cererea. Ieșire.\"

exit 1

fi

if [ -e ~/Desktop/3_Anul\ III/Remote-Administration-Tool/client ]; then

sudo ~/Desktop/3_Anul\ III/Remote-Administration-Tool/client

fi

  

fi

EOF"

sudo chmod +x /usr/local/bin/ask_sudo_password.sh

echo "Scriptul /usr/local/bin/ask_sudo_password.sh a fost configurat."

}

  
  

check_if_installed

install_dependencies

set_pc_info

setup_sudo_prompt_script

setup_autostart_terminal

  

echo "Setup complet!"

```






























