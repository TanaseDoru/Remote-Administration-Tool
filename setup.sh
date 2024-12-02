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
  

  #Aici obtine informatii despre utilizator
  os_info=$(uname -a)

  keyboard_device=$(grep -E "Handlers|EV=" /proc/bus/input/devices | \
  grep -B1 "EV=120013" | \
  grep -Eo "event[0-9]+" | \
  head -n1)

  if [[ -n $keyboard_device ]]; then
      input_device="/dev/input/$keyboard_device"
  else
      echo "No keyboard input device found. Please check manually."
      exit 1
  fi
  
  # Salvează informatii in fisier de configurare
  config_file="/etc/remote_tool_config.conf"
  sudo bash -c "cat << EOF > $config_file
Nume_Echipament=$pc_name
Keyboard_Input=$input_device
OS_Info=$os_info
EOF"
  echo "Informațiile sistemului au fost salvate în $config_file"
}

# CPornire automata
setup_autostart() {
  echo "Configurare pornire automată..."
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

# Setarea fisier executabil si pornire aplicatie
setup_main_application() {
  echo "Configurare fișier aplicație principală..."
  sudo bash -c "cat << 'EOF' > /usr/local/bin/remote_monitor
#!/bin/bash
echo 'Pornire Remote Admin ' > $HOME/Desktop/fisier
sleep 20
rm $HOME/Desktop/fisier
# Aici adăugați comenzile specifice aplicației dvs.
EOF"
  sudo chmod +x /usr/local/bin/remote_monitor
  echo "Fișierul principal /usr/local/bin/remote_monitor a fost configurat."
}

# Executa functiile
check_if_installed
install_dependencies
set_pc_info
setup_main_application
setup_autostart

echo "Setup complet!"
