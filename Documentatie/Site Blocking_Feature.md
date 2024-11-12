1- Pentru realizarea acestei functionalitati trebuie mai intai sa facem monitorizarea traficiului de retea
-  Verificarea domeniilor blocate: Se vor lua domeniile dintr-o lista. Daca site-ul este in lista, atucni se va trimite o alerta si se va bloca conexiunea
Pentru monitorizarea traficului de retea si blocarea site-urilor interzise vom folosi biblioteca **libcap-dev**

## Exemplu Cod
```c
const char *blocked_sites[MAX_SITES] = { "www.wikipedia.com", "wiki.mta.ro",
}

...

void process_packet(...)
{
	//Extragere pachet din placa de retea a echipamentului
	printf("Host accessed: %s\n", host);

	if (is_blocked_site(host))
	{
		pritnf("Alert: Site is blocked: %s", host);
		char command[256];
		snprintf(command, sizeof(command), "sudo iptables -A OUTPUT -p tcp --dport 80 -d %s -j REJECT", host); //executare comenzi de blocare a site-ului
		system(command);// 80 -> HTTP
		snprintf(command, sizeof(command), "sudo iptables -A OUTPUT -p tcp --dport 443 -d %s -j REJECT", host);//443 -> HTTPS
		system(command);
		printf("Blocked connection to %s\n", host);
	}
}

```

> [!WARNING]
> Trebuie instala libraria libpcap #install_util 

