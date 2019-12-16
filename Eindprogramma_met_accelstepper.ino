#include <FastLED.h>                                   //Bibliotheek voor de WS2812B
#include <Wire.h>                                      //I2C bibliotheek
#include <LCD.h>                                       //LCD bibliotheek
#include <LiquidCrystal_I2C.h>                         //LCD I2C bibliotheek
#include <SoftwareSerial.h>                            //bluetooth module bibliotheek
#include <AccelStepper.h>


#define NUM_LEDS 4
#define LED_PIN 4

CRGB led[NUM_LEDS];
LiquidCrystal_I2C  lcd(0x27, 2, 1, 0, 4, 5, 6, 7);     //0x27 is het standaard I2C adres van de LCD backpack
SoftwareSerial BT(3, 5);                               //BT module TX D3, BT module RX D5
const int upperLimitSwitch = 6;
const int lowerLimitSwitch = 7;
const int trigPin1 = 11;
const int echoPin1 = 12;
const int trigPin2 = 9;
const int echoPin2 = 8;
//const int stp = 2;
//const int dir = 10;
const int stp = 15;
const int dir = 16;
const int EN = 13;
AccelStepper stepper(1, 2, 10);

int buttonPushSelector = 1;
int Z = 0; 

long duration, cm, inches, mm;
long duration2, hoogte;

bool crate = 0;
void bierKrat();

void euroKrat();
void starting();
void crateSelector();
void boolBierKrat();
void readingSensorAfstand();
void boolEuroKrat();
void GreenLight();
void YellowLight();
void RedLight();
void BlueLight();
void krachtMeting();
void homing();

char a;
char b;


void setup() 
{
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(1000);
  stepper.setAcceleration(1000);
  
  BT.begin(9600);
  Serial.begin(9600);
  FastLED.clear();
  FastLED.addLeds<NEOPIXEL, LED_PIN>(led, NUM_LEDS);

  lcd.begin (16, 2); // for 16 x 2 LCD module          //Grote van het LCD scherm wordt ingesteld
  lcd.setBacklightPin(3, POSITIVE);                    
  lcd.setBacklight(HIGH);                              //Zet de backlight aan van het LCD
  
  pinMode(trigPin1, OUTPUT);                           //Stuurt een hoog frequent signaal uit vanaf de ultrasone afstandsensor
  pinMode(echoPin1, INPUT);                            //Vangt het hoog frequente signaal op voor de afstandssensor
  pinMode(trigPin2, OUTPUT);                           //Stuurt een hoog frequent signaal uit vanaf de ultrasone hoogtesensor
  pinMode(echoPin2, INPUT);                            //Vangt het hoog frequente signaal op voor de hoogtesensor                          
  pinMode(lowerLimitSwitch, INPUT);
  pinMode(upperLimitSwitch, INPUT);
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(EN, OUTPUT);
  digitalWrite(EN,LOW);
  
  starting();                                          //Laat de startanimatie zien
  homing();
}


void loop() 
{
 Serial.println("make a choice");
 if (Serial.available()>0)                                    //Leest of er een bluetooth signaal binnenkomt
 {
  a = Serial.read();                                      //Leest wat het bluetooth signaal inhoud 
  if (a=='A')
  {
    buttonPushSelector = 1;
  }
  if (a=='B')
  {
    buttonPushSelector = 2;
  }
 }
  switch (buttonPushSelector)                           //Een switch om de keuze te maken tussen de verschillende kratjes op basis van een variabele.
  {
    case 1:
      bierKrat();
      break;
    case 2:
      euroKrat();
      break;
  }
}


void homing()
{
  readingSensorAfstand();
  Serial.println(cm);
  if  (cm <= 110)
  {
    stepper.setSpeed(800);
    //digitalWrite(dir, LOW);
    while(digitalRead(upperLimitSwitch) == LOW)  //pin 7
    {
      stepper.run();
      //digitalWrite(stp,HIGH);
      //delay(1);
      //digitalWrite(stp, LOW);
      //delay(1);
    }
    Z = 10000;
    delay(100);
  }
  if  (cm > 110)
  {
    stepper.setSpeed(-800);
    //digitalWrite(dir, HIGH);
    while(digitalRead(lowerLimitSwitch) == LOW) // pin 6
    {
      stepper.run();
      /*digitalWrite(stp,HIGH);
      delay(1);
      digitalWrite(stp, LOW);
      delay(1);*/
    }
    Z = 0;
    delay(100);
  }
}

void motor(int target)
{
  int value;
  if(Z >= target)
  {
    stepper.setSpeed(-800);
    //digitalWrite(dir, HIGH);
    value = Z - target;
  }
  else
  {
    stepper.setSpeed(800);
    //digitalWrite(dir, LOW);
    value = target - Z; 
  }
  for(int i = 0; i < value; i++)
  {
    if(Z >= target)
    {
      Z = Z - 1; 
    }
    else
    {
      Z = Z + 1; 
    }
    stepper.run();
      /*digitalWrite(stp,HIGH);
      delay(1);
      digitalWrite(stp, LOW);
      delay(1);*/
  }
}

