//nodeMCU (ESP8266)
/*
 * this project sets a nodeMCU as server.
 * being the central hub, its intended functions include: 
 * 1. monitoring room temp and device status
 * 2. operation of light and fan of 1 room
 */

#include "FS.h"
#include "ESP8266WiFi.h"

//global variables
File ssid;
WiFiServer server(80);  //creating server object of class WiFiServer listening on port 80

void format()
{
  SPIFFS.begin();
  Serial.println();
  Serial.println("please wait 30s for SPIFFS to be formatted"); //printed garbage
  SPIFFS.format();
  Serial.println("SPIFFS formatted"); //printed correctly
}

void server_setup()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP("IoT", "12345678"); //AP name and default password set
  server.begin();                 //start the server
  IPAddress HTTPS_ServerIP= WiFi.softAPIP(); // Obtain the IP of the Server 
  Serial.print("Server IP is: ");  Serial.println(HTTPS_ServerIP);
}

//=========================================================================================
  const unsigned int gpio5 = 5;       //light (D1)
  const unsigned int gpio4 = 4;       //fan   (D2)
  String gpio5_state = "off";
  String gpio4_state = "off";
//=========================================================================================

void setup()
{
  Serial.begin(115200);
  format();             //format any pre-existing files in flash memory
  
  //setup pins and strings to display status of gpio on webpage
  pinMode(gpio5, OUTPUT);
  pinMode(gpio4, OUTPUT);
  pinMode(A0, INPUT);
  
  server_setup();       //setup the nodeMCU as a server
  Serial.println("wifi and server setup done \n");
}

void server_actions()
{
  WiFiClient client = server.available();
  String header;
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              gpio5_state = "on";
              digitalWrite(gpio5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              gpio5_state = "off";
              digitalWrite(gpio5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              gpio4_state = "on";
              digitalWrite(gpio4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              gpio4_state = "off";
              digitalWrite(gpio4, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + gpio5_state + "</p>");
            // If the output5State is off, it displays the ON button       
            if (gpio5_state=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + gpio4_state + "</p>");
            // If the output4State is off, it displays the ON button       
            if (gpio4_state=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.print("proximity reading: "); client.println(analogRead(A0));
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void loop() 
{
  //  Serial.print("proximity reading: "); Serial.println(analogRead(A0));  
  //printing continously hampering webserver call
  server_actions(); 
}
