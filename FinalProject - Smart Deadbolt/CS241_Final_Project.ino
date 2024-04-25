/*
  Spencer Baysinger
  Created: 3/16/2024
  Updated: 4/17/2024
  File: CS241 Final Project
  Use: This project uses a few peripherial sensors to display
  		environment information: Temp, Light, Humidity.

*/

// General init
#include <TimeLib.h>
int count = 0;
_Bool accesscontrol = false;
int lastviewstate = -1;
int viewstate = 0;

// Initialize LCD pins
#include <LiquidCrystal.h>
const int rs = 14, en = 9, d4 = 7, d5 = 6, d6 = 2, d7 = 4; // LED digital 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Initialize Photo, thermistor and humidity stuff
#include <DHT.h>
#include <DHT_U.h>
#define DHTTYPE DHT11
const int photoresistorPin = A5; // Photoresistor connected to A4
const int thermistorPin = A4; // Thermistor connected to A4
const int dhtpin = 8; // Humidity digital 
DHT dht(dhtpin, DHTTYPE);
const float R1 = 10000; // Resistance of the resistor (10k)
const float TO = 298.15; // Temperature in Kelvin for 25 degrees
const float B = 3977; // Beta coefficient

// Initialize button stuff
const int buttonpin = 3; // Button Control
int buttonstate;
int lastbuttonstate = HIGH;
unsigned long buttontime = 0, buttonbounce = 0, buttondelay = 500;

// RFID Initializations
#include <SPI.h>
#include <MFRC522.h>
const int rfid_ss = 10;
const int rfid_rst = 5;
MFRC522 rfid(rfid_ss, rfid_rst);
unsigned long rfidtime = 0, rfidbounce = 0, rfiddelay = 4000;
int gatetimer = 3000;
String validID[9] = {"23E1419A"};
int validIDcount = 1, validIDmax = 9;
_Bool hasScanned = false;

// Servo Initialization
#include <Servo.h>
const int servopin = 15; // Servo control digital
Servo myservo;

// **********************************
// Byte Drawing Stuff BEGIN
// **********************************

  byte Corner1[8] = {
    0b11111,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000
  };

  byte Corner2[8] = {
    0b11111,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001
  };

  byte Corner3[8] = {
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b11111
  };

  byte Corner4[8] = {
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b11111
  };

  byte Border1[8] = {
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };

  byte Border2[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111
  };

  byte Viewstate[8] = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };

// **********************************
// Byte Drawing Stuff END
// **********************************

void check_pin_change() {
  buttontime = millis(); // Get the current time
  // Serial.read(buttonpin);
  if ((buttontime - buttonbounce) > buttondelay) { // Check if debounce period has passed
    if (viewstate == 3) viewstate = -1; // Increment the viewState counter
    if (viewstate < 3) viewstate++; // Increment the viewState counter
    buttonbounce = buttontime; // Update the last debounce time
    timestamp();
    Serial.print("Updated to Viewstate ");
    Serial.println(viewstate);
  }
}

float calculate_temperature() { 
  // Read Thermistor
  float Vout = analogRead(thermistorPin) * 5.00 / 1023.00; // Read votage across thermistor
  float Rth = R1 / (5.0 / Vout - 1); // Calculate thermistor resistance
  float lnR = log(Rth / R1);
  float T = 1 / (1/TO + lnR/B);
  float temperatureF = (T - 273.15) * 9.0 / 5.0 + 32;
  return temperatureF;
}

void timestamp() {
  unsigned long now = millis()/1000;
  int seconds = now%60;
  int minutes = (now/60)%60;
  int hours = (now/3600)%24;
  int days = (now/3600*24);
  char buffer[20]; // Buffer to hold the formatted time
  sprintf(buffer, "[%02d:%02d:%02d] ", hours, minutes, seconds); // Format the time
  Serial.print(buffer);
}

