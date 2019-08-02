# esp32-led-pager
esp32 check msg from server and send to MAX7219 LED Matrix

![image](https://github.com/sw3nlab/esp8266-led-pager/blob/master/esp32/esp32.png)

HOW TO:
Find next strings in `main.c`
```php
        wifiMulti.addAP("ssid", "pass");//SSID and PASSWORD
        http.begin("http://YOURESERVER.com/esp.php"); //HTTP
```

UPLOAD TO You server and create file `esp.php`

```php

<?php
$date = date("d/m/Y H:i");

$str = array("LED: TEXT 1. ",
    "LED: TEXT 2",
    "LED:Today: ".$date,
    "LED:Join to vk.com/cyberunit ! ",
    "LED:>>> Kirishi HackerSpace <<< ",
    "LED:TEXT 6 ",
    "1111".date("H:i"));
$rnd = rand(0,6);
echo trim($str[$rnd]);

?>

```

PROFIT )
