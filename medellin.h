#include <stdio.h>  // console input/output, perror
#include <stdlib.h> // exit
#include <string.h> // string manipulation
#include <netdb.h>  // getnameinfo

#include <sys/socket.h> // socket APIs
#include <netinet/in.h> // sockaddr_in
#include <unistd.h>     // open, close

#include <signal.h> // signal handling
#include <time.h>   // time

void pabloGetFileURL(char *route, char *fileURL)
{
  // if route has parameters, remove them
  char *question = strrchr(route, '?');
  // if (question)
  //   *question = '\0';
  if (question){
    //char *parameter = strtok(route, "?");
    // char parameter = getString(12,20,0,route);
    // printf("\n parameter is: %s",parameter);
  }

  // if route is empty, set it to index.html
  if (route[strlen(route) - 1] == '/')
  {
    strcat(route, "index.html");
  }

  // get filename from route
  strcpy(fileURL, WEBROOT_FOLDER);
  strcat(fileURL, route);

  // if filename does not have an extension, set it to .html
  const char *dot = strrchr(fileURL, '.');
  if (!dot || dot == fileURL)
  {
    strcat(fileURL, ".html");
  }
}

void pabloGetMimeType(char *file, char *mime)
{
  // position in string with period character
  const char *dot = strrchr(file, '.');

  // if period not found, set mime type to text/html
  if (dot == NULL)
    strcpy(mime, "text/html");
  else if (strcmp(dot, ".html") == 0)
    strcpy(mime, "text/html");
  else if (strcmp(dot, ".jpg") == 0)
    strcpy(mime, "image/jpeg");
  else if (strcmp(dot, ".css") == 0)
    strcpy(mime, "text/css"); // support CSS included files in web page
  else if (strcmp(dot, ".js") == 0)
    strcpy(mime, "application/js"); // support Javascript included files in web page
}

void pabloGetTimeString(char *buf)
{
  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}
