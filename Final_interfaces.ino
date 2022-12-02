/*
 * A01732529 Mario Rodríguez Adams
 * A01732127 Javier Andoni Gonzalez Vásquez
 */
#include <HCSR04.h>
#include <USB.h>

#define INTERNAL_BUTTON  18
#define INTERNAL_LED 3
#define SENSOR_NUM 2

#define TRIG_PIN 0
#define ECHO_PIN_1 1
#define ECHO_PIN_2 2

#define DIST_LOW_LIMIT 2
#define DIST_HIG_LIMIT 10

HCSR04 hc(TRIG_PIN, new int[2]{ECHO_PIN_1, ECHO_PIN_2}, SENSOR_NUM);
USBFS usbDriver;
USBManager USB(usbDriver, 0x1209, 0x2600, "Intefaces", "Control multimedia");

HID_Media usbMedia;

bool buttonStatus = true;
bool pauseStatus = false;
bool volumeStatus = false;
int prevDist1 = 0;
int prevDist2 = 0;

void setup() {
  USB.addDevice(usbMedia);
  USB.begin();
  pinMode(INTERNAL_LED, OUTPUT);
  digitalWrite(INTERNAL_LED, LOW);
  pinMode(INTERNAL_BUTTON, INPUT);
  Serial.begin(9600);
}

void loop() {
  buttonStatus = digitalRead(INTERNAL_BUTTON);
  
  if (buttonStatus == false) {
    if (pauseStatus == false) {
      digitalWrite(INTERNAL_LED, HIGH);
      delay(10);
      usbMedia.pressConsumer(CONSUMER_PAUSE);
    
      while(digitalRead(INTERNAL_BUTTON) == false);
      delay(10);
      usbMedia.releaseConsumer(CONSUMER_PAUSE);
      digitalWrite(INTERNAL_LED, LOW);
      pauseStatus = true;
    } else {
      digitalWrite(INTERNAL_LED, HIGH);
      delay(10);
      usbMedia.pressConsumer(CONSUMER_PLAY);
    
      while(digitalRead(INTERNAL_BUTTON) == false);
      delay(10);
      usbMedia.releaseConsumer(CONSUMER_PLAY);
      digitalWrite(INTERNAL_LED, LOW);
      pauseStatus = false;
    }
  }
  
  for (int i=0; i<SENSOR_NUM; i++) {
    if (hc.dist(i) <= DIST_HIG_LIMIT && hc.dist(i) >= DIST_LOW_LIMIT && volumeStatus == false) {
      switch(i) {
        case 0:
          volumeStatus = true;
          digitalWrite(INTERNAL_LED, HIGH);
          delay(50);
          usbMedia.pressConsumer(CONSUMER_VOLUME_DOWN);
          delay(10);
          usbMedia.releaseConsumer(CONSUMER_VOLUME_DOWN);
          delay(50);
          digitalWrite(INTERNAL_LED, LOW);
          break;
        case 1:
          volumeStatus = true;
          digitalWrite(INTERNAL_LED, HIGH);
          delay(50);
          usbMedia.pressConsumer(CONSUMER_VOLUME_UP);
          delay(10);
          usbMedia.releaseConsumer(CONSUMER_VOLUME_UP);
          delay(50);
          digitalWrite(INTERNAL_LED, LOW);
          break;
      }
    }
    delay(10);
    volumeStatus = false;
  }
  
  delay(60);
}
