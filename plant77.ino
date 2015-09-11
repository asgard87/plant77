#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   20

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 77); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80
File webFile;

char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer

String filename;

int analog_pin = 0;
float temperatura;

void setup()
{
    Serial.begin(9600);
    //while (!Serial) ;        // Retraso para el Leonardo
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("Get Start!");
    Ethernet.begin(mac, ip);
    server.begin();
}

void loop()
{
  //char clientline[BUFSIZ];
  //int index = 0;
  
  EthernetClient client = server.available();
  
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
 
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (req_index < (REQ_BUF_SZ - 1)) {
            HTTP_req[req_index] = c;          // save HTTP request character
            req_index++;
        }
        //Serial.print(c);    // print HTTP request character to serial monitor
        if (c == '\n' && currentLineIsBlank) {

             // got a \n or \r new line, which means the string is done
            //clientline[index] = 0;
     
            // Print it out for debugging
            //Serial.println(clientline);
            // send a standard http response header
            
            // open requested web page file
            if (StrContains(HTTP_req, "GET / ") || StrContains(HTTP_req, ".htm")) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: keep-alive");
              client.println();
              webFile = SD.open("index2.htm");        // open web page file
            }else if (StrContains(HTTP_req, "ajax_data")) {
              // read switch state and send appropriate paragraph text
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: keep-alive");
              client.println();
              GetState(client);
            }else{
                getFile(HTTP_req);
                Serial.println(filename);
                webFile = SD.open(filename, FILE_READ);
                if (webFile) {
                    client.println("HTTP/1.1 200 OK");
                    //client.println("Content-Type: text/" + ext);
                    //client.println("Connection: keep-alive");
                    client.println();
                }
            }
            if (webFile) {
                while(webFile.available()) {
                    client.write(webFile.read()); // send web page to client
                }
                webFile.close();
            }
            req_index = 0;
            StrClear(HTTP_req, REQ_BUF_SZ);
            break;
        }

        // every line of text received from the client ends with \r\n
        if (c == '\n') {
            // last character on line of received text
            // starting new line with next character read
            currentLineIsBlank = true;
        } 
        else if (c != '\r') {
            // a text character was received from client
            currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}
void getFile(char *data){
  char *pch;
  filename = "";
  pch=strrchr(data,'.');
  for (int i = 5; i < pch-data+4; i++) {
    filename[i-5] += data[i];
  }
  /*for (int i = pch-data+1; i < pch-data+4; i++) {
     ext += data[i];
  }*/
}

void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}

void GetState(EthernetClient cl)
{
    //Serial.println("Enviando datos...");
    temperatura = analogRead(analog_pin);
    temperatura = (5.0*temperatura*100.0)/1024.0;
    cl.print(temperatura);
    cl.println(" °C");
}

