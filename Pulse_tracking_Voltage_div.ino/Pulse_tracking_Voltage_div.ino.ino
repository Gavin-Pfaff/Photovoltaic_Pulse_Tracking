#define loop_count 1000

#include <WiFiS3.h>


#include "arduino_secrets.h"
IPAddress ip;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;

int wifiStatus = WL_IDLE_STATUS;
WiFiServer server(80);


// #include "WIFI_STAGING.ino"  check if is needed??
float volts_2 = 0;

int readings[loop_count];
unsigned long timestamps[loop_count];

//capture and server client search conditions
bool captured = false;
bool server_staged = false;

void Connect_to_WiFi(){
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communcation with Wifi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmmware");
  }

  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID:");
    Serial.println(ssid);

    wifiStatus = WiFi.begin(ssid, pass);

    delay(10000);
    }

    server.begin(); //Server open on port 80

    //printWiFiStatus();
  }

//might have to code this into loop? capture then needs to be triggered separately
void Server_up() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client"); //HTTP req ends w/ blank line
    boolean currentLineIsBlank = true;
    while (client.connected()){
      char c;
      if (client.available()){
        char c = client.read();
        Serial.write(c); // newline character indicates end request, so we can reply

        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html"); 
          client.println("Connection: close");  //connection close after request complete
          client.println("Refresh: 5");   //refresh page every 5 sec
          client.println();
          //client.println("<!DOCTYPE HTML>");
          //client.println("<html>");
          // output values
          client.println("Microseconds,ADC,Voltage");

          for(int i = 0; i < loop_count; i++) {
            //client.println(i);
            client.print(timestamps[i]);
            client.print(",");
            client.print(readings[i]);
            client.print(",");
            client.println((readings[i]*3.3)/16384.0, 4);
            //client.print(",");
           }
        
          //client.println("</html>");
          break;
        }
        if (c == '\n') {
          // start new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    //give some downtime and close out connection
    delay(1);

    client.stop();
    Serial.println("client disconnected");
    server_staged = false;

  }
}



void setup() {
  
  while (!Serial) {
    ;
  }
  Connect_to_WiFi();
  //default IP 192.198.4.1

  // put your setup code here, to run once:
  Serial.begin(9600);
  analogReadResolution(14);
  volts_2 = (analogRead(A0)*3.3)/16384.0;
}

void printCSV(){
  Serial.println("Microseconds,ADC,Voltage");

  for(int i = 0; i < loop_count; i++) {
    //Serial.println(i);
   // Serial.print(timestamps[i]);
   // Serial.println(",");
   // Serial.print(readings[i]);
   // Serial.println(",");
    Serial.println((readings[i]*3.3)/16384.0, 4);
   // Serial.println(",");
  }
}



void capture(){
  //Serial.println("loop Begin");
  unsigned long start_time = micros();
  for(int i = 0; i < loop_count; i++){
    timestamps[i] = micros() - start_time;
    readings[i] = analogRead(A0);
    //delayMicroseconds(2);
  }
  captured = true;
  
  Serial.println("Capture complete! Press 's' to begin searching for connection or 'r' to reset");
}

void loop() {

//float volts = (analogRead(A0)*3.3)/16384.0;
int volts = analogRead(A0);
//Serial.println(volts);
if ((volts) >= 250) {
  capture();
  }

//volts_2 = volts;
//delayMicroseconds(2);



  if(captured){
    if(Serial.available()){
      server_staged = true;
      char cmd = Serial.read();
      if (char cmd = 's'){
        Serial.println("Searching...");
        while (server_staged) {
        Server_up();
        }
      } else if(char cmd = 'r'){
        captured = false;
        server_staged = false;
        Serial.println("ready for next observation");
      }
    }
    //Serial.println("Packet Delivered -- Resetting...");
    //server_staged = false;
    return;
  }

//ip = WiFi.localIP();
//Serial.println(ip);
}
//currently too fast of observations to have a meaningful differential to trigger the if statement from
//lets look into memory of the last 100 observations and do a smoothed comp?


