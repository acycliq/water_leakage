#include <SPI.h>
#include <Ethernet.h>

/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-water-sensor
 */

#define POWER_PIN  7
#define SIGNAL_PIN A5
#define THRESHOLD   300

// replace the MAC address below by the MAC address printed on a sticker on the Arduino Shield 2
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x73, 0xAE };

EthernetClient client;

int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET";
char   HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME   = "/trigger/send-email/with/key/nw_KyAEC___okvAwcj-v0qR0HBo2ey2cglcM9lTqLGP"; // change your Webhooks key
String queryString = "?value1=27"; // the supposed temperature value is 27°C
boolean IS_FIRST_TIME = true;


int value = 0; // variable to store the sensor value
unsigned long previousMillis = 0; // will store last time an email was sent out
const long interval = 60000;           // interval at which to blink (milliseconds)


void setup() {
  Serial.begin(9600);
  ini_ethernet_shield();
  pinMode(POWER_PIN, OUTPUT);   // configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
}

void send_email() {
  // connect to web server on port 80:
  if (client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header

    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {// if not connected:
    Serial.println("connection failed");
  }
}

void ini_ethernet_shield() {
  // initialize the Ethernet shield using DHCP:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to obtaining an IP address using DHCP");
    while (true);
  }
  else {
    Serial.println("obtained an IP address using DHCP");
  }
}

unsigned long time_delta(unsigned long t0, unsigned long t1){
  unsigned long out;
  if (IS_FIRST_TIME){
    // the first event might happen before the time set by interval. In that case do not calc the time diff.
    // just return a value high enough to trigger the action
    out = interval + 1.0;
    IS_FIRST_TIME = false;
  }
  else{
    out = t1 - t0;
  }
  return out;
}

void loop() {
  unsigned long currentMillis = millis();

  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("Sensor value: ");
  Serial.println(value);
  

  if (value > THRESHOLD) {
    unsigned long dt = time_delta(previousMillis, currentMillis);
    Serial.println(dt);
    if (dt >= interval) {
      previousMillis = currentMillis;
      send_email();
    }
  } 

  delay(1000);
}
