#include <Wire.h>
#include <Servo.h>
#include "Adafruit_TCS34725.h"

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
#define redpin 3
#define greenpin 5
#define bluepin 6
#define feedingServoPin 11
#define sortingServoPin 9

Servo feedingServo;
Servo sortingServo;

int value = 1450;
int currentPosition = 0;
int colorPositions[7]={0,180,360,540,720,900,1080};
int t_turn = 0;

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

float red, green, blue;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
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

void loop() {
  
  tcs.setInterrupt(false);  // turn on LED

  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  tcs.setInterrupt(true);  // turn off LED

  if(red >= 150 && red > green && red > blue){
    Serial.print("The Color is RED");
    redColors++;
    
    t_turn = colorPositions[0]-currentPosition;
    if(t_turn < 0){
      turnOneCup_L(abs(t_turn));
    }else if(t_turn > 0){
      turnOneCup_R(t_turn);
    }
    feed();
    currentPosition = colorPositions[0];
    
  }

  else if(green > red && green > blue){
    Serial.print("The Color is GREEN");
    greenColors++;

    t_turn = colorPositions[1]-currentPosition;
    if(t_turn < 0){
      turnOneCup_L(abs(t_turn));
    }else if(t_turn > 0){
      turnOneCup_R(t_turn);
    }
    feed();
    currentPosition = colorPositions[1];
    
  }

  else if(blue > red && blue > green){
    Serial.print("The Color is BLUE");
    blueColors++;

    t_turn = colorPositions[2]-currentPosition;
    if(t_turn < 0){
      turnOneCup_L(abs(t_turn));
    }else if(t_turn > 0){
      turnOneCup_R(t_turn);
    }
    feed();
    currentPosition = colorPositions[2];
    
  }

  else if(red > 100 && green > 70 && blue > 50){
    Serial.print("The Color is BROWN");
    brownColors++;

    t_turn = colorPositions[3]-currentPosition;
    if(t_turn < 0){
      turnOneCup_L(abs(t_turn));
    }else if(t_turn > 0){
      turnOneCup_R(t_turn);
    }
    feed();
    currentPosition = colorPositions[3];
    
  }

  else if(red > 100 && green > 70 && blue < 50){
    Serial.print("The Color is YELLOW");
    yellowColors++;

    t_turn = colorPositions[4]-currentPosition;
    if(t_turn < 0){
      turnOneCup_L(abs(t_turn));
    }else if(t_turn > 0){
      turnOneCup_R(t_turn);
    }
    feed();
    currentPosition = colorPositions[4];
    
  }

  else if(red > 100 && green > 50 && blue < 50){
    Serial.print("The Color is ORANGE");
    orangeColors++;

    t_turn = colorPositions[5]-currentPosition;
    if(t_turn < 0){
      turnOneCup_L(abs(t_turn));
    }else if(t_turn > 0){
      turnOneCup_R(t_turn);
    }
    feed();
    currentPosition = colorPositions[5];
    
  }

  else{
    Serial.print("Idle...");
    unidentifiedColors++;

    t_turn = colorPositions[6]-currentPosition;
    if(t_turn < 0){
      turnOneCup_L(abs(t_turn));
    }else if(t_turn > 0){
      turnOneCup_R(t_turn);
    }
    feed();
    currentPosition = colorPositions[6];
    
  }
  
  Serial.print("\n");

  #if defined(ARDUINO_ARCH_ESP32)
  ledcWrite(1, gammatable[(int)red]);
  ledcWrite(2, gammatable[(int)green]);
  ledcWrite(3, gammatable[(int)blue]);
#else
  analogWrite(redpin, gammatable[(int)red]);
  analogWrite(greenpin, gammatable[(int)green]);
  analogWrite(bluepin, gammatable[(int)blue]);
#endif
}

void turnOneCup_L(int t){
  sortingServo.attach(sortingServoPin);
  sortingServo.writeMicroseconds(1000);
  delay(t);
  sortingServo.detach();
  delay(5);
}

void turnOneCup_R(int t){
  sortingServo.attach(sortingServoPin);
  sortingServo.writeMicroseconds(2000);
  delay(t);
  sortingServo.detach();
  delay(5);
}

void feed(){
  feedingServo.attach(feedingServoPin);
  feedingServo.writeMicroseconds(2000);
  delay(540);
  feedingServo.detach();
  delay(5);
}
