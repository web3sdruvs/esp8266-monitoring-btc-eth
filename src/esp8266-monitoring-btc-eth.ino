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

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

ESP8266WiFiMulti WiFiMulti;
WiFiClientSecure client_bot;
UniversalTelegramBot bot(BOT_TOKEN, client_bot);

//SHA1 fingerprint of the certificate.
const char fingerprint_price[] = SHA1_CERTIFICATE_PRICE; 
const char fingerprint_index[] = SHA1_CERTIFICATE_INDEX; 
double btc_price; 
double eth_price;
int index_fg;
String index_fg_str;
int index_fg_last; 
double btc_price_last; 
double eth_price_last;
int btc_variation_percentage;
int eth_variation_percentage;

//Interval for duration analysis
int inverval_time = 30;

//Text to message for Telegram
String chat_start;
String chat_all;
String chat_price;
String chat_index;
String chat_time;
String chat_variation;

//Delay parameter for event
unsigned long current_millis = 0;
const unsigned long event_millis = 30000;
unsigned long previous_millis = 0;
unsigned long previous_millis2 = 0;

//LCD settings byte
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte zero[]  = {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000};
byte one[]   = {B10000,B10000,B10000,B10000,B10000,B10000,B10000,B10000};
byte two[]   = {B11000,B11000,B11000,B11000,B11000,B11000,B11000,B11000};
byte three[] = {B11100,B11100,B11100,B11100,B11100,B11100,B11100,B11100};
byte four[]  = {B11110,B11110,B11110,B11110,B11110,B11110,B11110,B11110};
byte five[]  = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};

void setup() {
  
  Serial.begin(115200);

  //Add certificate
  //get UTC time
  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      
    client_bot.setTrustAnchors(&cert); 
  #endif
  #ifdef ESP32
    client_bot.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  #endif

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

  //This line calls the displays the progress bar.
  lcd.clear();
  updateProgressBarLoop(); 
  lcd.setCursor(0, 0);      

  //Check connection WiFi
  connectionWifi(); 
  delay(500);
  lcd.clear();

  //Get api data  
  displayParameter(); 
  getDataIndex();

  //Save last values
  index_fg_last = index_fg;
  btc_price_last = btc_price; 
  eth_price_last = eth_price;

  //Send start menssage in Telegram 
  chat_start  = "🟢Bot Online\n\nHello Friend!\n\n";
  chat_start += "/start - return all commands\n";
  chat_start += "/all - return all informations\n";
  chat_start += "/p - return prices\n";
  chat_start += "/i - return index fear and greed\n";
  chat_start += "/t - change monitoring time\n";  
  chat_start += "/v - change variation scale\n"; 
  bot.sendMessage(CHAT_ID, chat_start, "");
      
}

void loop() {
  
  current_millis = millis();

  //Loop executes each input received by the chat
  int count_messages = bot.getUpdates(bot.last_message_received + 1);
  
  while(count_messages) {
    telegramCommands(count_messages);
    count_messages = bot.getUpdates(bot.last_message_received + 1);
  }

  eventTrigger();
}

//If you get here you have connected to the WiFi
void connectionWifi(){

  //Wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) { 
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

//Display settings
void displayParameter(){

  //Get api data
  getDataPrice();   
  lcd.setCursor(0, 0);
  lcd.print("BTC: ");
  lcd.setCursor(5, 0);
  lcd.print("$"+String(btc_price));
  lcd.setCursor(0, 1);
  lcd.print("ETH: ");
  lcd.setCursor(5, 1);
  lcd.print("$"+String(eth_price));
}

//Conditional when the bot receives new messages
void telegramCommands(int count_messages) {

  for (int i=0; i<count_messages; i++) {

    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "❌Unauthorized access", "");
      continue;
    }
    
    String user_text = bot.messages[i].text;

    if (user_text == "/start") {
      chat_start = "You can control me by sending these commands:\n\n";
      chat_start += "/all - return all informations\n";
      chat_start += "/p - return prices\n";
      chat_start += "/i - return index Fear And Greed\n";
      chat_start += "/t - change monitoring time\n";   
      chat_start += "/v - change variation scale\n";  
      bot.sendMessage(chat_id, chat_start, "");
    }

    if (user_text == "/all") {
      chat_all = "💰Prices\n\n";
      chat_all += "Bitcoin: $"+String(btc_price)+"\n";  
      chat_all += "Ethereum: $"+String(eth_price)+"\n\n";
      chat_all += "📊Fear And Greed Index\n\n";
      chat_all += "Classification: "+String(index_fg_str)+"\n";
      chat_all += "Index: "+String(index_fg)+"/100\n";
      bot.sendMessage(chat_id, chat_all, "");
    }

    if (user_text == "/p") {
      chat_price = "💰Prices\n\n";
      chat_price += "Bitcoin: $"+String(btc_price)+"\n";  
      chat_price += "Ethereum: $"+String(eth_price)+"\n";
      bot.sendMessage(chat_id, chat_price, "");
    }

    if (user_text == "/i") {
      chat_index = "📊Fear And Greed Index\n\n";
      chat_index += "Classification: "+String(index_fg_str)+"\n";
      chat_index += "Index: "+String(index_fg)+"/100\n";   
      bot.sendMessage(chat_id, chat_index, "");
    }

    if (user_text == "/t") {
      chat_time = "⏱️Set the analysis range\n\n";
      chat_time += "/5min - set the interval to 5 minutes\n";
      chat_time += "/15min - set the interval to 10 minutes\n";
      chat_time += "/30min - set the interval to 30 minutes\n";
      chat_time += "/1h - set the interval to 1 hour\n";     
      bot.sendMessage(chat_id, chat_time, "");
    }

    if (user_text == "/5min") {
      chat_time = "✅Analysis interval set to 5 minutes\n";  
      inverval_time = 10;
      bot.sendMessage(chat_id, chat_time, "");
    }

    if (user_text == "/15min") {
      chat_time = "✅Analysis interval set to 15 minutes\n";  
      inverval_time = 30;
      bot.sendMessage(chat_id, chat_time, "");
    }

    if (user_text == "/30min") {
      chat_time = "✅Analysis interval set to 30 minutes\n";  
      inverval_time = 60;
      bot.sendMessage(chat_id, chat_time, "");
    }
    
    if (user_text == "/1h") {
      chat_time = "✅Analysis interval set to 1 hour\n";  
      inverval_time = 120;
      bot.sendMessage(chat_id, chat_time, "");
    }
  }
}

