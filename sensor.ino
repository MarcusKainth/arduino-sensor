#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include <stdlib.h>

// Define CC3000 chip pins
#define ADAFRUIT_CC3000_IRQ  3
#define ADAFRUIT_CC3000_VBAT 5
#define ADAFRUIT_CC3000_CS 10

// WiFi network
#define WLAN_SSID  "undef"
#define WLAN_PASS  "undef"
#define WLAN_SECURITY  WLAN_SEC_WPA2 // WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2

#define ADDR "marcuskainth.co.uk" // Domain name to resolve

const unsigned long dhcpTimeout = 60L * 1000L; // Max time to wait for address from DHCP
const unsigned long connectTimeout = 15L * 1000L; // Max time to wait for server connection
const unsigned long responseTimeout = 15L * 1000L; // Max time to wait for data from server

const int sensorPin1 = A0; // Pin which sensor 1 is connected to
const int sensorPin2 = A1; // Pin which sensor 2 is connected to

uint32_t t; // Variable used for timeout

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ,
ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV2);

// Server's IP for posting data, we'll set this using the domain instead.
uint32_t ip = 0;

Adafruit_CC3000_Client client;

/**
 * Initialise the Arduino board here. Also set's up the
 * Wi-Fi module and resolves the domain using DNS from
 * DHCP. The LEDs are also initialised here.
 */
void setup() {
    Serial.begin(115200);
    Serial.print(F("Initialising..."));
    
    if(!cc3000.begin()) {
        Serial.println(F("Failed! Check your wiring?"));
        return;
    }
    
    Serial.println(F("OK!\r\nConnecting to network..."));
    cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
    Serial.println(F("connected!"));
    Serial.println(F("Requesting address from DHCP server..."));
    
    for(t = millis(); !cc3000.checkDHCP() && ((millis() - t) < dhcpTimeout); delay(500)) {
        Serial.println("....waiting");
    }
    
    if(cc3000.checkDHCP()) {
        Serial.println(F("OK"));
        while (ip == 0) {
            if (!cc3000.getHostByName(ADDR, &ip)) {
                Serial.println(F("Couldn't resolve!"));
                while(1) {}
            }
            
            delay(500);
        }
    } else {
        Serial.println(F("failed"));
        return;
    }
    
    // Declare pins used for LEDs
    pinMode(22, OUTPUT); // GREEN
    pinMode(24, OUTPUT); // ORANGE
    pinMode(26, OUTPUT); // RED
}

/**
 * Create an HTTP request using GET to the server.
 * For information on how to use this using the existing
 * PHP code, please refer to
 * https://github.com/marcuskainth/php-temperature
 *
 * @param temp1 The first temperature value to post
 * @param temp2 The second temperature value to post
 */
void postData(float temp1, float temp2) {
    /**
     * Attempt to connect to the server until we
     * timeout. Check your internet connection or
     * the server if you reach that stage.
     */
    do {
        client = cc3000.connectTCP(ip, 80);
    } while((!client.connected()) && ((millis() - t) < connectTimeout));

    /**
     * When we are connected we can send the data
     * to the server to be store in a database etc...
     */
    if(client.connected()) {
        client.print("GET /arduino/");
        client.print("get.php");
        client.print("?temp1=");
        client.print(temp1);
        client.print("&temp2=");
        client.print(temp2);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(ADDR);
        client.println("Connection: close");
        client.println();
        
        /**
         * Print the response from the server to check
         * if the data has been sent. We could add a value
         * to check here but that's entirely up to you.
         */
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.print(c);
            }
        }
        
        client.close();
        Serial.println();
    } else {
        Serial.println("failed");
    }
}

/**
 * This function is ran continuously until the program
 * is forced to close. Temperature will constanly be
 * sent to the server.
 */
void loop() {
    float temp1 = analogRead(sensorPin1);
    float temp2 = analogRead(sensorPin2);
    
    // Change 5.0 to 3.3 for 3.3v Arduino
    temp1 = (temp1 / 1024.0) * 5.0;
    temp2 = (temp2 / 1024.0) * 5.0;
    temp1 = (temp1 - .5) * 100;
    temp2 = (temp2 - .5) * 100;
    
    // Coversion to Celsius if required, just uncomment
    // temp1 = ((temp1 * 9) / 5) + 32;
    // temp2 = ((temp2 * 9) / 5) + 32;
    
    // Temperatures posted are in Fahrenheit
    postData(temp1, temp2);
    
    Serial.print("Temperature Sensor 1: ");
    Serial.print(temp1, 1);
    Serial.print("°F");
    Serial.println();
    delay(1000);
    
    Serial.print("Temperature Sensor 2: ");
    Serial.print(temp2, 1);
    Serial.print("°F");
    Serial.println();
    delay(1000);
    
    /**
     * These are the hard-coded values for the LEDs
     * taken from an average from both sensors. An
     * out-of-scope idea was to implement the ranges
     * from the website instead of hard-coding but we
     * didn't get time.
     */
    if ((temp1 + temp2) / 2 > 25) {
        digitalWrite(26, HIGH);
        digitalWrite(24, LOW);
        digitalWrite(22, LOW);
    } else if ((temp1 + temp2) / 2 > 15 && (temp1 + temp2) / 2 < 24) {
        digitalWrite(24, HIGH);
        digitalWrite(26, LOW);
        digitalWrite(22, LOW);
    } else if ((temp1 + temp2) / 2 < 15) {
        digitalWrite(22, HIGH);
        digitalWrite(24, LOW);
        digitalWrite(26, LOW);
    }
}
