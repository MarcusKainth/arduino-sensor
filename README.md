# Arduino Sensor
A project for 5CS012 (Collaborative Development) at the University of Wolverhampton.

## Introduction
A temperature sensor using an Arduino UNO board. The code can post data to a server, this can be implement by yourself or you can use the code found at [php-temperature](https://github.com/marcuskainth/php-temperature).

## Usage
The code requires you use the Texas Instruments LM35 sensor, you can also use LEDs if you like but this is not a requirement and the code in theory should work without although I have not tested it.

You are not required to use the code found here [php-temperature](https://github.com/marcuskainth/php-temperature) but if you'd like to test an example that code would be good to do so, I will stress that it is incomplete though but the main concept of grabbing temperautes works. You'll need MySQL for that implementation. There will be a better explaination in the README for that repository.

### Domain
If you'd like to set the sensor up for your own domain/IP, simply change the macro definition on line #18 which is currently set to ['marcuskainth.co.uk'](https://www.marcuskainth.co.uk):
```c
#define ADDR "example.com" // Domain name to resolve
```

### Sensor
You can change the pins used for the sensor on line #24-25, or only set up one sensor to be used.
```c
const int sensorPin1 = A0; // Pin which sensor 1 is connected to
const int sensorPin2 = A1; // Pin which sensor 2 is connected to
```

Also, depending on your board, you may need to make adjustments to the calculations for the sensor but you can find this on the Internet easily. It is currently set up for Arduino Uno 5v.
```c
// Change 5.0 to 3.3 for 3.3v Arduino
temp1 = (temp1 / 1024.0) * 5.0;
temp2 = (temp2 / 1024.0) * 5.0;
temp1 = (temp1 - .5) * 100;
temp2 = (temp2 - .5) * 100;
```

## Requirements
- Arduino (UNO was used here, have not tested other boards)
- Adafruit CC3000
- Texas Instruments LM35
- LEDs (Optional)

## Authors
Created for [University of Wolverhampton](https://www.wlv.ac.uk), [Computer Science](https://www.wlv.ac.uk/about-us/our-schools-and-institutes/faculty-of-science-and-engineering/school-of-mathematics-and-computer-science/) module 5CS012 (Collaborative Development) by:

[Marcus Kainth](https://www.marcuskainth.co.uk), [@marcuskainth](https://github.com/marcuskainth)

Marcus Giddings

Tekali Eyasu