void starting() 
{
  lcd.setCursor (0, 0);                              
  lcd.print("Lifter starting  ");                         
  for (int x = 0; x < 3; x++)                           //Loop voor het herhalen van het effect
  {
    for (int j = 0; j < 256; j++)                       //Loop voor het feller worden van de LED's
    {
      for (int i = 0; i < NUM_LEDS; i++)                //Loop voor het tellen van de LED's 
      {
        led[i] = CRGB(j, 0, j);
      }
      FastLED.show();
      delay(2);
    }
    for (int j = 255; j >= 0; j--)                      //Loop voor het donkerder worden van de LED's
    {
      for (int i = 0; i < NUM_LEDS; i++)                //Loop voor het tellen van de LED's
      {
        led[i] = CRGB(j, 0, j);                         //Geeft Rood en Blauw de waardes van j
      }
      FastLED.show();
      delay(2);                                         //Debug delay
    }
  }
}


void Tillen(int Hoogte)                                           //Deze functie zorgt er voor dat het kratje wordt opgetilt
{
  if (Serial.available()>0)
  {
    Serial.println("type c for lifting");
    b = Serial.read();
    if (b=='C')
    {
      motor(Hoogte);
    }
  }
}



void bierKrat()                                         //Deze functie wordt aangreoepen als het bierkrat gekozen is
{
  lcd.setCursor (0, 1);                                 //Start op de 2e regel
  lcd.print("Bierkrat      ");                          //Geeft aan op het LCD welk kratje getilt wordt
  Serial.println("Type: Bierkrat");
  readingSensorAfstand();
  if (cm >= 5 && cm <= 9)                               //Als de waarde cm tussen 10 en 25 is, zal de LED groen worden
  {
    GreenLight();
    delay(500);
    Tillen(1500);
  }
  else if (cm >= 10 && cm <= 79)                       //Als de waarde cm tussen de 26 en 79 is, zal de LED geel worden
  {
    YellowLight();
  }
  else if (cm >= 80)                                    //Als de waarde cm boven de 80 is, zal de LED blauw worden
  {
    BlueLight();
  }
  else if (cm <= 5)                                     //Als de waarde cm kleiner is dan 9, zal de LED rood worden
  {                 
    RedLight();
  }
  lcd.home (); // set cursor to 0,0
  lcd.print(F("Afstand: "));
  lcd.print(cm);
  lcd.print(F(" cm    "));
}


void euroKrat()                                         //Deze functie wordt aangeroepen als het eurokrat gekozen is
{
  lcd.setCursor (0, 1);                                 //Start op de 2e regel
  lcd.print("Eurokrat      ");                          //Geeft aan op het LCD welk kratje getilt wordt
  Serial.println("Type: Eurokrat");   
  readingSensorAfstand();

  if (cm >= 10 && cm <= 25)                             //Als de waarde cm tussen 10 en 25 is, zal de LED groen worden
  {        
    GreenLight();
    delay(500);
    Tillen(500);
  }
  else if (cm >= 26 && cm <= 79)                        //Als de waarde cm tussen de 26 en 79 is, zal de LED geel worden
  {  
    YellowLight();
  }
  else if (cm >= 80)                                    //Als de waarde cm boven de 80 is, zal de LED blauw worden
  {
    BlueLight();
  }
  else if (cm <= 9)                                     //Als de waarde cm kleiner is dan 9, zal de LED rood worden
  {
    RedLight();
  }
  lcd.home ();                                          //Set de cursor op 0,0
  lcd.print(F("Afstand: "));           
  lcd.print(cm);                                        //Print de afstand op het LCD in cm
  lcd.print(F(" mm    "));            
}


void readingSensorHoogte()                              //Leest de sensor voor de hooogte af
{
  delay(100);
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  hoogte = (duration2 / 2) / 29.1;                      //Zet de tijd van het signaal om naar een afstand
}


int readingSensorTest()                              //Leest de sensor voor de hooogte af
{
  delay(100);
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  hoogte = (duration2 / 2) / 29.1;                      //Zet de tijd van het signaal om naar een afstand
  return hoogte;
}


void readingSensorAfstand()                             //Leest de sensor voor de afstand uit
{
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration = pulseIn(echoPin1, HIGH);
  cm = (duration / 2) / 29.1;                           //Zet de tijd van het signaal om naar een afstand
}


void RedLight()                                         //Zorgt er voor dat de LED rood licht kan geven
{
  for (int i = 0; i < NUM_LEDS; i++) {
    led[i] = CRGB(153, 0, 0);
  }
  FastLED.show();
}


void YellowLight()                                      //Zorgt er voor dat de LED geel licht kan geven
{
  for (int i = 0; i < NUM_LEDS; i++) {
    led[i] = CRGB(153, 153, 0);
  }
  FastLED.show();
}


void GreenLight()                                       //Zorgt er voor dat de LED groen licht kan geven
{
  for (int i = 0; i < NUM_LEDS; i++) {
    led[i] = CRGB(0, 153, 0);
  }
  FastLED.show();
}


void BlueLight()                                        //Zorgt er voor dat de LED blauw licht kan geven
{
  for (int i = 0; i < NUM_LEDS; i++) {
    led[i] = CRGB(0, 0, 153);
  }
  FastLED.show();
}
