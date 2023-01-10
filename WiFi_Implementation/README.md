# WiFi Module Serial Communication
This design provides Wi-Fi capabilities to the microcontroller allowing us to communicate with user devices over Wi-Fi.

#### Basic Understandings
- [UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
- [HTTP](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol)

#### Useful Readings
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer/blob/master/README.md)

### Hardware

#### Part Selection

- Ai-Thinker [ESP-01](https://docs.ai-thinker.com/_media/esp8266/docs/esp-01_product_specification_en.pdf) Wi-Fi module
- Texas Instruments [MSP430FR2476](https://www.ti.com/product/MSP430FR2476/part-details/MSP430FR2476TRHAR) Mixed-Signal Microcontroller

#### Necessary Connections
![Hardware connections image...](https://github.com/KalebOTB/Embedded-Systems/blob/main/WiFi_Implementation/Circuit.png)

### Software

#### ESP8266-01

[Arduino IDE](https://www.arduino.cc/en/software) was used for rapid development on the ESP-01. The main functions are as follows:
- TCP/IP implemented as HTTP web server
  - Allows us to monitor and configure values over UART from our microcontroller in a user friendly format
- UART communication with our MSP430 microcontroller
  - Communication work horse. UART will allow us to change and read values stored in the MSP430. The MSP430 will be the microcontroller that is connected to the sensors and controlling different aspects of the greenhouse.

Full example code can be found [here]
