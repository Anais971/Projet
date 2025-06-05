#include <Arduino.h>
#include <FastLED.h>
#include <lib8tion.h>

#define DATA_PIN 23 //pin DI
#define CLOCK_PIN 18 // pin CI
#define NUM_LEDS 60   // Nombre de LEDs dans ton ruban


// Prépare le tableau de données pour 1 LED
// cLed contient toute l'addresse à envoyer

// LED *4 pour les 8 bits R, B, G et le (111) et le 5 bits pour l'intensité





CRGB leds[NUM_LEDS] = {0}; // initialise le tableau de led en fonction du nombre de led

void setup() 
{
  delay(500); // power-up safety delay
    // Two strips of LEDs, one in HD mode, one in software gamma mode.
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  // indique la ref de la led, on donne son DI et CI 
  FastLED.setBrightness(90); //111+11111 = 255 // à voir 
  
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

  for (int i = 0; i < NUM_LEDS; i++) {
      
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
  delay(1000); 

}
