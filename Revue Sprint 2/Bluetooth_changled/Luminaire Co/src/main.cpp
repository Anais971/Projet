#include <Arduino.h>
#include <FastLED.h>
#include <lib8tion.h>
#include "BluetoothSerial.h"


#define DATA_PIN 23 //pin DI
#define CLOCK_PIN 18 // pin CI
#define NUM_LEDS 60   // Nombre de LEDs dans ton ruban


// Prépare le tableau de données pour 1 LED
// cLed contient toute l'addresse à envoyer

// LED *4 pour les 8 bits R, B, G et le (111) et le 5 bits pour l'intensité

BluetoothSerial SerialBT;
char recblu[7]= "";
char tblue[3]= "";
char tgreen[3]= "";
char tred[3]= "";
bool isReceiving = false;
int recIndex = 0;

CRGB leds[NUM_LEDS] = {0}; // initialise le tableau de led en fonction du nombre de led


char bluetoothled()
{
  if (SerialBT.available()) {
    char blue = SerialBT.read();
    Serial.print("Reçu via BT : ");
    Serial.println(blue);
    return blue;
  } else {
    return '\0'; // caractère vide si rien reçu
  }
}

void recupled(char blu)
{

  if (blu == '#') {
    isReceiving = true;
    recIndex = 0;
    memset(recblu, 0, sizeof(recblu)); // Réinitialise le tableau
    return;
  }

  if (isReceiving && recIndex < 6 && blu != '\0') {
    recblu[recIndex++] = blu;

    if (recIndex == 6) {
      recblu[6] = '\0';
      isReceiving = false; // Terminé, prêt à traiter
    }
  }

}

void tRGB() {
  strncpy(tred, recblu, 2);
  strncpy(tgreen, recblu + 2, 2);
  strncpy(tblue, recblu + 4, 2);

  tred[2] = '\0';
  tgreen[2] = '\0';
  tblue[2] = '\0';
}


// Convertit HEX en nombre
void cHex(int &r, int &g, int &b) {
  r = strtol(tred, NULL, 16);
  g = strtol(tgreen, NULL, 16);
  b = strtol(tblue, NULL, 16);
}

// Applique la couleur aux LEDs
void envoieled(int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    CRGB c = CRGB(b, g, r);
    leds[i] = c;
  }
  FastLED.show();
}







void setup() 
{

  Serial.begin(115200);       // Initialisation du port série pour la communication avec le moniteur série
  SerialBT.begin("ESP32 AB"); // Nom de l'appareil Bluetooth
  Serial.println("Initialisation Bluetooth...");

  // Attendre que la connexion Bluetooth soit établie
  while (!SerialBT.available())
  {
    delay(100); // Attendre une connexion
  }
  Serial.println("Connecté à un appareil Bluetooth.");

  delay(500); // power-up safety delay
    // Two strips of LEDs, one in HD mode, one in software gamma mode.
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  // indique la ref de la led, on donne son DI et CI 
  FastLED.setBrightness(100); //111+11111 = 255 // à voir 
  

  




}

uint8_t wrap_8bit(int i) {
    // Modulo % operator here wraps a large "i" so that it is
    // always in [0, 255] range when returned. For example, if
    // "i" is 256, then this will return 0. If "i" is 257,
    // then this will return 1. No matter how big the "i" is, the
    // output range will always be [0, 255]
    return i % 256;
}



void loop() 
{

  char recep = bluetoothled();
  recupled(recep);

  if (recIndex == 6 && !isReceiving){

    tRGB();

    int r, g, b;
    cHex(r, g, b);

    Serial.printf("R=%d, G=%d, B=%d\n", r, g, b);

    envoieled(r, g, b);

    recIndex = 0;
    memset(recblu, 0, sizeof(recblu));
  
  }


  /*for (int i = 0; i < NUM_LEDS; i++) {
      
      CRGB c(227, 0, 189);  // Just make a shade of white.
     leds[i] = c;
  }
  FastLED.show();  // All LEDs are now displayed.
  delay(1000); 

  for (int i = 0; i < NUM_LEDS; i++) {
      
      CRGB c(255, 150, 50);  // Just make a shade of white.
     leds[i] = c;
  }
  FastLED.show();  // All LEDs are now displayed.
  delay(1000); 

  for (int i = 0; i < NUM_LEDS; i++) {
      
      CRGB c(200, 100, 255);  // Just make a shade of white.
     leds[i] = c;
  }
  FastLED.show();  // All LEDs are now displayed.
  delay(1000); */

}
