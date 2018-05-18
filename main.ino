#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

const char* ssid = "YOU_PHONE_WI-FI_SSID";    //SSID Wi-Fi AP
const char* password = "*********";           //PWD
const char* host = "your.message_server.com"; //Message server 

String line;
static  boolean bRestart = true;
int offset=1,refresh=0;
int pinCS = 0; // CS PIN
int numberOfHorizontalDisplays = 4; //horizontal LED module's
int numberOfVerticalDisplays = 1; //vertical LED module's
String decodedMsg;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
//matrix.cp437(true);

int wait = 50; // String Speed
int spacer = 2;
int width = 5 + spacer; // Letters spacer

// ========================TIME========================

#define MAX_DIGITS 16
byte dig[MAX_DIGITS]={0};
byte digold[MAX_DIGITS]={0};
byte digtrans[MAX_DIGITS]={0};
int updCnt = 0;
int dots = 0;
long dotTime = 0;
long clkTime = 0;
int dx=0;
int dy=0;
byte del=0;
int h,m,s;
String date;

void setup() {
matrix.setIntensity(13); // MATRIX Light 1 - 15

// Start matrix coordinate's 8*8
  matrix.setRotation(0, 1);        // 1 matrix
  matrix.setRotation(1, 1);        // 2 matrix
  matrix.setRotation(2, 1);        // 3 matrix
  matrix.setRotation(3, 1);        // 4 matrix

  
Serial.begin(115200);
delay(10);
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.println("");
Serial.println("WiFi connected"); 
Serial.println("IP address: ");
Serial.println(WiFi.localIP());


delay(3000);
ScrollText(utf8rus("Setup is over, system started... "));

}

void loop() {

delay(5000);
Serial.print("connecting to ");
Serial.println(host);
WiFiClient client;
const int httpPort = 80;
if (!client.connect(host, httpPort)) {
Serial.println("connection failed");
return;
}

unsigned long rnd = random(1000,9999);

String url = "/PATH/MESSAGE_FILE_OR_SCRIPT?rnd="; //PATH TO FILE WHITH MESSAGE (like: "/script.php?rnd=")
url += rnd;

//Serial.print("Requesting URL: ");
//Serial.println(url);

client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" + 
"Connection: close\r\n\r\n");
unsigned long timeout = millis();
while (client.available() == 0) {
if (millis() - timeout > 5000) {
Serial.println("»> Client Timeout !");
client.stop();
return;
}
}

String msg = "";
while(client.available()){
String lines = client.readStringUntil('\r');
msg +=lines;                 
}

//Serial.print(msg);

String result = getValue(msg,'\r\n',7);
Serial.print(result);

if(result.substring(0, 4)=="LED:") //MESSAGE TRIGGER IS "LED:" !!!
              {
                  Serial.println("Server Data catch");
                  String  code = result.substring(4);
                  Serial.println("LED DATA:" + code);
                  Serial.println("OK");
                  Serial.println();
                  ScrollText(utf8rus(code));// Display Message after trigger LED:"HELLO=)"
              }
              else
                {
                    Serial.println("Trigger LED: not found, send Time.");
                    Serial.println();
                    getTime();
                    DisplayTime(); //TIME                 
                 }
//Serial.println();
//Serial.println("closing connection");
//send to LED Matrix
}


void ScrollText (String text){
    for ( int i = 0 ; i < width * text.length() + matrix.width() - 1 - spacer; i++ ) {
    if (refresh==1) i=0;
    refresh=0;
    matrix.fillScreen(LOW);
    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2;
 
    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < text.length() ) {
        matrix.drawChar(x, y, text[letter], HIGH, LOW, 1);
      }
      letter--;
      x -= width;
    }
    matrix.write();
    delay(wait);
  }
}

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30-1;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70-1;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}

void DisplayTime(){
    updateTime();
    matrix.fillScreen(LOW);
    int y = (matrix.height() - 8) / 2; 

    /* ":" ==> Hider
    if(s & 1){matrix.drawChar(14, y, (String(":"))[0], HIGH, LOW, 1);} 
    else{matrix.drawChar(14, y, (String(" "))[0], HIGH, LOW, 1);}
    */
    
    matrix.drawChar(14, y, (String(":"))[0], HIGH, LOW, 1);
    
    String hour1 = String (h/10);
    String hour2 = String (h%10);
    String min1 = String (m/10);
    String min2 = String (m%10);
    String sec1 = String (s/10);
    String sec2 = String (s%10);
    int xh = 2;
    int xm = 19;
//    int xs = 28;

    matrix.drawChar(xh, y, hour1[0], HIGH, LOW, 1);
    matrix.drawChar(xh+6, y, hour2[0], HIGH, LOW, 1);
    matrix.drawChar(xm, y, min1[0], HIGH, LOW, 1);
    matrix.drawChar(xm+6, y, min2[0], HIGH, LOW, 1);
//    matrix.drawChar(xs, y, sec1[0], HIGH, LOW, 1);
//    matrix.drawChar(xs+6, y, sec2[0], HIGH, LOW, 1);  


  
    matrix.write(); 
}


// =======================================================================
// get GOOGLE time
// =======================================================================

float utcOffset = 3; //Moscow +3
long localEpoc = 0;
long localMillisAtUpdate = 0;

void getTime()
{
  WiFiClient client;
  if (!client.connect("www.google.com", 80)) {
    Serial.println("connection to google failed");
    return;
  }

  client.print(String("GET / HTTP/1.1\r\n") +
               String("Host: www.google.com\r\n") +
               String("Connection: close\r\n\r\n"));
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    //Serial.println(".");
    repeatCounter++;
  }

  String line;
  client.setNoDelay(false);
  while(client.connected() && client.available()) {
    line = client.readStringUntil('\n');
    line.toUpperCase();
    if (line.startsWith("DATE: ")) {
      date = "     "+line.substring(6, 22);
      h = line.substring(23, 25).toInt();
      m = line.substring(26, 28).toInt();
      s = line.substring(29, 31).toInt();
      localMillisAtUpdate = millis();
      localEpoc = (h * 60 * 60 + m * 60 + s);
    }
  }
  client.stop();
}

// =======================================================================r

void updateTime()
{
  long curEpoch = localEpoc + ((millis() - localMillisAtUpdate) / 1000);
  long epoch = round(curEpoch + 3600 * utcOffset + 86400L) % 86400L;
  h = ((epoch  % 86400L) / 3600) % 24;
  m = (epoch % 3600) / 60;
  s = epoch % 60;
}

// =======================================================================

 String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
