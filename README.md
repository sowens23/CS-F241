# CS-241 Final Project Write-Up
- Author  : Spencer Baysinger
- Course  : CS-241 Computer Hardware (Circuits)
- Date    : 2024/04/24
- Title   : Final Project Write-Up
- Purpose : This document is to serve as the final output of the final project, which was to design, code, and assemble an Arduino project from essentially scratch.

## Overview
For Computer Hardware (Circuits) I designed a hardware/software package that serves as an rfid-smart deadbolt lock for a door. Additinally we would imagine that this door lock would provide the user with useful information as well as a key management system.

<p float="left">
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/fullpic1.jpg" width="40%" />
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/fullpic2.jpg" width="40%" /> 
</p>

### Capabilities
  - Delivering analog data from;
    - Thermistor (Temperature Sensor) 
    - A Hygrometer (Humidity Sensor)
    - A PhotoSensitive Resistor (Light Level Sensor)
  - Reading RFID scans
  - Controlling a 4.8 micro servo motor (Deadbolt lock)
  - Displaying information on an LCD 1602

## Viewstates

The data is delivered to the user through an LCD1602 screen, which has four changeable menu *viewstates* which are displayed on the right side of the screen. The screens are selected by pressing a button.
Viewstates are updated in real-time with analog input, or scanned data from sensors.

These viewstates are;

### **Viewstate 1**
- Viewstate 1 contains **arm state** information. With a few different display variations depending on authorization attempts

<p float="left">
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/1-1.jpg" width="25%"/>
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/1-2.jpg" width="25%"/> 
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/1-3.jpg" width="25%"/> 
</p>

### **Viewstate 2**
- Viewstate 2 contains **Light** information. The intention was to also display Time on this screen, but I did not foresee the need, and couldn't order a *Real-Time Clock(RTC)* in time.
  
<img src="https://github.com/sowens23/CS-F241/blob/main/media/2-3.jpg" width="40%"/> 

### **Viewstate 3**
- Viewstate 3 contains **Temperature and Light** information. 

<img src="https://github.com/sowens23/CS-F241/blob/main/media/3-1.jpg" width="40%"/>

### **Viewstate 4**
- Viewstate 4 contains access to a screen that enables a user to **Add Authorized RFID**. It functions by first scanning an *authorized* badge, and then scanning an *unauthorized* badge to be added to a whitelist. 

<p float="center">
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/4-1.jpg" width="25%"/>
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/4-2.jpg" width="25%"/> 
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/4-3.jpg" width="25%"/> 
</p>

## Assembly and Development
- The arduino project uses headers: "LiquidCrystal.h", "GHT.h", "SPI.h", and "MFRC522.h". 
- The button is programmed using attachInterupt so that the lcd can update immediately as the button is pressed.
- The viewstate is only updated when the button is pressed. Otherwise, if the viewstate reads analog input, then this is redrawin every 100ms.
- The RFID reader required me to solder a pin header onto it. This took three attempts to get it to work correctly.

<p float="left">
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/solder1.jpg" width="20%"/>
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/solder2.jpg" width="20%"/> 
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/solder3.jpg" width="20%"/> 
  <img src="https://github.com/sowens23/CS-F241/blob/main/media/solder4.jpg" width="20%"/> 
</p>

- The standard arduino loop cycles through the following:
  - If button has been pressed, update drawn viewstate.
  - If current viewstate contains analog data, update data.
  - Viewstates 1-3: If authorized card is present, unlock servo for 3 seconds.
    - Display "Access Denied" for unauthorized card scans.
  - Viewstates 4: If authorized card is present, add next scanned rfid to whitelist

- The serial output is formatted to display time stamps and pertinent information regarding the hardware.

<img src="https://github.com/sowens23/CS-F241/blob/main/media/serialoutput.png" width="40%"/>

## Closing Notes
- I'm glad I had two arduino kits to use throughout the semester.
- This project was really fun, I only wish I had more time throughout the semester to work on this project.
- Future direction could include:
  - External power for servo.
  - 3-D printed door lock with deadbolt.
  - An RTC for current time and date.

<img src="https://github.com/sowens23/CS-F241/blob/main/media/arduinokits.jpg" width="40%"/>
