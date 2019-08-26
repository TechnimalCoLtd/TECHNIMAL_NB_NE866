#include "TECHNIMAL_NB_NE866.h"
#include "ESPSoftwareSerial.h"

String host = "192.168.1.100"; // Your Server IP
String port = "5683"; // Your Server Port
String token = "otp"; // Your secret token

ESPSoftwareSerial swSer;
TECHNIMAL_NB_NE866 nb;

const long interval = 20000;  //millisecond

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  swSer.begin(9600, 15, 2, SWSERIAL_8N1, false, 95, 11);

  nb.SetupModule(&swSer, host, port);
  if (nb.Connect()) {
    Serial.print(F("# Device IP : "));
    Serial.println(nb.GetDeviceIP());
    nb.Ping();
  }

  nb.SetToken(token);

  previousMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    TECHNIMAL_NB_NE866_COAP_RES resp = nb.Post("/test", "Hello world");
    previousMillis = currentMillis;  
  }
}