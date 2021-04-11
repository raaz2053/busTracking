#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
RTClib myRTC;

// GSM
#define gsmPort Serial3 // GSM TX connected to Arduino RX2 pin 17,

// GPS
#include <NMEAGPS.h>
NMEAGPS gps;            // This parses the GPS characters
gps_fix fix;            // This holds on to the latest values
#define gpsPort Serial1 // GPS TX connected to Arduino RX1 pin 19

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define SS_PIN 53
#define RST_PIN 49
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

//Variable to hold the tag's UID
String uidString;
String GPSLocation;
char msg;
void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial3.begin(9600);
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Scan PICC to see UID and type...");
  lcd.init();
  lcd.backlight();
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
}

void readCard()
{
  mfrc522.PICC_ReadCardSerial();
  Serial.print("Tag UID: ");
  uidString = String(mfrc522.uid.uidByte[0]) + " " + String(mfrc522.uid.uidByte[1]) + " " + String(mfrc522.uid.uidByte[2]) + " " + String(mfrc522.uid.uidByte[3]);
  Serial.println(uidString);
  delay(500);
}

void SMS()
{
  DateTime now = myRTC.now();
  delay(100);
  Serial3.println("AT+CMGF=1");                    //Sets the GSM Module in Text Mode
  delay(500);                                      // Delay of 1000 milli seconds or 1 second
  Serial3.println("AT+CMGS=\"+9779804098863\"\r"); // Replace x with mobile number
  // Serial3.println("AT+CMGS=\"+9779807918309\"\r"); // Replace x with mobile number
  delay(100);
  Serial3.println("https://maps.google.com/maps/place/26.655533,87.301943");
  Serial3.print(now.year(), DEC);
  Serial3.print('/');
  Serial3.print(now.month(), DEC);
  Serial3.print('/');
  Serial3.print(now.day(), DEC);
  Serial3.print(' ');
  Serial3.print(now.hour(), DEC);
  Serial3.print(':');
  Serial3.print(now.minute(), DEC);
  delay(100);
  Serial3.println((char)26); // ASCII code of CTRL+Z
  delay(100);
}
void verifyStudent()
{
  if (uidString == "196 70 169 167") //TAG
  {
    Serial.println("IIC Sir");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IIC BUS TRACKER");
    lcd.setCursor(0, 1);
    lcd.print("WELCOME sir!");
    digitalWrite(8, 1);
    SMS();
    delay(3000);
    digitalWrite(8, 0);
    lcd.clear();
    delay(1000);
  }
  else if (uidString == "0 0 0 0") //NULL
  {
    Serial.println("Swipe card");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IIC BUS TRACKER");
    lcd.setCursor(0, 1);
    lcd.print("Swipe your card");
    digitalWrite(8, 0);
    delay(3000);
    lcd.clear();
  }
  else if (uidString == "130 57 23 104") //KUMAR BHAI
  {
    Serial.println("welcome IICian");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IIC BUS TRACKER");
    lcd.setCursor(0, 1);
    lcd.print("Welcome IICian");
    digitalWrite(8, 1);
    SMS();
    delay(3000);
    digitalWrite(8, 0);

    lcd.clear();
  }
  else
  {
    Serial.println("Card Invalid"); //INVALID
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IIC BUS TRACKER");
    lcd.setCursor(0, 1);
    lcd.print("INVALID CARD");
    digitalWrite(11, 1);
    delay(3000);
    digitalWrite(11, 0);

    lcd.clear();
  }
}

void Gpsdata()
{
  if (gps.available(gpsPort))
  {
    fix = gps.read();
    // print whatever you want (or nothing)
    Serial.print(F("Location:"));
    if (fix.valid.location)
    {
      Serial.print(F("https://maps.google.com/maps/place/"));
      Serial.print(fix.latitude(), 6);
      Serial.print(',');
      Serial.print(fix.longitude(), 6);
      //  SendMessage();
    }
    Serial.println();
  }
}

void SendMessage()
{
  Gpsdata();
  delay(1000);
  Serial3.print("Student Checked From:");
  Serial3.print(F("https://maps.google.com/maps/place/")); // The SMS text you want to send
  Serial3.print(fix.latitude(), 6);
  Serial3.print(',');
  Serial3.print(fix.longitude(), 6);
  delay(100);
  Serial3.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial3.println("AT+CMGF=1"); //Sets the GSM Module in Text Mode
  //  Serial3.println("ATD9807918309;"); // ATDxxxxxxxxxx; -- watch out here for semicolon at the end!!
  // Serial.print("calling");/////////////
  delay(1000);                                     // Delay of 1000 milli seconds or 1 second
  Serial3.println("AT+CMGS=\"+9779805347890\"\r"); // Replace x with mobile number
}

void loop()
{

  if (gps.available(gpsPort))
  {
    fix = gps.read();
    // Gpsdata();
    Serial.println("\t");
    Serial.println(".....................");
  }

  Serial.println("Swipe card");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IIC BUS TRACKER");
  lcd.setCursor(0, 1);
  lcd.print("Swipe your card");
  delay(500);
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  // Dump debug info about the card. PICC_HaltA() is automatically called.
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  readCard();
  verifyStudent();

  // For testing, take commands from the Serial Monitor window
  if (gsmPort.available() > 0)
    Serial.write(gsmPort.read());
}
