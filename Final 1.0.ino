#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
#define redpin 3
#define greenpin 5
#define bluepin 6
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground

unsigned long time;
unsigned long timeFinal;
unsigned long timeOutput;

int redColors = 0;
int greenColors = 0;
int blueColors = 0;
int brownColors = 0;
int yellowColors = 0;
int orangeColors = 0;
int unidentifiedColors = 0;

byte btData[6];

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  //Serial.println("Color View Test!");

  if (tcs.begin()) {
    //Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  // use these three pins to drive an LED
#if defined(ARDUINO_ARCH_ESP32)
  ledcAttachPin(redpin, 1);
  ledcSetup(1, 12000, 8);
  ledcAttachPin(greenpin, 2);
  ledcSetup(2, 12000, 8);
  ledcAttachPin(bluepin, 3);
  ledcSetup(3, 12000, 8);
#else
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
#endif

  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
    //Serial.println(gammatable[i]);
  }
}

// The commented out code in loop is example of getRawData with clear value.
// Processing example colorview.pde can work with this kind of data too, but It requires manual conversion to 
// [0-255] RGB value. You can still uncomments parts of colorview.pde and play with clear value.
void loop() {
  //Time elapsed
  time = millis();
  
  float red, green, blue;
  
  tcs.setInterrupt(false);  // turn on LED

  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  tcs.setInterrupt(true);  // turn off LED

  //The color detection algorithm

  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));

//  Serial.print("\t");
//  Serial.print((int)red, HEX); Serial.print((int)green, HEX); Serial.print((int)blue, HEX);
  Serial.print("\n");

  if(red >= 150 && red > green && red > blue){
    Serial.print("The Color is RED");
    redColors++;
  }

  else if(green > red && green > blue){
    Serial.print("The Color is GREEN");
    greenColors++;
  }

  else if(blue > red && blue > green){
    Serial.print("The Color is BLUE");
    blueColors++;
  }

  else if(red > 100 && green > 70 && blue > 50){
    Serial.print("The Color is BROWN");
    brownColors++;
  }

  else if(red > 100 && green > 70 && blue < 50){
    Serial.print("The Color is YELLOW");
    yellowColors++;
  }

  else if(red > 100 && green > 50 && blue < 50){
    Serial.print("The Color is ORANGE");
    orangeColors++;
  }

  else{
    Serial.print("Idle...");
    unidentifiedColors++;
  }
  
  Serial.print("\n");

//  uint16_t red, green, blue, clear;
//  
//  tcs.setInterrupt(false);  // turn on LED
//
//  delay(60);  // takes 50ms to read
//
//  tcs.getRawData(&red, &green, &blue, &clear);
//  
//  tcs.setInterrupt(true);  // turn off LED
//
//  Serial.print("C:\t"); Serial.print(int(clear)); 
//  Serial.print("R:\t"); Serial.print(int(red)); 
//  Serial.print("\tG:\t"); Serial.print(int(green)); 
//  Serial.print("\tB:\t"); Serial.print(int(blue));
//  Serial.println();


#if defined(ARDUINO_ARCH_ESP32)
  ledcWrite(1, gammatable[(int)red]);
  ledcWrite(2, gammatable[(int)green]);
  ledcWrite(3, gammatable[(int)blue]);
#else
  analogWrite(redpin, gammatable[(int)red]);
  analogWrite(greenpin, gammatable[(int)green]);
  analogWrite(bluepin, gammatable[(int)blue]);
#endif

  timeFinal = millis();

  timeOutput = timeFinal - time;

  Serial.print("Total M&M's Detected: "); //print the total amount of m&m that the sensor detected
  Serial.print(" R: "); Serial.print(int(redColors));
  btData[0] = redColors;
  Serial.print(" G: "); Serial.print(int(greenColors));
  btData[1] = greenColors;
  Serial.print(" B: "); Serial.print(int(blueColors));
  btData[2] = blueColors;
  Serial.print(" Br: "); Serial.print(int(brownColors));
  btData[3] = brownColors;
  Serial.print(" Y: "); Serial.print(int(yellowColors));
  btData[4] = yellowColors;
  Serial.print(" O: "); Serial.print(int(orangeColors));
  btData[5] = orangeColors;
  Serial.print(" U: "); Serial.print(int(unidentifiedColors));
  btData[6] = unidentifiedColors;
  Serial.print("\n");
  
  Serial.println("Time Elapsed: "); //prints time since program started
  Serial.print(timeOutput);//printing Time Elapsed
  Serial.print("  ");
  Serial.print("\n");
  Serial.print("\n");
  delay(100);          // wait a second so as not to send massive amounts of data

  Serial1.write(btData[0]);
  Serial1.write(btData[1]);
  Serial1.write(btData[2]);
  Serial1.write(btData[3]);
  Serial1.write(btData[4]);
  Serial1.write(btData[5]);
  Serial1.write(btData[6]);
}
