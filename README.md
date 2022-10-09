Arduino IDE for ESP8266 - Bitcoin and Ethereum Price Monitoring with ESP8266 
===========================================

Application written by web3sdruvs

# Introduction
This library provides an interface to the Bitcoin and Ethereum price tracking APIs. It is written using Arduino IDE and works using ESP8266.

# Getting the code
You can install Arduino IDE esp8266-monitoring-btc-eth for your ESP8266 by downloading the code that is hosted here. You can check the latest development version and be informed about updates.

# Hardware
- NodeMcu ESP8266 Module
- 16x2 LCD Display Module

### Architecture
![alt text](https://raw.githubusercontent.com/web3sdruvs/esp8266-monitoring-btc-eth/main/data/architecture.webp?token=GHSAT0AAAAAABY36TU22DKAT366MTCNEEY2YZGS4HQ)

# Sketch / include Library / Add .Zip library	

### Installing with Boards Manager

Starting with 1.6.4, Arduino allows installation of third-party platform packages using Boards Manager. We have packages available for Windows, Mac OS, and Linux (32 and 64 bit).

- Install the current upstream Arduino IDE at the 1.8.9 level or later. The current version is on the [Arduino website](https://www.arduino.cc/en/software).
- Start Arduino and open the Preferences window.
- Enter ```https://arduino.esp8266.com/stable/package_esp8266com_index.json``` into the *File>Preferences>Additional Boards Manager URLs* field of the Arduino IDE. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *esp8266* platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).

### Getting started
View the last released ESP8266 documentation at: [https://arduino-esp8266.readthedocs.io/en/3.0.2/](https://arduino-esp8266.readthedocs.io/en/3.0.2/).

# Updates
Here are the forecast updates listed:

- Apollo - Monitoring: Track Bitcoin and Ethereum price with LCD. (Done) ✅ 
- Ares - PriceAlerts: Receive price alerts set via Telegram Bot. (Under development) ⌚
- Artemis - Monitoring Whale Portfolio. (Under development) ⌚

# Donations
If this repository helps you in your development or if you would like to support this development, you can give me a cup of coffee. ☕

You can send me:

- Bitcoin (BTC) 
- Ethereum (ERC-20)
- Polkadot (DOT)
- Cardano (ADA) 

Wallet address: 

```bash
web3sdruvs.crypto
```

Bitcoin lightning network: 
```bash
bc1qjgz6y7sh2rw5fl55432rr6n6vs9hrhhhsn2eft
```

# License and Credits
You may copy, distribute and modify the software provided that modifications are described and licensed for free under GNU . Derivatives works (including modifications or anything statically linked to the library) can only be redistributed under GNU  but applications that use the library don't have to be.

[ArduinoJson](https://arduinojson.org) library written by Benoit BLANCHON, built from https://github.com/bblanchon/ArduinoJson, is used in this project.  It is distributed under the [MIT License](https://arduinojson.org/#legal-details).

[BearSSL](https://bearssl.org) library written by Thomas Pornin, built from https://github.com/earlephilhower/bearssl-esp8266, is used in this project.  It is distributed under the [MIT License](https://bearssl.org/#legal-details).

[RTClib](https://learn.adafruit.com/ds1307-real-time-clock-breakout-board-kit/overview) library written by Adafruit Industries, built from https://github.com/adafruit/RTClib, is used in this project.  It is distributed under the [MIT License](https://learn.adafruit.com/ds1307-real-time-clock-breakout-board-kit/overview/#legal-details).

[LiquidCrystal_I2C](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library) library written by fdebrabander, built from https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library, is used in this project.

[UniversalTelegramBot](https://www.arduino.cc/reference/en/libraries/universaltelegrambot/) library written by Brian Lough, built from https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot, is used in this project.  It is distributed under the [MIT License](https://www.arduino.cc/reference/en/libraries/universaltelegrambot//#legal-details).
