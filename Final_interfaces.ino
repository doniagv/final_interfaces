/*
 * A01732529 Mario Rodríguez Adams
 * A01732127 Javier Andoni Gonzalez Vásquez
 */

#include <HCSR04.h>  // Librería para utilizar multiples sensores HC-SR04
#include <USB.h>


#define INTERNAL_BUTTON  18
#define INTERNAL_LED 3
#define SENSOR_NUM 2  // Número de sensores utilizados

#define TRIG_PIN 0 // Pin trigger
#define ECHO_PIN_1 1 // ECHO pin del primer sensor
#define ECHO_PIN_2 2 // ECHO pin del segundo sensor

#define DIST_LOW_LIMIT 2 // Distancia mínima para realizar una acción con el sensor
#define DIST_HIG_LIMIT 10 // Distancia máxima para realizar una acción con el sensor


HCSR04 hc(TRIG_PIN, new int[2]{ECHO_PIN_1, ECHO_PIN_2}, SENSOR_NUM); // Uso de la librería en donde se pasa (TRIG PIN, ARRAY de ECHO PINS, NUMERO DE SENSORES A USAR)
USBFS usbDriver;
USBManager USB(usbDriver, 0x1209, 0x2600, "Intefaces", "Control multimedia");

// Se define usbMedia para usar pressConsumer y releaseConsumer
HID_Media usbMedia;

bool buttonStatus = true; // Status para saber si se presiona el botón de la tarjeta
bool pauseStatus = false; // Status para saber si se necesita poner pausa o play
bool volumeStatus = false; // Status para limitar la interferencia de los inputs de ambos sensores

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
  // Manejo para cuando se presiona el botón de la tarjeta
  if (buttonStatus == false) {
    // Se checa si esta en Pause o Play
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
  
  // Manejo de los sensores, se hace un for loop con el numero de sensores
  for (int i=0; i<SENSOR_NUM; i++) {
    // Se checa la distancia con el método dist() que nos da la librería HCSR04.h y se checa si está en el rango 
    if (hc.dist(i) <= DIST_HIG_LIMIT && hc.dist(i) >= DIST_LOW_LIMIT && volumeStatus == false) {
      // Se usa un switch para ver que sensor se toma
      switch(i) {
        // Si es el sensor 0, se hace usa el comando para bajar el volumen
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
           // Si es el sensor 1, se hace usa el comando para subir el volumen
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