// **********************************
// RFID BEGIN
// **********************************

  String rfidscan() {
    String tempID = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      tempID += (rfid.uid.uidByte[i] < 0x10 ? "0" : "") + String(rfid.uid.uidByte[i], HEX);
    }
    tempID.toUpperCase();
    timestamp();
    Serial.print("ID Scan ");
    Serial.println(tempID);
    return tempID;
  }

  _Bool rfidcheck(String readID) {

    if (isauthorized(readID)) {
      accesscontrol = true;
      // rfid.PICC_HaltA(); // halt PICC
      // rfid.PCD_StopCrypto1(); // stop encryption on PCD
    } else {
      accesscontrol = false;
      timestamp();
      Serial.println("Access Denied");
    }
    count = 0;
    return accesscontrol;
  }

  void open_gate() {
    timestamp();
    Serial.println("Access Granted");

    // Open gate
    delay(250);
    timestamp();
    Serial.println("Opening Gate");
    myservo.write(180);
    delay(gatetimer);

    // Close gate
    timestamp();
    Serial.println("Closing Gate");
    myservo.write(0);
    delay(250);
    accesscontrol = false;
    lcd.clear();
    draw_border(14, 0, 1, 1);
    draw_viewstate_marker();
    draw_viewstate();
    delay(1000);

    // rfid.PICC_HaltA(); // halt PICC
    // rfid.PCD_StopCrypto1(); // stop encryption on PCD
  }

  bool isauthorized(String id) {
    for (int i = 0; i < (sizeof(validID) / sizeof(validID[0])); i++) {
      if (id == validID[i]) {
        return true;
      }
    }
    return false;
  }

  // RFID Whitelist Management
  // User will move to Viewstate 4 and scan a valid ID
  // Then User will be prompted to scan a NEW id (Timeout and Duplicate ID's rejected)
  // Then user will add ID and refresh Viewstate 4
  void rfidadd () {
    // Verify first ID scan
    String tempID1 = rfidscan();

    // Adding ID
    if (validIDcount < 9 && isauthorized(tempID1)) {
      // Authorized, prep scan
      timestamp();
      Serial.println("Viewstate 4 Authorized Scan");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("--REMOVE CARD---");
      delay(3000); // Delay to prevent duplicate scan

      // Wait for new card
      timestamp();
      Serial.println("Please Scan New ID Now");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("----SCAN NOW----");
      // Wait for new scan
      unsigned long timeout = millis() + 10000;
      // While loop looking for present ID scan - Timeout 10 seconds
      while (!rfid.PICC_IsNewCardPresent()) {
        if (millis() > timeout) {
          timestamp();
          Serial.print("Scan Timeout");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("----TIME OUT----");
          delay(3000);
          lastviewstate = -1;
          return;
        }
        delay(100);
      }

      // New scan received, collect ID
      if (rfid.PICC_ReadCardSerial()) {
        String tempID2 = rfidscan();

        // If ID Scan is already authorized
        if (isauthorized(tempID2)) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("--DUPLICATE ID--");
          timestamp();
          Serial.print("Rejected Duplicate ID Scan ");
          Serial.println(tempID2);
          delay(3000);
          lastviewstate = -1;
          return;
        }

        // Card is not duplicate, add ID
        validID[validIDcount] = tempID2;
        validIDcount++;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("----ID ADDED----");
        timestamp();
        Serial.print("ID Added ");
        Serial.println(tempID2);
      }
    } else {
      // No more space, all 9 slots are filled
      timestamp();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("----NO SPACE----");
      Serial.println("No Space To Add ID");
    }

    // Temporary delay before rewriting Viewstate 4
    delay(3000);
    lastviewstate = -1;
  }

// **********************************
// RFID END
// **********************************


