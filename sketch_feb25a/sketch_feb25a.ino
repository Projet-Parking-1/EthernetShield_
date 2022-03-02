#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h> 
#include <SPI.h>
#include <Ethernet.h>

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

 boolean currentLineIsBlank;
 String req_str;
 int data_length;
 char c;

 int angle1;
 int angle2;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xA8, 0x61, 0x0A, 0xAE, 0x74, 0x85
};

EthernetServer server(80);
EthernetClient client;
Servo monservo1;  // création d’un objet servo
Servo monservo2;  // création d’un objet servo

void setup() {
  lcd.begin(16, 2);

   lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Demarrage ^_^ <3");
  delay(2000);
  lcd.clear();
  delay(1000);
  lcd.setCursor(0,0);
    // Print a message to the LCD.
   lcd.print("Adresse IP");

   delay(1000);
    
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  monservo1.attach(6);
  monservo2.attach(9);
  delay(2000);
  monservo1.write(angle2);
  monservo2.write(angle2);
  // start the Ethernet connection:
  Serial.println("Initialisation Ethernet avec DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Echec de la configuration d'Ethernet avec le DHCP");
    
    lcd.setCursor(0, 1);
    lcd.print("Erreur 1");
    
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Le shield Ethernet n'a pas été détecté.");

      lcd.setCursor(0, 1);
      lcd.print("Erreur 2");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Le cable Ethernet n'est pas branché.");

      lcd.setCursor(0, 1);
      lcd.print("Erreur 3");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  // print your local IP address:
  Serial.print("Mon addresse IP: ");
  Serial.println(Ethernet.localIP());

  lcd.setCursor(0, 1);
  lcd.print(Ethernet.localIP());

  Ethernet.begin(mac, Ethernet.localIP());
  server.begin();
}


  void writeResponse(EthernetClient client) {
    // send a standard http response header
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");  // the connection will be closed after completion of the response

    client.println();

    client.println("<html>"
                   "<head>"                    
                     "<meta charset=UTF-8 />"
                     "<script>" 
                      "function ouvrir_barriere1() {"
                      "var xhr = new XMLHttpRequest();"
                      "xhr.open('POST', '/b1=1', true);"
                      "xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"
                      "xhr.onreadystatechange = function() { "
                        "if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {"
                        "var response_server = xhr.responseText;"
                        "var extract_response = response_server.substring(0, 6);"
                        "document.getElementById('etat_b1').innerHTML = 'Dernière action : ' + '<font color=' + 'green' + '>' + extract_response + '</font>';"
                      "}"
                     "}"
                     "\n"
                     "xhr.send();"
                     "}"
                     "function fermer_barriere1() {"
                      "var xhr = new XMLHttpRequest();"
                      "xhr.open('POST', '/b1=0', true);"
                      "xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"
                      "xhr.onreadystatechange = function() { "
                        "if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {"
                        "var response_server = xhr.responseText;"
                        "var extract_response = response_server.substring(0, 6);"
                        "document.getElementById('etat_b1').innerHTML = 'Dernière action : ' + '<font color=' + 'red' + '>' + extract_response + '</font>';"
                      "}"
                     "}"
                     "\n"
                     "xhr.send();"
                     "}"
                     "function ouvrir_barriere2() {"
                      "var xhr = new XMLHttpRequest();"
                      "xhr.open('POST', '/b2=1', true);"
                      "xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"
                      "xhr.onreadystatechange = function() { "
                        "if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {"
                        "var response_server = xhr.responseText;"
                        "var extract_response = response_server.substring(0, 6);"
                        "document.getElementById('etat_b2').innerHTML = 'Dernière action : ' + '<font color=' + 'green' + '>' + extract_response + '</font>';"
                      "}"
                     "}"
                     "\n"
                     "xhr.send();"
                     "}"
                     "function fermer_barriere2() {"
                      "var xhr = new XMLHttpRequest();"
                      "xhr.open('POST', '/b2=0', true);"
                      "xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"
                      "xhr.onreadystatechange = function() { "
                        "if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {"
                        "var response_server = xhr.responseText;"
                        "var extract_response = response_server.substring(0, 6);"
                        "document.getElementById('etat_b2').innerHTML = 'Dernière action : ' + '<font color=' + 'red' + '>' + extract_response + '</font>';"
                      "}"
                     "}"
                     "\n"
                     "xhr.send();"
                     "}"
                     "</script>"
                    "</head>"
                     "<header>"
                        "<h2>Interface dadministration</h2>" 
                    "</header>"
                   "<body>"
                      "<p>Barrière d'entrée :  <button onclick='ouvrir_barriere1()'>Ouvrir</button> <button onclick='fermer_barriere1()'>Fermer</button> <span id='etat_b1'></span> </p>"
                      "<p>Barrière de sortie : <button onclick='ouvrir_barriere2()'>Ouvrir</button> <button onclick='fermer_barriere2()'>Fermer</button> <span id='etat_b2'></span> </p>"  
                    "</body>"
                   "</html>"
                     );         
  }

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.print("new client [");
    
    // an http request ends with a blank line
    currentLineIsBlank = true;
    req_str = "";
    data_length = -1;
   
    //int empty_line_count = 0;
    if (client.connected()) {
      //if (client.available()) {
        char c = client.read();
        while (c != '\n') {
          req_str += c;
          c = client.read();
          }
        //req_str += c;

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply       
         Serial.println("char read");
         Serial.println(req_str);
        if (c == '\n' && currentLineIsBlank && req_str.startsWith("POST")) {
          Serial.println("method POST");

          String content_length = req_str.substring(req_str.indexOf("Content-Length:"));
          Serial.print("Content-Length=");
          Serial.println(content_length);
          content_length.trim();
         // Serial.print("Content-Length=");
         // Serial.println(content_length);
          data_length = content_length.toInt();

          while(data_length-- > 0) {
            c = client.read();
            req_str += c;
          }
          Serial.println(req_str);
          
          if (req_str.indexOf("b1=1") > 0) {
            Serial.println("Barrière d'entrée ouverte");
            server.write("ouvrir");
            //server.send(200, "text/plain", "allee sur off");
            angle1= monservo1.read();
            while (angle1 <90) {
              angle1 = angle1 +1;
              monservo1.write(angle1);
               delay(50);   
            }
            while (angle1 >90) {
              angle1 = angle1 -1;
              monservo1.write(angle1);
              delay(100);   
            }
             
          }
          if (req_str.indexOf("b1=0") > 0) {
           // Serial.println("Barrière d'entrée fermée");
           server.write("fermer");
           angle1= monservo1.read();
            while (angle1 >0) {
              angle1 = angle1 -1;
              monservo1.write(angle1);
              delay(100);   
            }
          }
          if (req_str.indexOf("b2=1") > 0) {
           // Serial.println("Barrière de sortie ouverte");
           server.write("ouvrir");
           angle2= monservo2.read();
            while (angle2 <90) {
              angle2 = angle2 +1;
              monservo2.write(angle2);
               delay(100);   
            }
            while (angle2 >90) {
              angle2 = angle2 -1;
              monservo2.write(angle2);
              delay(100);   
            }
          }
          if (req_str.indexOf("b2=0") > 0) {
           // Serial.println("Barrière de sortie fermée");
           server.write("fermer");
           angle2= monservo2.read();
           
            while (angle2 >0) {
              angle2 = angle2 -1;
              monservo2.write(angle2);
              delay(15);   
            }
          }
          
          }
          writeResponse(client);
         // break;
      //  }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
     delay(1);
     client.stop(); 
    }

    // give the web browser time to receive the data
   // delay(1);
    // close the connection:
  //  client.stop();
   // Serial.println("client disconnected");
  }