//Event trigger for price change and market sentiment change
void eventTrigger(){

  //This is the event 
  if (current_millis - previous_millis >= event_millis) {
    displayParameter();
    previous_millis = current_millis; 
  }  

  //This is the event 2
  if (current_millis - previous_millis2 >= (event_millis*inverval_time)) {
    getDataIndex();

    //Percentage variation calculation    
    btc_variation_percentage = round((btc_price/btc_price_last-1)*100);
    eth_variation_percentage = round((eth_price/eth_price_last-1)*100);

    if (index_fg != index_fg_last) {
      chat_index = "🚨Alert for change in market sentiment\n\n";
      chat_index += "Classification: "+String(index_fg_str)+"\n"; 
      chat_index += "Index Now: "+String(index_fg)+"/100\n"; 
      chat_index += "Index Previous: "+String(index_fg_last)+"/100\n";
      bot.sendMessage(CHAT_ID, chat_index, "");
    }

    if (btc_variation_percentage >= 5 || btc_variation_percentage <= -5) {
      chat_price = "🚨Bitcoin price change alert\n\n";
      chat_price += "Bitcoin: $"+String(btc_price)+" | "+btc_variation_percentage+"%\n"; 
      bot.sendMessage(CHAT_ID, chat_price, "");
    }

    if (eth_variation_percentage >= 5 || eth_variation_percentage <= -5) {
      chat_price = "🚨Ethereum price change alert\n\n";
      chat_price += "Ethereum: $"+String(eth_price)+" | "+eth_variation_percentage+"%\n"; 
      bot.sendMessage(CHAT_ID, chat_price, "");
    }

    //Save last values
    previous_millis2 = current_millis;
    index_fg_last = index_fg;
    btc_price_last = btc_price; 
    eth_price_last = eth_price;
  }
}

void getDataPrice() {
 
  //Use WiFiClientSecure class to create TLS connection
  //Use SSL certificate
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setFingerprint(fingerprint_price);

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

    //Fixed a small bug in the API that happens for a few seconds
    if (btc_price < 10) {
      btc_price = btc_price_last;
    } else if (eth_price < 10) {
      eth_price = eth_price_last;
    }

    btc_price_last = btc_price;    
    eth_price_last = eth_price;

    https.end(); 
  }
}

void getDataIndex() {
 
  //Use WiFiClientSecure class to create TLS connection
  //Use SSL certificate
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setFingerprint(fingerprint_index);

  if ((WiFi.status() == WL_CONNECTED)) { 

    HTTPClient https;
    StaticJsonDocument<200> doc;
    int httpCode;
   
    //Fear and Greed
    https.begin(*client, "https://api.alternative.me/fng/?");
    httpCode = https.GET(); 
    
    //Check the status of the api, if it is activated, the json get is done
    if (httpCode > 199 && httpCode < 300) {//Check if API is enabled
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, https.getString());
      JsonObject object = doc.as<JsonObject>();
      index_fg = object["data"][0]["value"]; //Get the value that is in json
    }
    else {
      index_fg = 0; 
    }

    https.end(); 

    //Zones index feal and greed
    if (index_fg > 0 && index_fg <= 24) {
      index_fg_str = "Extreme Fear";
    } 
    else if (index_fg > 24 && index_fg <= 44) {
      index_fg_str = "Fear";
    }
    else if (index_fg > 44 && index_fg <= 55) {
      index_fg_str = "Neutral";
    }
    else if (index_fg > 55 && index_fg <= 74) {
      index_fg_str = "Greed";
    }
    else if (index_fg > 74 && index_fg <= 100) {
      index_fg_str = "Extreme Greed";
    }
    else {
      index_fg_str = "No data";
    }
  }
}

void updateProgressBarLoop() {
  
  for(int i=0; i <= 100; i++) {
    lcd.setCursor(0,0);
    lcd.print(i);
    lcd.print("   ");
    updateProgressBar(i, 100, 1); //This line calls the subroutine that displays the progress bar.  
    delay(50);
  }

  delay(1000);
  lcd.clear(); 
}

void updateProgressBar(unsigned long count, unsigned long total_count, int line_to_print_on) {
  
  double factor = total_count/80.0; //See note above!
  int percent = (count+1)/factor;
  int number = percent/5;
  int remainder = percent%5;

  if(number > 0) {
    lcd.setCursor(number-1,line_to_print_on);
    lcd.write(5);
  }
    lcd.setCursor(number,line_to_print_on);
    lcd.write(remainder);      
}
