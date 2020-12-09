# Picoclick

<a href="https://www.instagram.com/p/CEPDfjlKE4K/?utm_source=ig_web_button_share_sheet"><img src="docs/pico_coin.JPG" width="450px"></a>

(Click on the image for a small video.)

People who follow my Instagram already knew what I have worked on several months now with tons of PCB iterations... But now it's done!
The Picoclick is a simple IOT-Button in a very small package. The dimensions of the PCB are only 18x20mm and the height with a 300909 battery is only about 10mm. It is able to connect to your local WiFi at an amazingly high speed of about 1.5 seconds in average. Yes, as it calls IOT-Button, it uses WiFi of course! That enables tons of possibilities for that little device. The original idea behind it was to control my local lights, outlets and radiators in my smart home through a single button press - or two, or a button hold, ... As the Picoclick **doesn't use deepsleep** it enables many possible button combinations, because the button isn't connected to the reset line of the MCU, like it is mainly done on such devices (even on my own old dashbuttons...). Nevertheless, the Picoclick can be used somewhere else of course! I have successfully integrated it into MQTT and IFTTT, but it can be used to control everything which has at least a WiFi connection. Additionally it has four external GPIOs which can be used to control things and read sensors for other projects. BTW. the Picoclick can be flashed with that sexy USB-C jack, because it has an onboard CP2102N USB-Serial bridge.

Here are some specs of the Picoclick:
- “Pico” size: 18.0×20.0mm board outline
- Single button interface
- Three WS2812 LEDs for visual feedback
- ESP8285 MCU board (behaves like the ESP8266, just a little less memory)
- Four external GPIOs (as solder pads)
- Fully integrated LiPo charging solution with a great protection circuit (including Over(dis)charge, reverse polarity, short circuit protection and more)
- Chargind and programming via the USB Type-C jack (integrated CP2102N USB-Serial bridge)
- A 30mAh (9x9x3mm pouch cell) battery is enough to reaches over 350 clicks.
- Ultra low stand-by current

<img src="docs/pico_back.JPG" width="450px"></a>

<img src="docs/pico_front.JPG" width="450px"></a>

<img src="docs/pico_usbc.JPG" width="450px"></a>

For battery info, check my [website](https://makermoekoe.com/2020/12/08/picoclick/).


## GPIOs

Function | GPIO | Mode
-------- | -------- | --------
WS2812 Din | GPIO14   | Output
Button | GPIO12   | Input
Latch* | GPIO13   | Output
Charge Stat. | GPIO4  | Input
Ext IO1  |  GPIO6   | IO
Ext IO2  |  GPIO7   | IO
Ext IO3  |  GPIO8   | IO
Ext IO4  |  GPIO10  | IO

*The Picoclick doesn't use the deepsleep functionality of the MCU, because it uses too much power. It comes with its own power latching circuit. The MCU can control its own power with the latch pin. Once it is held high the ESP8285 will get power and as soon as it is pulled low the ESP8285 will shut down. Easy peasy...

## Circuit

The circuit is based on the following components:
- MCU ESP8285 with external 24MHz oscillator and external Rainsun AN2051 chip antenna
- CP2102N USB-Serial bridge with autoreset circuit based on the MUN5214DW1T1G dual transistor
- RT9193 3,3v voltage regulator
- LiPo charger BRCL4054BME (MCP73831 alternative, pin compatible)
- XB5353A LiPo protection with SEU7401U reverse polarity protection; power measurement via voltage divider
- power latching circuit based on 4148 SOD523 diodes
- WS2812 2020 mini RGB LEDs

## Code

You can just run every code that you have in your mind, just make sure that the Picoclick needs to be powered in one of the following two ways:
- button press
- latching pin
So please keep in mind that it will shut down if you release the button and doesn't pull the latch pin high. Typically you have to call following two lines of code at first.
```
pinMode(latch,OUTPUT);
digitalWrite(latch,HIGH);
```
To turn the Picoclick off, you have to call:
```
digitalWrite(latch,LOW);
```

The charge status GPIO can be used to read the state of the onboard LiPo charger. Use the following code first to enable this feature:
```
pinMode(status_charger,INPUT_PULLUP);
int status = digitalRead(status_charger);
```
Where ```status_charger```is the GPIO4. The ```status```variable will hold the status. It returns 0 when the battery is charging through the USB port. It returns 1 if the battery is fully charged.

### Fast WiFi connection
To enable a really fast WiFi connection you have to put your local access point into a fixed WiFi channel. In doing so, the Picoclick is able to know that channel which saves around a second while connecting to the AP. Another thing is the BSSID, which can be found on your access points settings. Just enter your local IP into your favorite internet browser and find that terrifying number somewhere. Add that settings to the following lines of code:

```
const uint8_t bssid[6] = {0x80, 0x71, 0x7A, 0x3F, 0x8B, 0x63}; //put your access points bssid here
const int wifi_channel = 9; //set the channel in your access points settings
```

I have attached three simple examples in the code folder of this repo. A bare_minimum sketch with just a simple led animation, an mqtt example and an IFTTT example.

If you still have questions then feel free to ask!

Have fun! :)
