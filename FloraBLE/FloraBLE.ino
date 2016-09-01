#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
#define PIN 6
#define PINO 9
#define PINP 8

Adafruit_BluefruitLE_UART ble(Serial1, BLUEFRUIT_UART_MODE_PIN);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

Adafruit_NeoPixel left = Adafruit_NeoPixel(45, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel right = Adafruit_NeoPixel(45, PINO, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel neo = Adafruit_NeoPixel(1, PINP, NEO_GRB + NEO_KHZ800);

int timer = 0;
int compassReading = 0;
uint32_t compassTimer = timer;
int lastDir = 16;

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void setup(void)
{

  
  if ( !ble.begin(VERBOSE_MODE) ) {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);
  ble.verbose(false);

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }
  
  left.begin();
  left.setBrightness(222);
  left.show();
  right.begin();
  right.setBrightness(222);
  right.show();
  neo.begin();
  neo.setBrightness(100);
  neo.show();

  if (!mag.begin())
  {
    // Unable to initialize!
    while (1);
  }
}


void loop(void)
{
  timer += 1;
  compassCheck();
  compassDirection(compassReading);
  
  // Check for incoming characters from Bluefruit
  ble.println("AT+BLEUARTRX");
  ble.readline();
  if (strcmp(ble.buffer, "OK") == 0) {
    // no data
    return;
  }
  // Some data was found, its in the buffer
  //Serial.println(ble.buffer);
  
  if (strcmp(ble.buffer, "left") == 0) {
    clearAll();
    turnLeft();
    turnLeft();
    turnLeft();
    turnLeft();
    turnLeft();
    turnLeft();
  } else if (strcmp(ble.buffer, "right") == 0) {
    clearAll();
    turnRight();
    turnRight();
    turnRight();
    turnRight();
    turnRight();
    turnRight();
  } else if (strcmp(ble.buffer, "soonleft") == 0) {
    soonLeft();
    soonLeft();
    soonLeft();
    soonLeft();
    soonLeft();
    soonLeft();
    soonLeft();
    soonLeft();
  } else if (strcmp(ble.buffer, "soonright") == 0) {
    soonRight();
    soonRight();
    soonRight();
    soonRight();
    soonRight();
    soonRight();
    soonRight();
    soonRight();
  } else if (strcmp(ble.buffer, "forward") == 0) {
    int i;
    for (i = 0; i < 10; i++) {
      forward();
    }
  } else if (strcmp(ble.buffer, "display") == 0) {
    neo.setPixelColor(0, neo.Color(0, 0, 0));
    neo.show();
    delay(10);

    int i;
    for (i = 0; i < 10; i++) {
      fadeIn(0, 128, 26, 24, 61, 181);
      fadeIn(24, 61, 181, 219, 149, 27);
      fadeIn(219, 149, 27, 0, 128, 26);
    }

    neo.setPixelColor(0, neo.Color(0, 0, 0));
    neo.show();
  } else if (strcmp(ble.buffer, "place") == 0) {
    clearAll();
    int i;
    for (i=0; i<20; i++) {
      middleColorFade(neo.Color(128, 140, 250));
      delay(30);
      middleColorFade(neo.Color(82, 100, 255));
      delay(30);
      middleColorFade(neo.Color(71, 86, 218));
      delay(30);

      middleColorFade(neo.Color(58, 69, 174));
      delay(30);

      middleColorFade(neo.Color(71, 86, 218));
      delay(30);
      middleColorFade(neo.Color(82, 100, 255));
      delay(30);
    }
    clearAll();
  } else if (strcmp(ble.buffer, "dest") == 0) {
    clearAll();
    int i;
    for (i=0; i<8; i++) {
      allColor(neo.Color(128, 140, 250));
      delay(30);
      allColor(neo.Color(82, 100, 255));
      delay(30);
      allColor(neo.Color(71, 86, 218));
      delay(30);
      
      allColor(neo.Color(58, 69, 174));
      delay(30);

      allColor(neo.Color(71, 86, 218));
      delay(30);
      allColor(neo.Color(82, 100, 255));
      delay(30);
    }
    clearAll();
  } else if (strcmp(ble.buffer, "saw") == 0) {
    sawWave(right.Color(63, 0, 127));
    sawWave(right.Color(63, 0, 127)); 
    sawWave(right.Color(63, 0, 127));  
    sawWave(right.Color(63, 0, 127));
  } else if (strcmp(ble.buffer, "green") == 0) {
    bottomUpColor(left.Color(0, 127, 0));
  } else if (strcmp(ble.buffer, "red") == 0) {
    bottomUpColor(left.Color(127, 0, 0));
  } else if (strcmp(ble.buffer, "blue") == 0) {
    bottomUpColor(left.Color(0, 0, 127));
  } else if (strcmp(ble.buffer, "pink") == 0) {
    bottomUpColor(left.Color(127, 0, 127));
  } else if (strcmp(ble.buffer, "teal") == 0) {
    bottomUpColor(left.Color(0, 127, 127));
  } else if (strcmp(ble.buffer, "yellow") == 0) {
    bottomUpColor(left.Color(127, 127, 0));
  } else if (strcmp(ble.buffer, "orange") == 0) {
    bottomUpColor(left.Color(127, 63, 0));
  } else if (strcmp(ble.buffer, "purple") == 0) {
    bottomUpColor(left.Color(63, 0, 127));
  } else if (strcmp(ble.buffer, "rainbow") == 0) {
    bottomUpColor(left.Color(127, 0, 0));
    bottomUpColor(left.Color(127, 63, 0));
    bottomUpColor(left.Color(127, 127, 0));
    bottomUpColor(left.Color(63, 127, 0));
    bottomUpColor(left.Color(0, 127, 0));
    bottomUpColor(left.Color(0, 127, 63));
    bottomUpColor(left.Color(0, 127, 127));
    bottomUpColor(left.Color(0, 63, 127));
    bottomUpColor(left.Color(0, 0, 127));
    bottomUpColor(left.Color(63, 0, 127));
    bottomUpColor(left.Color(127, 0, 127));
    bottomUpColor(left.Color(127, 0, 63));
  } else if (strcmp(ble.buffer, "rain") == 0) {
    int i;
    for (i=0; i<10;i++) {
      rainbow();
      unrainbow();
    }
    clearAll();
  } else if (strcmp(ble.buffer, "done") == 0) {
    bottomUpColor(left.Color(0, 0, 0));
  }
  
  ble.waitForOK();
}

void compassCheck() {
  if (compassTimer > timer) compassTimer = timer;

  // Approximately every 10 seconds or so, update time
  if (timer - compassTimer > 1) {
    /* Get a new sensor event */
    sensors_event_t event; 
    mag.getEvent(&event);

    float Pi = 3.14159;

    compassTimer = timer;

    // Calculate the angle of the vector y,x
    float heading = (atan2(event.magnetic.y,event.magnetic.x) * 180) / Pi;

    // Normalize to 0-360
    if (heading < 0)
    {
      heading = 360 + heading;
    }
    compassReading = heading; 
  }  
} 

void compassDirection(int compassHeading) 
{
  // This code maps the compassHeading from [0, 360] to [0, 255]
  // This new range is more compatible with the NeoPixel color scheme
  // After the transformation, 0 and 255 refer to NORTH and 127 refers to SOUTH
  int dir = map(compassHeading, 0, 360, 0, 255);
  dir = (dir + 64) % 255;
//  Serial.print("Compass Direction: ");
//  Serial.println(compassHeading);
//  Serial.println(dir);
  
  int i;
  
  if (dir <= 127) {
    neo.setPixelColor(0, neo.Color((255 - dir*2)/3, (dir*2)/3, 0));
    neo.show();
//    for(i = 30; i<45; i++) {
//      left.setPixelColor(i, neo.Color((255 - dir*2)/3, (dir*2)/3, 0));
//      right.setPixelColor(i, neo.Color((255 - dir*2)/3, (dir*2)/3, 0));
//    }
//    left.show();
//    right.show();
  } else {
    dir -= 127;
    neo.setPixelColor(0, neo.Color((dir*2)/3, (255 - dir*2)/3, 0));
    neo.show();
//    for(i = 30; i<45; i++) {
//      left.setPixelColor(i, neo.Color((dir*2)/3, (255 - dir*2)/3, 0));
//      right.setPixelColor(i, neo.Color((dir*2)/3, (255 - dir*2)/3, 0));
//    }
//    left.show();
//    right.show();
  }
}

void bottomUpColor(uint32_t c) {
  int i;
  for (i = 0; i < 45; i++) {
    left.setPixelColor(i, c);
    left.show();
    right.setPixelColor(i, c);
    right.show();
    delay(10);
  }
}

void allColor(uint32_t c) {
  int i;
  for (i = 0; i < 45; i++) {
    left.setPixelColor(i, c);
    right.setPixelColor(i, c);
  }
  left.show();
  right.show();
  delay(10);
}

void middleColorFade(uint32_t c) {
  int i;
  for (i = 15; i < 30; i++) {
    left.setPixelColor(i, c);
    right.setPixelColor(i, c);
  }
  left.show();
  right.show();
  delay(10);
}

void soonRight() {
  int i;
  for (i = 0; i < 15; i++) {
    if (i == 0) {
      right.setPixelColor(15, right.Color(120, 120, 0));
      right.show();
      delay(70);
    } else if (i == 1) {
      right.setPixelColor(15, right.Color(0, 0, 0));
      right.setPixelColor(0, right.Color(120, 120, 0));
      right.setPixelColor(30, right.Color(120, 120, 0));
      right.setPixelColor(16, right.Color(120, 120, 0));
      right.show();
      delay(70);
    } else {
      if (i >= 7) {
        right.setPixelColor(i-9, right.Color(0, 0, 0));
        right.setPixelColor(i+7, right.Color(0, 0, 0));
        right.setPixelColor(i+21, right.Color(0, 0, 0));
        right.setPixelColor(i-8, right.Color(120, 120, 0));
        right.setPixelColor(i+8, right.Color(120, 120, 0));
        right.setPixelColor(i+22, left.Color(120, 120, 0));
      }
      right.setPixelColor(i-2, right.Color(0, 0, 0));
      right.setPixelColor(i+14, right.Color(0, 0, 0));
      right.setPixelColor(i+28, right.Color(0, 0, 0));
      right.setPixelColor(i-1, right.Color(120, 120, 0));
      right.setPixelColor(i+15, right.Color(120, 120, 0));
      right.setPixelColor(i+29, right.Color(120, 120, 0));
      right.show();
      delay(70);
    }
  }
    right.setPixelColor(i-2, right.Color(0, 0, 0));
    right.setPixelColor(i+14, right.Color(0, 0, 0));
    right.setPixelColor(i+28, right.Color(0, 0, 0));
    right.setPixelColor(i-1, right.Color(120, 120, 0));
    right.setPixelColor(i+29, right.Color(120, 120, 0));
    right.show();
    delay(70);
    right.setPixelColor(i-1, right.Color(0, 0, 0));
    right.setPixelColor(i+29, right.Color(0, 0, 0));
    right.setPixelColor(6, left.Color(0, 0, 0));
    right.setPixelColor(36, left.Color(0, 0, 0));
    right.setPixelColor(22, left.Color(0, 0, 0));
    right.show();
    delay(70);
}

void soonLeft() {
  int i;
  for (i = 0; i < 15; i++) {
    if (i == 0) {
      left.setPixelColor(29, left.Color(120, 120, 0));
      left.show();
      delay(70);
    } else if (i == 1) {
      left.setPixelColor(29, left.Color(0, 0, 0));
      left.setPixelColor(14, left.Color(120, 120, 0));
      left.setPixelColor(44, left.Color(120, 120, 0));
      left.setPixelColor(28, left.Color(120, 120, 0));
      left.show();
      delay(70);
    } else {
      if (i >= 7) {
        left.setPixelColor(23-i, left.Color(0, 0, 0));
        left.setPixelColor(53-i, left.Color(0, 0, 0));
        left.setPixelColor(37-i, left.Color(0, 0, 0));
        left.setPixelColor(22-i, left.Color(120, 120, 0));
        left.setPixelColor(52-i, left.Color(120, 120, 0));
        left.setPixelColor(36-i, left.Color(120, 120, 0));
      }
      left.setPixelColor(16-i, left.Color(0, 0, 0));
      left.setPixelColor(46-i, left.Color(0, 0, 0));
      left.setPixelColor(30-i, left.Color(0, 0, 0));
      left.setPixelColor(15-i, left.Color(120, 120, 0));
      left.setPixelColor(29-i, left.Color(120, 120, 0));
      left.setPixelColor(45-i, left.Color(120, 120, 0));
      left.show();
      delay(70);
    }
  }
    left.setPixelColor(1, left.Color(0, 0, 0));
    left.setPixelColor(15, left.Color(0, 0, 0));
    left.setPixelColor(31, left.Color(0, 0, 0));
    left.setPixelColor(0, left.Color(120, 120, 0));
    left.setPixelColor(30, left.Color(120, 120, 0));
    left.show();
    delay(70);
    left.setPixelColor(0, left.Color(0, 0, 0));
    left.setPixelColor(30, left.Color(0, 0, 0));
    left.setPixelColor(8, left.Color(0, 0, 0));
    left.setPixelColor(22, left.Color(0, 0, 0));
    left.setPixelColor(38, left.Color(0, 0, 0));
    left.show();
    delay(70);
}

void turnLeft() {
  int i;
  for (i = 0; i < 15; i++) {
    if (i == 0) {
      left.setPixelColor(29, left.Color(0, 120, 0));
      left.show();
      delay(70);
    } else {
      left.setPixelColor(15-i, left.Color(0, 120, 0));
      left.setPixelColor(29-i, left.Color(0, 120, 0));
      left.setPixelColor(45-i, left.Color(0, 120, 0));
      left.show();
      delay(50);
    }
  }
  left.setPixelColor(0, left.Color(0, 120, 0));
  left.setPixelColor(30, left.Color(0, 120, 0));
  left.show();
  delay(50);
  for (i = 0; i<left.numPixels(); i++) {
    left.setPixelColor(i, right.Color(0, 0, 0));
  }
  left.show();
  delay(50);
}

void turnRight() {
  int i;
  for (i = 0; i < 15; i++) {
    if (i == 0) {
      right.setPixelColor(15, right.Color(0, 120, 0));
      right.show();
      delay(70);
    } else {
      right.setPixelColor(i-1, right.Color(0, 120, 0));
      right.setPixelColor(i+15, right.Color(0, 120, 0));
      right.setPixelColor(i+29, right.Color(0, 120, 0));
      right.show();
      delay(70);
    }
  }
  right.setPixelColor(14, right.Color(0, 120, 0));
  right.setPixelColor(44, right.Color(0, 120, 0));
  right.show();
  delay(70);
  for (i = 0; i<right.numPixels(); i++) {
    right.setPixelColor(i, right.Color(0, 0, 0));
  }
  right.show();
  delay(50);
}

void forward() {
  clearAll();
  int color = neo.Color(0, 127, 127);
  int off = neo.Color(0, 0, 0);
  oneRow(color, 0);
  oneRow(color, 1);
  oneRow(color, 2);
  oneRow(off, 0);
  oneRow(off, 1);
  oneRow(off, 2);
}

void oneRow(uint32_t c, int x) {
  int i;
  int j = x*15;
  for (i = 0 + j; i< 15 + j; i++) {
    left.setPixelColor(i, c);
    right.setPixelColor(i, c);
  }
  left.show();
  right.show();
  delay(50);
}

void fadeIn(int firstR, int firstG, int firstB, int lastR, int lastG, int lastB) {
  int Rnew, Gnew, Bnew;
  int n = 100;
  for(int i = 0; i < n; i++) {
   Rnew = firstR + (lastR - firstR) * i / n;
   Gnew = firstG + (lastG - firstG) * i / n;
   Bnew = firstB + (lastB - firstB) * i / n;
   neo.setPixelColor(0, neo.Color(Rnew, Gnew, Bnew));
   neo.show();
   delay(10);
  }
}

void rainbow() {
  for(uint16_t i=0; i<15; i++) {
    left.setPixelColor(i, Wheel((i*(265/15)) & 255));
    left.setPixelColor(i+15, Wheel((i*(265/15)) & 255));
    left.setPixelColor(i+30, Wheel((i*(265/15)) & 255));
    left.show();
    right.setPixelColor(14-i, Wheel((i*(265/15)) & 255));
    right.setPixelColor(29-i, Wheel((i*(265/15)) & 255));
    right.setPixelColor(44-i, Wheel((i*(265/15)) & 255));
    right.show();
    delay(40);
  }
}

void unrainbow() {
  for(uint16_t i=0; i<15; i++) {
    right.setPixelColor(i, Wheel((i*(265/15)) & 255));
    right.setPixelColor(i+15, Wheel((i*(265/15)) & 255));
    right.setPixelColor(i+30, Wheel((i*(265/15)) & 255));
    right.show();
    left.setPixelColor(14-i, Wheel((i*(265/15)) & 255));
    left.setPixelColor(29-i, Wheel((i*(265/15)) & 255));
    left.setPixelColor(44-i, Wheel((i*(265/15)) & 255));
    left.show();
    delay(40);
  }
}

void sawWave(uint32_t color) {
  int i, j;
  for(i = 0; i < 15; i++) {
    allColor(left.Color(127, 63, 0));
    left.setPixelColor(30 + (i%4), color);
    left.setPixelColor(16 + (i%4), color);
    left.setPixelColor(2 + (i%4), color);
    left.setPixelColor(18 + (i%4), color);

    left.setPixelColor(34 + (i%4), color);
    left.setPixelColor(20 + (i%4), color);
    left.setPixelColor(6 + (i%4), color);
    left.setPixelColor(22 + (i%4), color);

    left.setPixelColor(38 + (i%4), color);
    left.setPixelColor(24 + (i%4), color);
    left.setPixelColor(10 + (i%4), color);
    left.setPixelColor(26 + (i%4), color);

    left.setPixelColor(42 + (i%4), color);
    left.setPixelColor(28 + (i%4), color);
    left.setPixelColor(14 + (i%4), color);
    
    left.show();

    right.setPixelColor(30 + (i%4), color);
    right.setPixelColor(16 + (i%4), color);
    right.setPixelColor(2 + (i%4), color);
    right.setPixelColor(18 + (i%4), color);

    right.setPixelColor(34 + (i%4), color);
    right.setPixelColor(20 + (i%4), color);
    right.setPixelColor(6 + (i%4), color);
    right.setPixelColor(22 + (i%4), color);

    right.setPixelColor(38 + (i%4), color);
    right.setPixelColor(24 + (i%4), color);
    right.setPixelColor(10 + (i%4), color);
    right.setPixelColor(26 + (i%4), color);

    right.setPixelColor(42 + (i%4), color);
    right.setPixelColor(28 + (i%4), color);
    right.setPixelColor(14 + (i%4), color);

    right.show();
    delay(140);
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return neo.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return neo.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return neo.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void clearAll() {
  int i;
  for (i = 0; i<right.numPixels(); i++) {
    right.setPixelColor(i, right.Color(0, 0, 0));
    left.setPixelColor(i, right.Color(0, 0, 0));
  }
  right.show();
  left.show();
  delay(60);
}
