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

  //ici pour essayer un changement de couleur qui défile
  // else { (on mais à l'exterieur) dyna = blu; }

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


/*void dynamicled(int r, int g, int b){
 if(dyna = "a"){
 mode dynamique elementaire ?

animation_step++;

// Si on atteint la fin du cycle (valeur max), on recommence à 0
  if (animation_step >= 1530) {
      animation_step = 0;
  }

// Stocke la valeur courante de l’animation (entre 0 et 1530)
int autosli = animation_step;
    

}*/

static int redled(int sli)// faire une fonction pour tous les elements
{
  int red;     // valeur de sortie
  int slideb;  // base de décalage sur l’axe du slider
  int Y;       // valeur de référence pour le calcul

  // 1re phase : Vert augmente, Rouge reste à 255
  if ((sli >= 0) && (sli < 1*255)) {
    red = 255;
  }

  // 2e phase : Rouge descend, Vert reste
  else if ((sli >= 1*255) && (sli < 2*255)) {
    slideb = 1*255;
    Y = 255;
    red = A * (sli - slideb) + Y;
  }

  // 5e phase : Rouge remonte, Bleu reste
  else if ((sli >= 4*255) && (sli < 5*255)) {
    slideb = 4*255;
    Y = 0;
    red = -A * (sli - slideb) + Y;
  }

  // 6e phase : Bleu descend, Rouge reste
  else if ((sli >= 5*255) && (sli < 6*255)) {
    red = 255;
  }

  // Valeur par défaut (en dehors des plages définies)
  else {
    red = 0;
  }

  return red;
}

static int redled(int sli)// faire une fonction pour tous les elements
{
  int red;     // valeur de sortie
  int slideb;  // base de décalage sur l’axe du slider
  int Y;       // valeur de référence pour le calcul

  // 1re phase : Vert augmente, Rouge reste à 255
  if ((sli >= 0) && (sli < 1*255)) {
    red = 255;
  }

  // 2e phase : Rouge descend, Vert reste
  else if ((sli >= 1*255) && (sli < 2*255)) {
    slideb = 1*255;
    Y = 255;
    red = A * (sli - slideb) + Y;
  }

  // 5e phase : Rouge remonte, Bleu reste
  else if ((sli >= 4*255) && (sli < 5*255)) {
    slideb = 4*255;
    Y = 0;
    red = -A * (sli - slideb) + Y;
  }

  // 6e phase : Bleu descend, Rouge reste
  else if ((sli >= 5*255) && (sli < 6*255)) {
    red = 255;
  }

  // Valeur par défaut (en dehors des plages définies)
  else {
    red = 0;
  }

  return red;
}

// Fonction pour calculer la valeur du canal vert
static int greenled(int sli)
{
  int gre;
  int slideb;
  int Y;

  // 1re phase : Vert monte
  if ((sli >= 0) && (sli < 1*255)) {
    slideb = 0;
    Y = 0;
    gre = -A * (sli - slideb) + Y;
  }

  // 2e phase : Rouge descend, Vert reste
  else if ((sli >= 1*255) && (sli < 2*255)) {
    gre = 255;
  }

  // 3e phase : Bleu monte, Vert reste
  else if ((sli >= 2*255) && (sli < 3*255)) {
    gre = 255;
  }

  // 4e phase : Vert descend
  else if ((sli >= 3*255) && (sli < 4*255)) {
    slideb = 3*255;
    Y = 255;
    gre = A * (sli - slideb) + Y;
  }

  else {
    gre = 0;
  }

  return gre;
}


// Fonction pour calculer la valeur du canal bleu
static int blueled(int sli)
{
  int blue;
  int slideb;
  int Y;

  // 3e phase : Bleu monte
  if ((sli >= 2*255) && (sli < 3*255)) {
    slideb = 2*255;
    Y = 0;
    blue = -A * (sli - slideb) + Y;
  }

  // 4e phase : Vert descend, Bleu reste
  else if ((sli >= 3*255) && (sli < 4*255)) {
    blue = 255;
  }

  // 5e phase : Rouge monte, Bleu reste
  else if ((sli >= 4*255) && (sli < 5*255)) {
    blue = 255;
  }

  // 6e phase : Bleu descend
  else if ((sli >= 5*255) && (sli < 6*255)) {
    slideb = 5*255;
    Y = 255;
    blue = A * (sli - slideb) + Y;
  }

  else {
    blue = 0;
  }

  return blue;
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
