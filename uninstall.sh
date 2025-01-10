#!/bin/bash

# Funcție pentru eliminarea fișierelor și configurațiilor
uninstall_application() {
  echo "Încep dezinstalarea aplicației..."

  # Eliminare fișier executabil principal
  if [ -f /usr/local/bin/remote_monitor ]; then
    sudo rm /usr/local/bin/remote_monitor
    echo "Fișierul executabil /usr/local/bin/remote_monitor a fost șters."
  else
    echo "Fișierul executabil /usr/local/bin/remote_monitor nu există."
  fi

  # Eliminare script sudo prompt
  if [ -f /usr/local/bin/ask_sudo_password.sh ]; then
    sudo rm /usr/local/bin/ask_sudo_password.sh
    echo "Scriptul /usr/local/bin/ask_sudo_password.sh a fost șters."
  else
    echo "Scriptul /usr/local/bin/ask_sudo_password.sh nu există."
  fi

  # Eliminare configurații din ~/.config/autostart
  if [ -f ~/.config/autostart/remote_monitor_terminal.desktop ]; then
    rm ~/.config/autostart/remote_monitor_terminal.desktop
    echo "Fișierul de autostart ~/.config/autostart/remote_monitor_terminal.desktop a fost șters."
  else
    echo "Fișierul de autostart ~/.config/autostart/remote_monitor_terminal.desktop nu există."
  fi

  # Eliminare fișier de configurare temporar
  if [ -f /tmp/remote_tool_config.conf ]; then
    rm /tmp/remote_tool_config.conf
    echo "Fișierul temporar de configurare /tmp/remote_tool_config.conf a fost șters."
  else
    echo "Fișierul temporar de configurare /tmp/remote_tool_config.conf nu există."
  fi

  echo "Aplicația a fost dezinstalată cu succes!"
}

# Confirmare înainte de a rula dezinstalarea
read -p "Sunteți sigur că doriți să dezinstalați aplicația Remote Monitor? (y/[n]): " confirm
if [[ "$confirm" =~ ^(da|yes|y)$ ]]; then
  uninstall_application
else
  echo "Dezinstalarea a fost anulată."
fi

