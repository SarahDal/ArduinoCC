/* ---------------------------------------------------------------
    I am trying to record a reading when the reading from an air
    pressure sensor jumps suddenly - ie when a car drives over a tube
    attached to the sensor. The Pressure Sensor is a MPX5500DP 
    I am using the movingAvg library to simplify the maths a bit.

    This is V2, which should have displayed the count on a OLED,
    but there wasn't enough memory! So I gave up. 
   ---------------------------------------------------------------*/

#include <movingAvg.h>  // https://github.com/JChristensen/movingAvg
#include <SD.h>
#include <SPI.h> 
#include <Wire.h>
#include <RV-3028-C7.h>        // https://github.com/constiko/RV-3028_C7-Arduino_Library
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const uint8_t airSensor(A0);  // connect pressure sensor from A0 pin to ground

movingAvg airAvg(20);         // This is the moving average - change the figure in brackets to what you want

unsigned long new_time=0;     // set some variables for preventing reading multiple spikes
unsigned long old_time=0;     // 

RV3028 rtc;                   // get the clock going

int pressureCount = 0;        // Start the count of pressure readings
int carCount = 0;

String timestamp;             // 
     
const int chipSelect = 8;     // SD card pin
int ledPin = 5;               // the pin the LED is connected to

File logFile; // the logging file

/* --------------------------------------------
/*  setup the average, pullup the air sensor, 
 *   begin the serial monitor and show an initial 
 *   reading so we knnow it is working
   --------------------------------------------*/
void setup(){
  Serial.begin(9600);                 // begin serial monitor

// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
 if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
 for(;;); // Don't proceed, loop forever
 }
 

  Wire.begin();
     if (rtc.begin() == false)
    {
     Serial.println("Something went wrong, check wiring");
     while (1);
    }
    else
      Serial.println("RTC online!");
    
  delay(1000);
  pinMode(airSensor, INPUT_PULLUP);   // air sensor

  airAvg.begin();                     //averages

   pinMode(chipSelect, OUTPUT); 
   digitalWrite(chipSelect, HIGH); //ALWAYS pullup the ChipSelect pin with the SD library
   delay(100);
   
  // initialize the SD card
  Serial.print("Initializing SD card...");

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(8, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
   Serial.println("Card failed, or not present");
    // don't do anything more:
    //return;
  }
  Serial.println("Card initialized.");
  Serial.print("Logging to: ");
  Serial.println("TRAFFIC.CSV");
  logFile = SD.open("TRAFFIC.CSV", FILE_WRITE);
  logFile.println("");
  logFile.println("NEW SESSION");
  logFile.close();

  Serial.println("Setup complete");
  Serial.println("initial reading");
  int pc = analogRead(airSensor); // read the sensor
  Serial.println(pc);
  
}

/* --------------------------------------------
/*  Each loop should comapare the reading against 
 *   the moving average, and if it is greater than 
 *   the specific amount, print this to the monitor
   --------------------------------------------*/
void loop()
{
   rtc.updateTime();                       // get the time
   int pc = analogRead(airSensor);        // read the sensor
   int avg = airAvg.reading(pc);          // calculate the moving average
   int avgPlus = avg + 5;               // to simplify conditional below
   unsigned long new_time = millis();    // this is to make sure peaks are spaced, in case a single count causes a double spike
 
 // if the reading is greater than the average & however many ms has passed since last time, print it. 
 // This is the ms value between peaks - change it to help calibrate your counter
   if ((pc > avgPlus) && ((new_time - old_time) > 400))  
   {
    pressureCount++;
    
    // write data to serial
    Serial.print(rtc.stringDate());
    Serial.print(" ");
    Serial.print(rtc.stringTime());
    Serial.print(", ");
    Serial.print(pc);
    Serial.print(", ");
    Serial.println(avg);
    Serial.print("Count: ");
    Serial.println(pressureCount);
    Serial.print(", Car Count: ");
    Serial.println(carCount);
    
    logFile = SD.open("TRAFFIC.CSV", FILE_WRITE); // open TRAFFIC.CSV file on SD Card and write to it
    Serial.println("log");
    logFile.print(rtc.stringDate()); 
    logFile.print(" ");
    logFile.print(rtc.stringTime());
    logFile.print(", ");
    logFile.print(pc);
    logFile.print(", ");
    logFile.println(avg);
    logFile.close();
    Serial.println("done.");
   
    old_time = new_time;  // spacing spikes

   }
   else
   {
      delay(1);    // this is needed for some reason to make the IF statement work
   
   }
// display count of cars on OLED
  carCount = (pressureCount/4);
  display.setTextSize(4); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(carCount);
}

 
