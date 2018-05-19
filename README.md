# esp8266-led-pager
esp8266 check msg from server and send to MAX7219 LED Matrix
![image](https://github.com/sw3nlab/esp8266-led-pager/blob/master/title.png)

Example:

![image](https://github.com/sw3nlab/esp8266-led-pager/blob/master/esp-led-pager.gif)

Circuit Scheme

![image](https://github.com/sw3nlab/esp8266-led-pager/blob/master/pager.png)

1) Download  Arduino IDE, check updates and add `.zip` library from archive.
2) Open `main.ino` file find and edit next lines:

```php
const char* ssid = "YOU_PHONE_WI-FI_SSID";    //SSID Wi-Fi AP
const char* password = "WI-FI_AP_PASS";           //PWD
const char* host = "your.message_server.com"; //Message server 
```
and
```php
String url = "/PATH_TO_LED_FILE/led.php?rnd="; //PATH TO FILE WHITH MESSAGE (like: "/led.php?rnd=")
```
3) Create `led.php` example:
 ```php
 <?
 echo "LED:HELLO WORLD"
 ;?>
 ```
 
 4) Create WI-FI AP to your phone whith SSID and PASS like file `main.ino`
 
 5) Power ON esp8266
