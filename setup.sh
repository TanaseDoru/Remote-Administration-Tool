#!/bin/bash

check_if_installed() {
  if [ -f /usr/local/bin/remote_monitor ] || [ -f ~/.config/autostart/remote_monitor.desktop ]; then
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

  # Obține informații despre sistem
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
  
  # Salvează informațiile într-un fișier de configurare
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

# Verifică dacă scriptul este deja rulat cu sudo
if [ \"\$EUID\" -ne 0 ]; then
  echo \"Acest script necesită permisiuni de root. Introduceți parola pentru sudo:\"
  sudo bash -c \"echo 'Permisiuni sudo acordate. Scriptul continuă...'\"
  if [ \$? -ne 0 ]; then
    echo \"Parola sudo incorectă sau utilizatorul a anulat cererea. Ieșire.\"
    exit 1
  fi
fi

# Comenzile care necesită sudo
sudo echo "$(whoami)" > $HOME/Desktop/fisier
EOF"
  sudo chmod +x /usr/local/bin/ask_sudo_password.sh
  echo "Scriptul /usr/local/bin/ask_sudo_password.sh a fost configurat."
}

# Execută funcțiile
check_if_installed
install_dependencies
set_pc_info
setup_sudo_prompt_script
setup_autostart_terminal

echo "Setup complet!"

