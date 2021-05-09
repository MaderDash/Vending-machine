/*
    Project : RF ID Vending Machine Simulator
    Programmed by: JimFyyc
    Date Started: May-08-2021
    Date Updated: May-08-2021
    Inspired by : Twitch.tv/madderdash
    Inspired by : BuildEmUp (on Discord Server)
    Real code made by @buildemup#0229  on discord.
    05/08/2021
*/
 
#include <SPI.h>
#include <RFID.h>
#include <Servo.h>
int timer              = 900;
int clockwise          = 120;
//int counterclockwise = 55;
int Stop               = 90;
int locked             = 1;
int redLEDPin          = 5;
int greenLEDPin        = 6;
int accessGrantedSize  = 2;            //The number of serial numbers
int ledArray[2] = {redLEDPin, greenLEDPin};
int Buttons[4] = {A0, A1, A2, A3};
int buttonSet          = 0;
int buttonFinal        = 0;
int buttonNumber       = 1;
 
long TimeOut           = 0;
long tripped           = 5000;
 
RFID rfid(10, 4);                    //D10:pin of tag reader SDA. D9:pin of tag reader RST
unsigned char status;
unsigned char str[MAX_LEN];          //MAX_LEN is 16: size of the array
 
String accessGranted [2] = {"310988016", "271011115139"};
//RFID serial numbers to grant access to
 
Servo lockServo;                    //Servo for locking mechanism
Servo lockServo1;
Servo lockServo2;
Servo lockServo3;
 
void setup()
{
  Serial.begin(9600);               //Serial monitor is only required to get tag ID numbers and for troubleshooting
  delay(20);
  SPI.begin();                      //Start SPI communication with reader
  delay(20);
  rfid.init();                      //initialization
  delay(20);
  pinMode(redLEDPin, OUTPUT);       //LED startup sequence
  pinMode(greenLEDPin, OUTPUT);
  leds(2, 0);
  lockServo.attach(9);
  lockServo1.attach(8);
  lockServo2.attach(7);
  lockServo3.attach(3);
  for (int Counter = 0; Counter < 4; Counter++) {
    pinMode(Buttons[Counter], INPUT_PULLUP);
  }
}
 
void loop()
{
  //motorTest();
  Serial.println("Place card/tag near reader...");
  //Wait for a tag to be placed near the reader
  if (rfid.findCard(PICC_REQIDL, str) == MI_OK)
  {
    Serial.println("Card found");
    String temp = "";
    //Temporary variable to store the read RFID number
    if (rfid.anticoll(str) == MI_OK)
      //Anti-collision detection, read tag serial number
    {
      Serial.print("The card's ID number is : ");
      for (int i = 0; i < 4; i++)
        //Record and display the tag serial number
      {
        temp = temp + (0x0F & (str[i] >> 4));
        temp = temp + (0x0F & str[i]);
      }
      Serial.println (temp);
      checkAccess (temp);
      //Check if the identified tag is an allowed to open tag
    }
    rfid.selectTag(str);
    //Lock card to prevent a redundant read, removing the line will make the sketch read cards continually
  }
  rfid.halt();
}
 
void checkAccess (String temp)
//Function to check if an identified tag is registered to allow access
{
  buttonSet             = 0;
  int granted           = 0;
  for (int i = 0; i <= (accessGrantedSize - 1); i++)
    //Runs through all tag ID numbers registered in the array
  {
    if (accessGranted[i] == temp)
      //If a tag is found then open/close the lock
    {
      TimeOut = millis();
      Serial.println ("Access Granted");
      granted            = 1;
 
      while (buttonSet == 0) {
        for (int counter = 0; counter < 4; counter++) {
          buttonNumber = digitalRead(Buttons[counter]);
          if (millis() > (tripped + TimeOut)) {
            counter     = 5;
            buttonSet   = 1;
            buttonFinal = 4;
          }
          if (counter == 3 && buttonNumber == 1) {
            counter = -1;
          }
          if (buttonNumber == 0) {
            buttonFinal = counter;
            buttonSet   = 1;
            counter     = 5;
          }
        }
      }
      switch (buttonFinal)
      { case 0:  lockServo.write(clockwise); break;
        case 1: lockServo1.write(clockwise); break;
        case 2: lockServo2.write(clockwise); break;
        case 3: lockServo3.write(clockwise); break;
        default: Serial.println("no button pressed"); break;
      }
      delay(timer);
      buttonFinal      = 0;
      lockServo.write(Stop);
      lockServo1.write(Stop);
      lockServo2.write(Stop);
      lockServo3.write(Stop);
      leds(2, 1);
      leds(2, 1); //Green LED sequence
    }
  }
  //If the tag is not found
  if (granted == 0)
  {
    Serial.println ("Access Denied");
    for (int blinks = 0; blinks < 2; blinks++) {
      //Red LED sequence
      leds(1 , 0);
    }
  }
}
void leds(int number, int call) {
 
  for (int count = call; count < number; count++) {
    digitalWrite(ledArray[count], HIGH);
    delay(200);
  }
  for (int count = call; count < number; count++) {
    digitalWrite(ledArray[count], LOW);
    delay(200);
  }
}
 
void motorTest() {
 
  lockServo.write(counterclockwise);
  Serial.println("servo 0 is turning");
  delay(4000);
  lockServo.write(Stop);
  lockServo1.write(counterclockwise);
  Serial.println("servo 1 is turning");
  delay(4000);
  lockServo1.write(Stop);
  Serial.println("servo 2 is turning");
  lockServo2.write(counterclockwise);
  delay(4000);
  lockServo2.write(Stop);
  Serial.println("servo 3 is turning");
  lockServo3.write(counterclockwise);
  delay(4000);
  lockServo3.write(Stop);
  delay(2000);
}
