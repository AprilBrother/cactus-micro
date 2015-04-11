#include <SoftwareSerial.h>

//period between posts, set at 60 seconds
#define DELAY_PERIOD 30000

// Important!! We use pin 13 for enable esp8266  
#define WIFI_ENABLE_PIN 13

// TODO
#define SSID         "siyue-r2-1110"
#define PASS         "siyuexiongdi"

#define HOST         "192.168.60.225"
#define HOST_NAME    "localhost"

#define DEBUG         1

int wifiConnected = 0;
long nextTime;

SoftwareSerial mySerial(11, 12); // rx, tx

void setup()
{

    mySerial.begin(9600);   //connection to ESP8266
    Serial.begin(9600);     //serial debug

    // For atmega32u4, Please set DEBUG = 0 if it not connected to USB
    if(DEBUG) {
        while(!Serial);
    }

    pinMode(WIFI_ENABLE_PIN, OUTPUT);
    digitalWrite(WIFI_ENABLE_PIN, HIGH);

    delay(1000);
    //set mode needed for new boards
    mySerial.println("AT+RST");
    delay(3000);//delay after mode change       
    mySerial.println("AT+CWMODE=1");
    delay(300);
    mySerial.println("AT+RST");
    delay(2000);
    // Sinlge connection
    mySerial.println("AT+CIPMUX=0");
    delay(500);

    nextTime = millis(); //reset the timer
}

boolean connectWifi() {     
    String cmd = "AT+CWJAP=\"";
    cmd.concat(SSID);
    cmd.concat("\",\"");
    cmd.concat(PASS);
    cmd.concat("\"");
    Serial.println(cmd);
    mySerial.println(cmd);

    for(int i = 0; i < 20; i++) {
        Serial.print(".");
        if(mySerial.find("OK")) {
            wifiConnected = 1;
            break;
        }    
        delay(50);
    }

    Serial.println(wifiConnected ? "Connected to WiFi OK." : "Can not connect to the WiFi.");
    return wifiConnected;
}

void loop() {
    if(!wifiConnected) {
        mySerial.println("AT");
        delay(1000);
        if(mySerial.find("OK")){
            Serial.println("Module Test: OK");
            connectWifi();
        } 
    }

    if(!wifiConnected) {
        delay(500);
        return;
    }

    //wait for timer to expire
    if(nextTime < millis()){
        Serial.print("timer reset: ");
        Serial.println(nextTime);

        httpPost("test=123");

        //reset timer
        nextTime = millis() + DELAY_PERIOD;       
    }

}


void printlnDebug(String data) {
    if (DEBUG) {
        Serial.println(data);
    }
    mySerial.println(data);
}

void printDebug(String data) {
    if (DEBUG) {
        Serial.print(data);
    }
    mySerial.print(data);
}


//web request needs to be sent without the http for now, https still needs some working

void httpPost(String data) {    
    Serial.println("Sending data...");

    String startCmd = "AT+CIPSTART=\"TCP\",\"";
    startCmd.concat(HOST);
    startCmd.concat("\", 8080");
    printlnDebug(startCmd);

    //test for a start error
    if(mySerial.find("Error")){
        Serial.println("error on start");
        return;
    }

    //send to ESP8266
    printDebug("AT+CIPSEND=");
    printlnDebug(String(data.length()));

    if(mySerial.find(">")) {
        //create the request command
        printlnDebug("POST /event.php");
        printDebug("Host: "); 
        printlnDebug(HOST_NAME);
        printDebug("Accept: *");
        printDebug("/");
        printlnDebug("*");
        printDebug("U-ApiKey: ");
        printlnDebug(String(data.length()));
        printlnDebug("Content-Type: application/x-www-form-urlencoded");
        printlnDebug("Connection: close");
        printlnDebug("");  

        // Send data
        printlnDebug(data);
        delay(1500);
        printlnDebug("AT+CIPCLOSE");

        //output everything from ESP8266 to the Arduino Micro Serial output
        delay(1500);
        while (mySerial.available() > 0) {
          Serial.write(mySerial.read());
        }
    } else {
        printlnDebug("AT+CIPCLOSE");
        Serial.println("Connect timeout");
        delay(1000);
        return;
    }

}


