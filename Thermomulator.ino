/*

Thermomulator.ino
 
Copyright (c) 2011-2013Paul Kulyk <paul.kulyk@usask.ca>
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
*/


#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2);
char inByte = 0;

#define bPin 4

unsigned int sensor_pin = A0;
#define MAX_SENSOR 45.0
#define MIN_SENSOR 30.0
int sensor_value = 0;
int timer;
int start_time;

#define WAIT 0
#define SPINNING 1
#define DISPLAY 2
#define DISPLAY_WAIT 3
#define SPIN_TIME 5000
#define DISPLAY_TIME 15000


void display_temp(float );

void setup() {
  Serial.begin(9600);
  Serial.println("Simumometer on.");
  mySerial.begin(9600);
  delay(100);
  pinMode(bPin, INPUT);
  digitalWrite(bPin, HIGH);// Pullup on.
  
  timer = millis();
  
  mySerial.write(0x76); //Reset display
  
  mySerial.write(0x77); // Set decimals
  mySerial.write(0b00100010);// Turn on decimal point and degree sign
  
  mySerial.write(0x7A);
  mySerial.write(1); // Dim display (0 bright, 255 dim)
  
  display_temp(88.8);
}

int state = WAIT;
int spinner = 1;

void loop() {
  float temperature;
  sensor_value = analogRead(sensor_pin);
  temperature = sensor_value*(MAX_SENSOR-MIN_SENSOR)/1023.0 + MIN_SENSOR;
  switch (state){
   case WAIT: 
      if (digitalRead(bPin) == LOW)
      {
        state = SPINNING;
        start_time = millis();
      }
      break;
   case SPINNING:
      if (millis() - start_time < SPIN_TIME)
      {
          mySerial.write(0x7B);
          mySerial.write(spinner);
          mySerial.write(0x7C);
          mySerial.write(spinner);
          mySerial.write(0x7D);
          mySerial.write(spinner);
          //mySerial.write(0x7E);
          //mySerial.write(spinner);
          spinner = spinner << 1;
          if (spinner == 0b01000000) spinner = 1;
      }
      else
        state = DISPLAY;
      break;
    case DISPLAY:
      display_temp(temperature);
      state = DISPLAY_WAIT;
    case DISPLAY_WAIT:
      if (digitalRead(bPin) == LOW) display_temp(temperature);
      if (millis() - start_time > DISPLAY_TIME) state = WAIT;
      break;
  }
  

//  
  delay(100);
}

void display_temp(float temperature)
{
  float temp10x = temperature*10.;
  mySerial.write(0x76); //Reset display
  mySerial.print((int)temp10x);
  mySerial.write("C");
}

