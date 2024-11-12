Rularea in fundal va fi prin folosirea unui serviciu de tip `systemd` astfel:
## 1. Setup de fisiere
1. Crearea unui nou fisier de configurare pentru serviciu in directorul `/etc/systemd/system`
   ```bash
   sudo gedit /etc/systemd/system/RAT.service
```
2. In acest fisier vom adauga:
```shell
[Unit]
Description="Remote Admin Tool"
After=network.target

[Service]
ExecStart=/path_catre_executabil # Executabilul trebuie pus intr-un director aparte
WorkingDirectory=/path_catre_directorul_programului # In cazul in care executabilul foloseste alte fisiere din director
StandardOutput=null # Redirectionarea STD_OUT
StandardError=null # Si STD_ERR
Restart=always # Programul se va reporni automat daca se inchide accidental
User=root # utilizatorul si grupul care vor porni procesul
Group=root

[Install]
WantedBy=multi-user.target # Porneste serviciul automat la boot in mod multi-user

```
## 2. Activarea si pornirea serviciului
Activare serviciu:
```shell
sudo systemctl enable RAT.service
```
Pornire Serviciu:
```shell
sudo systemctl start RAT.service

```

## 3. Verificare stare serviciu
```shell
sudo systemctl status RAT.service
```

## 4. Oprire/Repornire Serviciu
Oprire:
```shell
sudo systemctl stop RAT.service
```

Repornire
```Shell
sudo systemctl restart RAT.service
```