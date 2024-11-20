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

  # Eliminare configurații din ~/.config/autostart
  if [ -f ~/.config/autostart/remote_monitor.desktop ]; then
    rm ~/.config/autostart/remote_monitor.desktop
    echo "Fișierul de autostart ~/.config/autostart/remote_monitor.desktop a fost șters."
  else
    echo "Fișierul de autostart ~/.config/autostart/remote_monitor.desktop nu există."
  fi

  # Eliminare fișier de configurare globală
  if [ -f /etc/remote_tool_config.conf ]; then
    sudo rm /etc/remote_tool_config.conf
    echo "Fișierul de configurare /etc/remote_tool_config.conf a fost șters."
  else
    echo "Fișierul de configurare /etc/remote_tool_config.conf nu există."
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
