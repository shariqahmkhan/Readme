/*
**************************************
*********program flow*****************
*arduino as AP
*user goes online and enter ssid and password
*it gets saved in EEPROM
*and setup retrieves from EEPROM
*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

ESP8266WebServer server(80);

IPAddress local_IP (192,168,4,22);
IPAddress gateway (192,168,4,22);
IPAddress subnet (255,255,255,0);

const char *ssid = "vivo Y51L";   //enter your ssid here (for sta mode)
const char *password = "shariqkhan";  //enter password (sta)
const char *myssid = "esp8266 wifi";  //enter ssid (ap)
const char *mypassword = "shariqkhan29!!";  //enter password
String st;
String content;
int statusCode;

bool testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");  
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) { return true; } 
    delay(500);
    Serial.print(WiFi.status());    
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
} 
void launchWeb(int webtype) {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer(webtype);
  // Start the server
  server.begin();
  Serial.println("Server started"); 
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  }
  Serial.println(""); 
  st = "<ol>";
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st += "<li>";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ol>";
  delay(100);
  WiFi.softAP(myssid, mypassword, 6);
  Serial.println("softap");
  launchWeb(1);
  Serial.println("over");
}

void createWebServer(int webtype)
{
  if ( webtype == 1 ) {
    server.on("/", []() {
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
        content += ipStr;
        content += "<p>";
        content += st;
        content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
        content += "</html>";
        server.send(200, "text/html", content);  
    });
    server.on("/setting", []() {
        String qsid = server.arg("ssid");
        String qpass = server.arg("pass");
        if (qsid.length() > 0 && qpass.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
          Serial.println(qsid);
          Serial.println("");
          Serial.println(qpass);
          Serial.println("");
            
          Serial.println("writing eeprom ssid:");
          for (int i = 0; i < qsid.length(); ++i)
            {
              EEPROM.write(i, qsid[i]);
              Serial.print("Wrote: ");
              Serial.println(qsid[i]); 
            }
          Serial.println("writing eeprom pass:"); 
          for (int i = 0; i < qpass.length(); ++i)
            {
              EEPROM.write(32+i, qpass[i]);
              Serial.print("Wrote: ");
              Serial.println(qpass[i]); 
            }    
          EEPROM.commit();
          content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
          statusCode = 200;
        } else {
          content = "{\"Error\":\"404 not found\"}";
          statusCode = 404;
          Serial.println("Sending 404");
        }
        server.send(statusCode, "application/json", content);
    });
  } 
  else if (webtype == 0) {
    server.on("/", []() {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      server.send(200, "application/json", "{\"IP\":\"" + ipStr + "\"}");
    });
    server.on("/cleareeprom", []() {
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Clearing the EEPROM</p></html>";
      server.send(200, "text/html", content);
      Serial.println("clearing eeprom");
      for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
      EEPROM.commit();
    });
  }
}

void setup()
{
  
  Serial.begin(115200);
  //WiFi.mode(WIFI_AP_STA);
  EEPROM.begin(512);
  delay(2000);
  Serial.print("***********************The setup has started******************");
  delay(1000);
  Serial.println("Process flow of setup\n 1) set on STA mode and begin with ssid and password\n 2) print ssid\n 3) Persistent and autoconnect is off so wifi does not recognize old set credentials\n 4) server on, begin server");
  delay(1000);
  //WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
 // WiFi.softAPConfig(local_IP, gateway, subnet);  //method of defining a router/access point
 // WiFi.softAP(myssid, mypassword);
  delay(1000);
  Serial.print("reading EEPROM memory");
  
    //server.handleClient();
    delay(1000);
    Serial.println("connection lost!");
    String new_ssid;
  for (int i = 0; i < 32; ++i)  //pre-increment
  {
    new_ssid += char(EEPROM.read(i));
  }
  Serial.println("SSID:");
  Serial.println(new_ssid);

  String new_password;
  Serial.println("reading new_password");
  for (int i = 32; i < 96; ++i)
  {
    new_password += char(EEPROM.read(i));
  }
  Serial.print("new Password:");
  Serial.print(new_password);
  //condition to make ssid and passowrd
  if( new_ssid.length() > 1)
  {
    WiFi.begin(new_ssid.c_str(), new_password.c_str());
    if (testWifi())
    {
      launchWeb(0);
      return;
    }
  }
  setupAP();
  
  
  WiFi.persistent(false);
  delay(500);
  WiFi.setAutoConnect(false);
}


void loop() {

 if (WiFi.status() == WL_CONNECTED)
  {
    delay(1000);
    //WiFi.setAutoConnect(false);
    delay(1000);
    Serial.println("connection exist");
    Serial.println(WiFi.SSID());
    //perform the task of sending 
  }
  //else
  else
  {
   
    for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
   server.handleClient(); 
   
    }
   
}
