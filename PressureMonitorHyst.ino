/* ---------------------------------------------------------------
    I am trying to record a reading when the reading from an air
    pressure sensor jumps suddenly - ie when a car drives over a tube
    attached to the sensor. The Pressure Sensor is a MPX5500DP 
    I am using the movingAvg library to simplify the maths a bit.
   ---------------------------------------------------------------*/

#include <movingAvg.h> // https://github.com/JChristensen/movingAvg
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RV-3028-C7.h>                 // this is a low-power clock

const uint8_t airSensor(A0);  // connect pressure sensor from A0 pin to ground
movingAvg airAvg(20);        // define the moving average object

RV3028 rtc;                  // get the clock going

String timestamp;           //

int active;   
const int chipSelect = 8;      // SD card pin
int ledPin = 5;                // the pin the LED is connected to

File logFile; // the logging file



/* --------------------------------------------
/*  setup the average, pullup the air sensor, 
 *   begin the serial monitor and show an initial 
 *   reading so we knnow it is working
   --------------------------------------------*/
void setup()
{
  Wire.begin();
     Serial.begin(9600);                 // begin serial monitor
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
   active = false ;                   // setting up the hysteresis
   
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
 //   return;
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
 *  the moving average, and if it is greater than 
 *  the specific amount, print this to the monitor.
 *  when the threshold is reached, active is set 
 *  so it can't be triggered again until it falls 
 *  below the specific amount, stopping cars being
 *  counted multiple times eg if they stop on the tube
 *  for a length of time.
   --------------------------------------------*/
void loop()
{
   rtc.updateTime();                       // get the time
   int pc = analogRead(airSensor);        // read the sensor
   int avg = airAvg.reading(pc);          // calculate the moving average
   int avgPlus2 = avg + 2;               // to simplify conditional below
//   unsigned long new_time = millis();    // this is to make sure peaks are spaced, in case a single count causes a double spike
 
 delay(1);   // For some reason, the If statement that follows doesn't work without a delay here?????
   
   if ((pc > avgPlus2) && !active)  
   {

    // write data to serial
    Serial.print(rtc.stringDate());
    Serial.print(" ");
    Serial.print(rtc.stringTime());
    Serial.print(", ");
    Serial.print(pc);
    Serial.print(", ");
    Serial.println(avg);
    
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

    active = true ;
   }
   
    if (pc < (avg+1)) 
   {
     active = false ;
   }
}

 
