#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
int compteur = 0;

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
}

void loop()
{
  compteur++;
  SerialBT.print("Valeur du compteur: ");
  SerialBT.println(compteur);
  delay(500);
}
