# ArduinoCC
Arduino Car Counter

A rubber tube connected to a pressure sensor. Cars travelling over the tubing change the pressure, which is then recorded by the arduino.

pressure_test.ino can be used to calibrate the pressure readings

PressureMonitorImp.ino registers cars travelling over the rubber tube, and writes the time and date of each trigger to a microSD card

## Project description

While talking about electronics at work, my colleague metnioned a car counter he’d bought for a large sum of money that never really worked. I had the idea that I could build one myself, from Arduino, and thought I’d give it a try.

Internet searching found the following projects that I used as inspiration:

[Arudino people counting, using PIR (passive infra-red)](https://www.dbpharrison.com/projects/interactivefloor/arduinopeoplecounter1/)

[Kris Temmermen’s car counter on Hackaday](https://hackaday.io/project/4567-traffic-counter-road-tube)

Plus a one more that is  now gone and only available on internet archive: [makercave](https://web.archive.org/web/20141222170015/https://wiki.makercave.org/wiki/Arduino_Vehicle_Traffic_Counter), and [tomorrow-lab](http://www.old.tomorrow-lab.com/lab16) which doesn't have all the files any more. All of these sites were incredibly useful, both for ideas, inspiration and of course code help.

The plan was to use a pressure sensor to record when there is a change in pressure inside a sealed rubber tube, such as when a car drives over it. Simple! I wanted to make it as low-power as possible, so it would run for a good while before needing batteries changed. It’s to be deployed in a rural location, counting cars coming in and out of a car park.

There was then weeks AND WEEKS of experimenting, testing and trialling, but here’s what I ended up with.  

## Kit
- Arduino Pro Mini 328 - 5V/16MHz
- 4mm internal diameter rubber fuel hose
- Pelicase 1120
- DC POWER JACK 2.1MM BARREL-TYPE PCB MOUNT
- Hook Up Wire Kit, 22 AWG
- Barrel jack power switch
- clear plastic tube, 4mm internal diamter
- SparkFun Level Shifting microSD Breakout
- RV 3028 RTC Breakout
- 4mm double headed bulkhead connector
- 4mm hose T-connector
- Flash Memory Card, MicroSD Card
- MPX5100DP Pressure Sensor

## Connecting it up:
The Arduino Pro Mini has offset A4 (SDA) and A5 (SCL) pins. This is a pain when using a breadboard because they don’t line up with any holes. The trick is to solder them to A2 and A3 respectively, turning those pins into SDA and SCL. It’s a bit fiddly! Here’s what it looks like with my soldering skills:

![ea49c5e9-5237-4ac8-aecd-99db6db747b0](https://user-images.githubusercontent.com/46293785/213927734-a5b2e295-23e3-4adb-9e8a-055d892fbb13.jpg)

Soldering A2 to A4 and A3 to A5

Breadboard everything first, so you can see how things connect up. Later, when you’re happy everything works, you can arrange them on a prototype board for soldering.

Arduino Pro Mini has two power pins: VCC and RAW. RAW is power in, which gets converted by the Arduino to 5V. The closer to 5V your power in is, the more efficient the Arduino will run. Take the power from the Arduino VCC pin for everything else.

Here’s how the RV3028 RTC connects to the arduino:

Arduino - RV3028 RTC
A5 (SCL) - SCL
A4 (SDA) - SDA
Gnd - Gnd
VCC - 2-5V
2 - INT

Let’s connect up the Sparkmatch Shifting microSD cardbreakout:

Arduino - MicroSD breakout
GND - GND
9 - CD
13 - DO
12 - SCK
11 - DI
8 - CS
VCC - VCC

Finally the Pressure sensor. This has 6 pins and two air ports. But we’re only going to use three pins, and attach the hose to one ofthe ports. Which ones?

According to the data sheet for the MPX5100DP the pin with a bite taken out of it is pin 1. Here’s how to wire it to the arduino:.

Arduino - MPX5100DP Pin
A0 - 1 (Vout)
GND - 2 (GND)
VCC - 3 (Vs)

The pressure sensor works by measuring the pressure in one of the ports, and comparing it to the pressure in the other port. The difference is returned as a voltage between 0 and 5. This can then be converted to Kpa or mb but we don’t really need to know that, so we’ll just use the raw values the pressure sensor returns to us.

(When I first set this up, I used the MPX5500. This measures up to 500kPa, which means it wasn’t sensitive enough to recognise a lot of cars. I switched to the MPX5100, which measures up to 100kPa, and the sensitivity was vastly improved. The company, NXP have a [forum on their website](https://community.nxp.com/) which was really helpful in figuring this out).

## Code
The first code, pressure_test.ino, is used to calibrate the sensor. Set this up, and if it runs, blow into the tubes on the pressure sensor. The readings will display on the serial monitor. Make sure you see which port is measuring pressure, and how much it’s changing by. Have a look at the csv file too.

The main code is PressureMonitorImp.ino. This  code waits until the reading from the pressure sensor is a certain amount above the average of the past twenty readings. This means it measures the data spikes. It also waits 400ms between readings, so for example, a car taking a while to cross the tube doesn’t register as many. (I think this could be improved with a debounce or similar instead however).

Blow into it and see if it registers as a hit on the serial monitor and that it's written to the csv on the SD card.

## Housing
Drill a hole in the plastic housing case for the bulkhead connector, and seal it in place. To fix my breadboard I glued a bit of wood inside the case, very hi-tec. I snipped small bits of the black tube and used them as spacers under the prototyping boar.! Use a short length of the small transparent tubing to connect the correct pressure nozzle to the bulkhead connector. Connect up the switch and the battery, the black rubber tube to the outside and it is ready to deploy

![housing](https://user-images.githubusercontent.com/46293785/213927512-b8d7897e-858f-4851-a5f5-a51b65d03545.jpg)

Fix your rubber tube to the road (I just use rawl plugs and saddle clips), and seal the other end. You can do this by using the T-connector at the end, and then fit a short piece of tube from onearm of the T to the other in a D-shape. Or just put some sealant in there and a screw, or however you like.

## Testing and Calibrating
With the first piece of code uploaded, take it outside, switch it on, and drive over the tube several times. Switch it off, and have a look at the.csv file on the microSD card to see what is happening. Every 10ms, a row will have been recorded, showing the date, pressure recorded and average pressure from the past twenty readings. By adding ina column simply counting the rows, and then dividing it by 100, I was able to make this graph showing the spikes by my car and my truck (and me) going overthe tube, with the number of seconds on the X axis:

![graph_annotated](https://user-images.githubusercontent.com/46293785/213927589-e6cc593c-daa3-4e46-8913-950740d5b2f1.JPG)

Looking at this, the easiest would surely be to record a car whenever the reading is equal or greater than 60. However, we’re in the middle of a cold winter and I’m not sure how the pressure readings will change in hot, sunny weather – will the pressure in the tube increase greatly? So instead, I’m going to take a reading whenever the pressure is more than 7 above the average of the last ten readings.

You can also see a peak for each wheel axle. I looked at the data, and the average time between each axle is 0.345s (min 0.22s, max 0.56s) so I think a delay of 200ms (0.20s) should work OK to stop a single axle being recorded more than once. This is only from a sample of 7 vehicle triggers though, so I might change this. 

As for each vehicle causing two spikes (one for each axle) – the easiest way to deal with this is simply to divide the final number by two when you get the data off the SD card.

Once you’ve decided where you are going to measure cars, the black tube can be held in place with saddle-clips fixed to the road with screws and rawlplugs. Try to position them so they won’t cause a puncture. For slow traffic (which I’m using, going in and out of a car park on a narrow track) this should be enough, but faster traffic may stretch and move the tube.

Set everything up, and try it out, see what data you get back. I am able to use a motion-activated camera and leave it in place for a few days, then compare the number of cars photographed vs the numbers recorded, but be wary of this (particularly in Europe) as you will need to consider data protection laws if you can’t put the camera where the reg plate isn’t recorded.

Instead you might need to sit and count cars for a few hours! Keep adjusting the triggers, both the moving average and the trigger value, until it seems to work.

## Power consumption
I don’t have a way of measuring low power, but I calculated the power consumption from the components as using 193mA when triggered, and 11.3mA when at rest. I had it running for 147 hours off 1900mAh batteries (12mA average), but there weren’t many cars to trigger it. There are loads of ways to further reduce power consumption of an Arduino pro mini (see https://www.gammon.com.au/power), but this is low enough for me for now.

