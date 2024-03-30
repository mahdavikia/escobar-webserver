/**
 * gcc -Wall -Wextra -g -o escobar escobar.c
 * ./escobar
*/
#include "cali.h"   // Cali is defines header file
#include "medellin.h"   // Medellin is functions header file
#include "los_pepes.h"   // Los Pepes is predefine functions & variables header file

int main()
{

  signal(SIGINT, pabloHandleSignal);

  struct sockaddr_in pabloServerAddress;
  pabloServerAddress.sin_family = AF_INET;                     // IPv4
  pabloServerAddress.sin_port = htons(PORT);                   // port number in network byte order (host-to-network short)
  // pabloServerAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // localhost 
  pabloServerAddress.sin_addr.s_addr = inet_addr(HOST_ADDRESS);

  // socket of type IPv4 using TCP protocol
  pabloServerSocket = socket(AF_INET, SOCK_STREAM, 0);

  // reuse address and port
  setsockopt(pabloServerSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

  // bind socket to address
  if (bind(pabloServerSocket, (struct sockaddr *)&pabloServerAddress, sizeof(pabloServerAddress)) < 0)
  {
    printf("Error: The server is not bound to the address.\n");
    return 1;
  }

  // listen for connections
  if (listen(pabloServerSocket, BACKLOG) < 0)
  {
    printf("Error: The server is not listening.\n");
    return 1;
  }

  // get server address information
  char hostBuffer[NI_MAXHOST], serviceBuffer[NI_MAXSERV];
  int error = getnameinfo((struct sockaddr *)&pabloServerAddress, sizeof(pabloServerAddress), hostBuffer,
                          sizeof(hostBuffer), serviceBuffer, sizeof(serviceBuffer), 0);

  if (error != 0)
  {
    printf("Error: %s\n", gai_strerror(error));
    return 1;
  } 
  printf("\nServer started to listening on http://%s:%s/\n\n", HOST_ADDRESS, PORT_STR);

  while (1)
  {
    // buffer to store data (request)
    pabloRequest = (char *)malloc(SIZE * sizeof(char));
    char method[10], route[100];

    // accept connection and read data
    pabloClientSocket = accept(pabloServerSocket, NULL, NULL);
    read(pabloClientSocket, pabloRequest, SIZE);

    // parse HTTP request
    sscanf(pabloRequest, "%s %s", method, route);
    printf("%s %s", method, route);   

    free(pabloRequest);

    // only support GET method
    if (strcmp(method, "GET") != 0)
    {
      const char response[] = "HTTP/1.1 400 Bad Request\r\n\n";
      send(pabloClientSocket, response, sizeof(response), 0);
    }
    else
    {     
      char fileURL[100];

      // generate file URL
      pabloGetFileURL(route, fileURL);

      // read file
      FILE *file = fopen(fileURL, "r");
      if (!file)
      {
        const char response[] = "HTTP/1.1 404 Not Found\r\n\n";
        send(pabloClientSocket, response, sizeof(response), 0);
      }
      else
      {
        // generate HTTP response header
        char resHeader[SIZE];

        // get current time
        char timeBuf[100];
        pabloGetTimeString(timeBuf);

        // generate mime type from file URL
        char mimeType[32];
        pabloGetMimeType(fileURL, mimeType);

        sprintf(resHeader, "HTTP/1.1 200 OK\r\nDate: %s\r\nContent-Type: %s\r\n\n", timeBuf, mimeType);
        int headerSize = strlen(resHeader);       
        // Calculate file size
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Allocates memory for response buffer and copies response header and file contents to it
        char *resBuffer = (char *)malloc(fsize + headerSize);
        strcpy(resBuffer, resHeader);

        // Starting position of file contents in response buffer
        char *fileBuffer = resBuffer + headerSize;
        fread(fileBuffer, fsize, 1, file);        
        send(pabloClientSocket, resBuffer, fsize + headerSize, 0);
        free(resBuffer);
        fclose(file);
      }
    }
    close(pabloClientSocket);
    printf("\n");
  }
}
void pabloHandleSignal(int signal) 
{
  if (signal == SIGINT) 
  {
    printf("\nShutting down Escobar...\n");

    close(pabloClientSocket); 
    close(pabloServerSocket); 

    if (pabloRequest != NULL) 
      free(pabloRequest); 

    exit(0); 
  }
}