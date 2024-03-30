void pabloGetFileURL(char *route, char *fileURL);
void pabloGetMimeType(char *file, char *mime);
void pabloHandleSignal(int signal);
void pabloGetTimeString(char *buf);

int pabloServerSocket;
int pabloClientSocket;

char *pabloRequest;