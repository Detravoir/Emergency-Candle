#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

const char* ssid_from_AP_1    = "iPhone";  
const char* password_for_AP_1 = "keeskees";     

const int led = 15;

const int greenButtonPin = 13;     // the number of the pushbutton pin
const int yellowButtonPin = 5;     // the number of the pushbutton pin
const int redButtonPin = 14;     // the number of the pushbutton pin

bool ledStatus = false;

int greenButtonState = 0;
int yellowButtonState = 0;
int redButtonState = 0;

void handleRoot();              // function prototypes for HTTP handlers
void handleLED();
void handleNotFound();

void setup(void){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer 
  delay(10);
  Serial.println('\n');

  pinMode(led, OUTPUT);
  
  pinMode(greenButtonPin, INPUT);
  pinMode(yellowButtonPin, INPUT);
  pinMode(redButtonPin, INPUT);

  wifiMulti.addAP(ssid_from_AP_1, password_for_AP_1);   // add Wi-Fi networks you want to connect to

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/LED", HTTP_POST, handleLED);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
  
}

void loop(void){
  server.handleClient();                    // Listen for HTTP requests from clients

  greenButtonState = digitalRead(greenButtonPin);
  yellowButtonState = digitalRead(yellowButtonPin);
  redButtonState = digitalRead(redButtonPin);

  handleWarningCall();
}

void handleWarningCall() {
  String warningStatus = "";
  if (redButtonState == HIGH) {
    warningStatus = "PANIEEEEK";
    Serial.println("paniek");
    server.send(200, "text/html", "<p> " + warningStatus + " </p>");
//    digitalWrite(led, HIGH);
  } 
  if(yellowButtonState == HIGH) {
    warningStatus = "half PANIEEEEK";
    Serial.println("half paniek");
    server.send(200, "text/html", "<p> " + warningStatus + " </p>");
  }
  if(greenButtonState == HIGH) {
    warningStatus = "geen PANIEEEEK";
    Serial.println("geen paniek");
    server.send(200, "text/html", "<p> " + warningStatus + " </p>");
  }
  
//  Serial.println(warningStatus);
}

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
    String statusText = "Led is currently on";

  if(!ledStatus) {
    statusText = "Led is currently off";
  }
  server.send(200, "text/html", "<p> " + statusText + " </p>" + "<form action=\"/LED\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\"></form>");
}

void handleLED() {                          // If a POST request is made to URI /LED
  Serial.println("toggling");
  ledStatus = !ledStatus;
  
  digitalWrite(led,!digitalRead(led));      // Change the state of the LED
  
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
