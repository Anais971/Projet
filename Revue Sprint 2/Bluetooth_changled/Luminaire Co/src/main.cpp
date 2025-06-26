#include <Arduino.h>
#include <FastLED.h>
#include <lib8tion.h>
#include "BluetoothSerial.h"


#define DATA_PIN 23 //pin DI
#define CLOCK_PIN 18 // pin CI
#define NUM_LEDS 11   // Nombre de LEDs dans ton ruban


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
CRGBPalette16 currentPalette = RainbowColors_p;
TBlendType currentBlending = LINEARBLEND;
static uint8_t startIndex = 0;
bool isEffectActive = false;

CRGBPalette16 pyroPalette = CRGBPalette16(
  CRGB::Red, CRGB::OrangeRed, CRGB::DarkOrange, CRGB::Yellow,
  CRGB::Red, CRGB::Orange, CRGB::FireBrick, CRGB::Red,
  CRGB::OrangeRed, CRGB::Orange, CRGB::Red, CRGB::DarkOrange,
  CRGB::Orange, CRGB::Yellow, CRGB::Red, CRGB::Orange);

CRGBPalette16 hydroPalette = CRGBPalette16(
  CRGB::Blue, CRGB::DeepSkyBlue, CRGB::Cyan, CRGB::Aqua,
  CRGB::DodgerBlue, CRGB::LightBlue, CRGB::Teal, CRGB::Blue,
  CRGB::Aqua, CRGB::DeepSkyBlue, CRGB::Blue, CRGB::Teal,
  CRGB::Cyan, CRGB::Blue, CRGB::DodgerBlue, CRGB::LightBlue);

CRGBPalette16 anemoPalette = CRGBPalette16(
  CRGB::Green, CRGB::Aquamarine, CRGB::Lime, CRGB::SpringGreen,
  CRGB::SeaGreen, CRGB::LightSeaGreen, CRGB::MediumAquamarine, CRGB::Green,
  CRGB::Aquamarine, CRGB::LightGreen, CRGB::Green, CRGB::SpringGreen,
  CRGB::Lime, CRGB::LightSeaGreen, CRGB::Green, CRGB::Aquamarine);

CRGBPalette16 electroPalette = CRGBPalette16(
  CRGB::Purple, CRGB::Violet, CRGB::MediumPurple, CRGB::DarkViolet,
  CRGB::Indigo, CRGB::Orchid, CRGB::DarkOrchid, CRGB::Magenta,
  CRGB::Purple, CRGB::Violet, CRGB::MediumPurple, CRGB::DarkViolet,
  CRGB::Indigo, CRGB::Orchid, CRGB::DarkOrchid, CRGB::Magenta);

CRGBPalette16 cryoPalette = CRGBPalette16(
  CRGB::White, CRGB::LightCyan, CRGB::AliceBlue, CRGB::LightBlue,
  CRGB::Snow, CRGB::MintCream, CRGB::GhostWhite, CRGB::Azure,
  CRGB::White, CRGB::LightCyan, CRGB::AliceBlue, CRGB::LightBlue,
  CRGB::Snow, CRGB::MintCream, CRGB::GhostWhite, CRGB::Azure);

CRGBPalette16 geoPalette = CRGBPalette16(
  CRGB::Gold, CRGB::DarkGoldenrod, CRGB::Orange, CRGB::Peru,
  CRGB::Goldenrod, CRGB::SandyBrown, CRGB::Chocolate, CRGB::Gold,
  CRGB::DarkGoldenrod, CRGB::Orange, CRGB::Peru, CRGB::Goldenrod,
  CRGB::SandyBrown, CRGB::Chocolate, CRGB::Orange, CRGB::Gold);

CRGBPalette16 dendroPalette = CRGBPalette16(
  CRGB::ForestGreen, CRGB::DarkOliveGreen, CRGB::LimeGreen, CRGB::OliveDrab,
  CRGB::SeaGreen, CRGB::GreenYellow, CRGB::Chartreuse, CRGB::LawnGreen,
  CRGB::ForestGreen, CRGB::DarkOliveGreen, CRGB::LimeGreen, CRGB::OliveDrab,
  CRGB::SeaGreen, CRGB::GreenYellow, CRGB::Chartreuse, CRGB::LawnGreen);




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

void handleElementEffect(char c) {

  if (c == '\0' || c == '\n' || c == '\r') return;

  isEffectActive = true;
  startIndex = 0;
  switch (c) {
    case 'p': currentPalette = pyroPalette; Serial.println("→ PYRO"); break;
    case 'h': currentPalette = hydroPalette; Serial.println("→ HYDRO"); break;
    case 'a': currentPalette = anemoPalette; Serial.println("→ ANEMO"); break;
    case 'e': currentPalette = electroPalette; Serial.println("→ ELECTRO"); break;
    case 'c': currentPalette = cryoPalette; Serial.println("→ CRYO"); break;
    case 'g': currentPalette = geoPalette; Serial.println("→ GEO"); break;
    case 'd': currentPalette = dendroPalette; Serial.println("→ DENDRO"); break;
    default:  isEffectActive = false; Serial.println("→ Inconnu"); break;
  }

  Serial.print("Effet reçu : ");
  Serial.println(c);

}

void debugPalette(CRGBPalette16 palette) {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t index = map(i, 0, NUM_LEDS - 1, 0, 255);
    leds[i] = ColorFromPalette(palette, index);
  }
  FastLED.show();
  delay(2000);
}

void updateEffectCycle() {
  
   if (isEffectActive) {
    startIndex += 3;
    uint8_t colorIndex = startIndex;
    for (int i = 0; i < NUM_LEDS; i++) {
      CRGB c = ColorFromPalette(currentPalette, colorIndex, 255, LINEARBLEND);
      leds[i] = CRGB(c.b, c.g, c.r); // <- correction ici
      colorIndex += 3;
    }
    FastLED.show();
    delay(100);
  }
  
}



void setup() 
{

  Serial.begin(115200);       // Initialisation du port série pour la communication avec le moniteur série
  SerialBT.begin("PaimonLight"); // Nom de l'appareil Bluetooth
  Serial.println("Initialisation Bluetooth...");

  // Attendre que la connexion Bluetooth soit établie
  while (!SerialBT.available())
  {
    delay(100); // Attendre une connexion
  }
  Serial.println("Connecté à un appareil Bluetooth.");

  delay(500); // power-up safety delay
    // Two strips of LEDs, one in HD mode, one in software gamma mode.
  FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  // indique la ref de la led, on donne son DI et CI 
  FastLED.setBrightness(150); //111+11111 = 255 // à voir 
  

  




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

  if (recep != '\0') {
    handleElementEffect(recep);
  }

  updateEffectCycle();

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
