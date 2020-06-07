/*
 * Name: Johnny Pham
 * Filename: final_project.ino
 * Description: Final project for Physics of Sensors and Robotics Course
 */

#include <LiquidCrystal.h>
#include <Servo.h>

const int  buttonPin = 7;    // the pin that the pushbutton is attached to
// const int  testButton = 7;
const int ledPin = 13;       // the pin that the LED is attached to
const int redLed = 8;
const int blueLed = 6;
const int irLED = 9;
const int irRecv = 10;
const int alertTime = 5;
const int disarmCode = 3;

Servo myservo;  // create servo object to control a servo

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int startTime;
int secondsPassed;
int zeroDetected;
int pos = 0;    // variable to store the servo position
int irLeft;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  
  myservo.attach(A0);  // attaches the servo on pin 9 to the servo object
  // put your setup code here, to run once:
  // initialize the button pin as a input:
  pinMode(buttonPin, INPUT);
  
  pinMode(blueLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  
  securitySystem();
}

void loop() { // put your main code here, to run repeatedly:}

void disarm()
{
  if (secondsPassed == alertTime and buttonPushCounter != disarmCode)
  {
    intruder();
  }
  else
  {
    if (secondsPassed == alertTime and buttonPushCounter == disarmCode)
    {
      digitalWrite(blueLed, HIGH);
      lcd.clear();
      lcd.write("    Passcode    ");
      lcd.setCursor(0,1);
      lcd.write("    Accepted    ");
      delay(2000);
      lcd.clear();
      securitySystem();   // Go back to the start
    }
  }
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);

  lcd.setCursor(0,1);
  lcd.write("Enter D-Code: ");

  lcd.setCursor(14, 1);
  lcd.print(buttonPushCounter);
  
  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
}

void securitySystem()
{
  buttonPushCounter = 0;
  secondsPassed = 0;
  Serial.println("In securitySystem()...");
  // startTime = millis();
  lcd.clear();
  lcd.write("Security System");
  lcd.setCursor(0,1);
  lcd.write("     Armed     ");
  
  // * IR LED and IR Receiver Sensor Module Code
  // * Setup code that initializes the IR LED to constantly blast the IR Receiver with IR
  // * While loop that continues to run as long as connection between IR LED and Receiver
  //   is not broken
  
  digitalWrite(blueLed, HIGH);
  irLeft = irDetect(irLED, irRecv, 38000);       // Check for object
  Serial.println(irLeft);
  while (irLeft != 0)                            // Checks to see if IR sensor is triggered
  {
    irLeft = irDetect(irLED, irRecv, 38000);     // Check for object
    Serial.println(irLeft);
    delay(100);
  }
  tone(13, 1000, 100);                           // Play tone for 1 second
  delay(200);                                    // Delay to finish tone
  tone(13, 500, 100);                            // Play tone for 1 second
  delay(200);                                    // Delay to finish tone
//  tone(13, 1000, 250);                         // Play tone for 1 second
//  delay(200);                                  // Delay to finish tone
  Serial.println("Got here");
  digitalWrite(blueLed, LOW);
  // Begin Count Down Sequence
  countdownSeq();
}

void countdownSeq()
{
  lcd.clear();
  Serial.println("Tripped");
  Serial.print("seconds at beg: ");
  Serial.println(secondsPassed);
  // Count for 5 seconds
  
  while (secondsPassed < alertTime + 1)
  {
    disarm();
    secondsPassed++;
    lcd.setCursor(0, 0);
    Serial.println(secondsPassed);
    lcd.print(secondsPassed);
    
    lcd.setCursor(2,0);
    lcd.write("seconds");
    
    unsigned long oneSec = millis();

    while (millis() < oneSec + 1000)
    {
      disarm();
    }
  }
  Serial.print("seconds at end: ");
  Serial.println(secondsPassed);
}

void intruder()
{
  lcd.clear();
  int b1 = digitalRead(buttonPin);
  while (true)
  {
    b1 = digitalRead(buttonPin);
    deter();
    
    int beginCount = 1;
    //twitchServo();
    while (b1 == 1)
    {
      deter();
      myservo.write(1500);
      lcd.clear();
      lcd.write("Resetting...");
      lcd.setCursor(0, 1);
      lcd.print(beginCount);
      b1 = digitalRead(buttonPin);
      Serial.println("In this loop");
      unsigned long oneSec = millis();
  
      while (millis() < oneSec + 1000);
      beginCount++;
      if (beginCount == 3)
      {
        myservo.write(1500);
        delay(1000);
        securitySystem();
      }
    }
    // Reset counter back to 0 if button is released
    lcd.clear();
    beginCount = 1;
  }
}

int irDetect(int irLedPin, int irReceiverPin, long frequency)
{
  tone(irLedPin, frequency, 8);              // IRLED 38 kHz for at least 1 ms
  delay(1);                                  // Wait 1 ms
  int ir = digitalRead(irReceiverPin);       // IR receiver -> ir variable
  delay(1);                                  // Down time before recheck
  return ir;                                 // Return 1 no detect, 0 detect
}

void deter()
{
    tone(13, 3000, 1000);                    // Play tone for 1 second
    digitalWrite(redLed, HIGH);
    lcd.write("GET OUTTA HERE!!!");
    myservo.writeMicroseconds(1300);
    delay(100); 
    digitalWrite(redLed, LOW);
    tone(13, 1500, 1000);                    // Play tone for 1 second
    myservo.writeMicroseconds(1700);
    delay(100);
}
