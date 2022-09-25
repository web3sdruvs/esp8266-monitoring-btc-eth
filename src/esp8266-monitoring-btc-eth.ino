/*
 Copyright (c) 2022 web3sdrvus.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h> 
#include <Wire.h>  
#include <ESP8266WiFi.h>  
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecureBearSSL.h>
#include <UniversalTelegramBot.h> 
#include "credentials.h"

ESP8266WiFiMulti WiFiMulti;

//SHA1 fingerprint of the certificate. 
const char fingerprint[] = SHA1_CERTIFICATE;

double btc_price; 
double eth_price;
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte zero[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

byte two[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};

byte three[] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};

byte four[] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};

byte five[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};


     
void setup() {
  
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  lcd.clear(); 
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  lcd.setCursor(5, 0);
  lcd.print("HELLO");
  delay(300);
  lcd.setCursor(4, 1);      
  lcd.print("FRIEND!");
  delay(2000);
  //Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID,WIFI_PASSWD); 
  lcd.clear(); 
  lcd.setCursor(0, 0);      
  connectionWifi(); //Check connection WiFi
  delay(3000);
  lcd.clear(); 
      
}

void loop() {
 
  getData(); //Get api data
  lcd.setCursor(0, 0);
  lcd.print("BTC: ");
  lcd.setCursor(5, 0);
  lcd.print("$"+String(btc_price));
  lcd.setCursor(0, 1);
  lcd.print("ETH: ");
  lcd.setCursor(5, 1);
  lcd.print("$"+String(eth_price));
  delay(15000);
   
}

void connectionWifi(){

//If you get here you have connected to the WiFi
  if ((WiFiMulti.run() == WL_CONNECTED)) { //Wait for WiFi connection
      lcd.setCursor(0, 0);      
      lcd.print("CONNECTED");
      delay(1000);
    }
    else {
      lcd.setCursor(0, 0);      
      lcd.print("NOT CONNECTED");
      delay(1000);
    }

}

void getData() {
 
  //Use WiFiClientSecure class to create TLS connection
  //Use SSL certificate
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setFingerprint(fingerprint);

  if ((WiFi.status() == WL_CONNECTED)) { 

      HTTPClient https;
      StaticJsonDocument<200> doc;
      int httpCode;

      //Bitcoin
      https.begin(*client, "https://api.binance.com/api/v3/ticker/price?symbol=BTCUSDT");
      httpCode = https.GET();   

      //Check the status of the api, if it is activated, the json get is done
      if (httpCode > 199 && httpCode < 300) {//Check if API is enabled
          DynamicJsonDocument doc(1024);
          deserializeJson(doc, https.getString());
          JsonObject object = doc.as<JsonObject>();
          btc_price = object["price"]; //Get the value that is in json
      }
      else {
        btc_price = 0.00; 
      }

      //Ethereum
      https.begin(*client, "https://api.binance.com/api/v3/ticker/price?symbol=ETHUSDT");
      httpCode = https.GET();   

      //Check the status of the api, if it is activated, the json get is done
      if (httpCode > 199 && httpCode < 300) {//Check if API is enabled
          DynamicJsonDocument doc(1024);
          deserializeJson(doc, https.getString());
          JsonObject object = doc.as<JsonObject>();
          eth_price = object["price"]; //Get the value that is in json
      }
      else {
        eth_price = 0.00; 
      }

      https.end(); 
      
    }
}

void updateProgressBarLoop() {
  
  for(int i=0; i <= 100; i++){
    lcd.setCursor(0,0);
    lcd.print(i);
    lcd.print("   ");
    updateProgressBar(i, 100, 1); //This line calls the subroutine that displays the progress bar.  
    delay(200);
  }

  delay(1000);

  for(int i=100; i >= 0; i--){
    lcd.setCursor(0,0);
    lcd.print(i);
    lcd.print("   ");
    updateProgressBar(i, 100, 1);   
    delay(50);
  }
  delay(1000);
  
}

void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn) {
  
  double factor = totalCount/80.0; //See note above!
  int percent = (count+1)/factor;
  int number = percent/5;
  int remainder = percent%5;

  if(number > 0){
     lcd.setCursor(number-1,lineToPrintOn);
     lcd.write(5);
  }
 
     lcd.setCursor(number,lineToPrintOn);
     lcd.write(remainder); 
       
}
