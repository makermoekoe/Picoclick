# Picoclick

<a href="https://www.instagram.com/p/CIODN2rq7Tt/?utm_source=ig_web_button_share_sheet"><img src="docs/pico_coin.JPG" width="300px"></a>

(Click on the image for a small video.)

People who follow my on Instagram already knew what I have worked on several months now with tons of PCB iterations... The Picoclick is a simple IOT-Button in a very small package. The dimensions of the PCB are only 18x20mm and the height with a 300909 battery is only about 10mm. It is able to connect to your local WiFi at an amazingly high speed of about 1.5 seconds in average. Yes, as it calls IOT-Button, it uses WiFi of course! That enables tons of possibilities for that little device. The original idea behind it was to control my local lights, outlets and radiators through a single button press - or two, or a button hold, ... As the Picoclick **doesn't use deepsleep** it enables many possible button combination, because the button isn't connected to the reset line of the MCU, like it is mainly done on such devices (even on my own old dashbuttons...). Nevertheless, the Picoclick can be used somewhere else of course! I have successfully integrated it into MQTT and IFTTT, but it can be used to control everything which has at least a WiFi connection. Additionally it has four external GPIOs which can be used to control things and read sensors for other projects. BTW. the Picoclick can be flashed with that sexy USB-C jack, because it has an onboard CP2102N USB-Serial bridge.

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

<img src="docs/pico_back.png" width="300px"></a>

<img src="docs/pico_front.png" width="300px"></a>

<img src="docs/pico_usbc.png" width="300px"></a>


If you still have questions then feel free to ask!

Have fun! :)
