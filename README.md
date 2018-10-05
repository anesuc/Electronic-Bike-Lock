# Electronic-Bike-Lock
This is the main codebase that handles the Electronic Bike Lock logic for the Engineering Design 3B Project - __Team 37__. The runs on the [Arduino Uno Rev3](https://store.arduino.cc/usa/arduino-uno-rev3) platform powered by the ATmega328P [(datasheet)](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf). The PICC is the RFID Card or Tag using the [ISO/IEC 14443A](https://en.wikipedia.org/wiki/ISO/IEC_14443) interface, for example Mifare or NTAG203.

* The RFID reader (and writer) used in this project can be found [here](https://www.jaycar.com.au/arduino-compatible-rfid-read-and-write-kit/p/XC4506). It's known as the RFID-rc522. However our version has firmware v2 (there is another different but popular version of the same product with the same board number "rc522").
* The microcontroller and the RFID reader use SPI for communication.
* The RFID reader and the RFID tags communicate using a 13.56 MHz electromagnetic field.

## How does it work?

It constantly checks if there is an RFID card that has been scanned. When an RFID card has been scanned it then checks if it's a valid RFID card. This is followed by checking if it matches any previously scanned RFID card, if not store the UID and any relevant information of that card, send a 1 second pulse to one of the actuator pins (i.e actuator pin A), trigger the Red LED and turn off the Green LED. If the card that has been scanned does match the previously scanned card, we then send a 1 second pulse to the other actuator pin (i.e actuator pin B), turn off Red LED and turn on Green LED.

When the bike is currently in a locked state and a card is scanned but does not match the one that was scanned to lock the bike, it pulses both LED pins to indicate an "access denied/invalid card" response.

## How to set it up and run

1. Firstly, connect the RFID reader and writer (rc522) as specified in the code. You can also follow the visual representation of the connections as seen [here](RFID Pin Layout.png).
2. Connect the green LED to pin 7 and red LED to pin 8 (or whatever specified in the code under those variables).
3. Connect the 2 actuator triggers to pin 2 and 4 (or whatever specified in the code under those variables).
4. Now on your computer download the RFID library zip and install it (if you are using the Arduino IDE). You can do this by selecting (from the top bar menu depending on your OS "Sketch" -> "Include Library" -> "Add .ZIP library". Or by following instructions as provided [here](https://www.arduino.cc/en/Guide/Libraries).
5. Now simply load the main bike lock .ino file into the IDE, connect the Arduino to the PC through USB (or whatever you use to estabilish the communication between the Arduino and the PC) and upload it to the Arduino. Now test to see if it works! You can verify to see that everything is fine through the serial monitor.