// **********************************
// VIEWSTATE CONTROL BEGIN
// **********************************

  void set_border() {
    lcd.createChar(1, Corner1);
    lcd.createChar(2, Corner2);
    lcd.createChar(3, Corner3);
    lcd.createChar(4, Corner4);
    lcd.createChar(5, Border1);
    lcd.createChar(6, Border2);
    lcd.createChar(7, Viewstate);
  }

  void draw_border(int x, int y, int width, int height) {
    set_border();

    lcd.setCursor(x, y);
    lcd.write(byte(1));

    lcd.setCursor(x+width, y);
    lcd.write(byte(2));

    lcd.setCursor(x+width, y+height);
    lcd.write(byte(3));

    lcd.setCursor(x, y+height);
    lcd.write(byte(4));

    if (width > 2) {
      for (int i = 0; i <= 1; i++) {
        for (int j = x+1; j <= x+width-1; j++) {
          if (i==0) {
            lcd.setCursor(j,0);
            lcd.write(byte(5));
          }
          if (i==1) {
            lcd.setCursor(j,y+height);
            lcd.write(byte(6));
          }
        }
      }
    }
  }

  void draw_viewstate() {
    // View state 0 displays: Arm state
    // View state 1 displays: Time and Light level
    // View state 2 displays: Temperature and humidty
    // View state 3 displays: 
    lastviewstate = viewstate;
    if (viewstate == 0) draw_view0();
    if (viewstate == 1) draw_view1();
    if (viewstate == 2) draw_view2();
    if (viewstate == 3) draw_view3();
  }

  void  redraw_data() {
    if (viewstate == 1) {
      lcd.setCursor(7,1);
      lcd.print(analogRead(photoresistorPin));
    }
    if (viewstate == 2) {
      int humi = dht.readHumidity();
      int tempF = dht.readTemperature(true);  
      int tempF2 = (tempF + calculate_temperature()) / 2;
      lcd.setCursor(8,0);
      lcd.print(tempF2);
      lcd.setCursor(8,1);
      lcd.print(humi);
    }
    if (viewstate == 3) {
      lcd.setCursor(10,1);
      lcd.print(validIDcount);
    }
  }


  void draw_viewstate_marker() {
    if (viewstate == 0) {
      lcd.setCursor(14,0);
      lcd.write(byte(7));
    }
    else if (viewstate == 1) {
      lcd.setCursor(15,0);
      lcd.write(byte(7));
    }
    else if (viewstate == 2) {
      lcd.setCursor(14,1);
      lcd.write(byte(7));
    }
    else if (viewstate == 3) {
      lcd.setCursor(15,1);
      lcd.write(byte(7));
    }
  }

  void draw_view0(){
    // View 1, temp and time
    lcd.setCursor(0, 0);
    lcd.print("Arm State");
    lcd.setCursor(0, 1);
    if (accesscontrol) lcd.print("--Unlocked");
    else lcd.print("--Locked");
  }

  void draw_view1() {
    // Light and Time
    lcd.setCursor(0, 0);
    lcd.print("Time : 99:99");
    lcd.setCursor(0, 1);
    lcd.print("Light: ");
    lcd.print(analogRead(photoresistorPin));

  }

  void draw_view2() {
    // Temp and Humidity
    int humi = dht.readHumidity();
    int tempF = dht.readTemperature(true);  
    int tempF2 = (tempF + calculate_temperature()) / 2;

    lcd.setCursor(0, 0);
    lcd.print("Temp  = ");
    lcd.print(tempF2);
    lcd.setCursor(0, 1);
    lcd.print("Humid = ");
    lcd.print(humi);
  }

  void draw_view3() {
    lcd.setCursor(0, 0);
    lcd.print("Scan to Add ID");
    lcd.setCursor(0, 1);
    lcd.print("ID Space: ");
    lcd.print(validIDcount);
    lcd.print("/");
    lcd.print(validIDmax);
  }

// **********************************
// VIEWSTATE CONTROL END
// **********************************

// **********************************
// SETUP BEGIN
// **********************************

void setup() {
  // Set baud rate
  Serial.begin(9600); 
  Serial.println();
  timestamp();
  Serial.println("Initializing");

  // LCD and temp/humidity sensor init
  lcd.begin(16, 2); // Initialize LCD
  lcd.clear();
  dht.begin();

  // Set button to pullup and attachinterupt
  pinMode(buttonpin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonpin), check_pin_change, FALLING);

  // Init RFID package 
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

  timestamp();
  Serial.print("Valid IDs: ");
  for (int i = 0; i < validIDcount; i++) {
    Serial.print(validID[i]);
  }
  Serial.println();

  // Init Servo
  myservo.attach(servopin);
  myservo.write(0);

  // Finished init
  timestamp();
  Serial.println("Started");
}

// **********************************
// LOOP BEGIN
// **********************************

void loop() {
  redraw_data();
  if (viewstate != lastviewstate) {
    lcd.clear();
    draw_border(14, 0, 1, 1);
    draw_viewstate_marker();
    draw_viewstate();
  }

  
  if (rfid.PICC_IsNewCardPresent()) { // Check for present card
    if (rfid.PICC_ReadCardSerial()) { // Read the present card
      rfidtime = millis(); // Trigger bounce counter to prevent too frequent scans
      if ( ((hasScanned == false) && millis() <= 10000) || (rfidtime - rfidbounce) > rfiddelay) { // If last scan was more than ten seconds ago
        hasScanned = true;
        String readID = "";
        readID = rfidscan();
        rfidbounce = millis(); // Update the last debounce time

        if (viewstate != 3) {
          if (rfidcheck(readID)) {
            // Update to Viewstate 0
            viewstate = 0;
            lcd.clear();
            draw_border(14, 0, 1, 1);
            draw_viewstate_marker();
            draw_viewstate();
            lastviewstate = -1;
            open_gate();
          } else {
            lcd.clear();
            draw_border(14, 0, 1, 1);
            draw_viewstate_marker();
            draw_viewstate();
            lcd.setCursor(2,1);
            lcd.print("Auth Failed");
            lastviewstate = -1;
            delay(3000);
          }
        }
        else if (viewstate == 3) {
          if (rfidcheck(readID)) {
            accesscontrol = false;
            rfidadd();  
            lastviewstate = -1;
          }
        }
      }
    }
  }

  // Standard end-of-loop delay
  delay(100); 
}