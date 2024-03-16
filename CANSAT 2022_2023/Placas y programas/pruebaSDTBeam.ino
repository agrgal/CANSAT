#include <SPI.h>
#include <SD.h>

// HSPI
#define HSPI_SCLK 14
#define HSPI_MISO 4
#define HSPI_MOSI 13
#define HSPI_CS 2

SPIClass * hspi = NULL;

String dataString =""; // holds the data to be written to the SD card
File sensorData;

void setup() {
  delay(400);
  hspi = new SPIClass(HSPI);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_CS); //SCLK, MISO, MOSI, SS
  pinMode(HSPI_CS, OUTPUT); //HSPI SS
  
  Serial.begin(115200);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(HSPI_CS,*hspi)) {
    Serial.println("Card failed, or not present");
  }

  if (SD.exists("/data.csv")) {
    Serial.println("data.csv exists.");
  }
  else {
    Serial.println("data.csv doesn't exist.");
  }

  // open a new file and immediately close it:
  Serial.println("Creating data.csv...");
  sensorData = SD.open("/data.csv", FILE_WRITE);
  sensorData.print("Pruebaprueba");
  sensorData.print(';');
  sensorData.print("xd?");
  sensorData.print('\n');
  sensorData.print("XDXDXD");
  sensorData.close();

  // Check to see if the file exists:
  if (SD.exists("/data.csv")) {
    Serial.println("data.csv exists.");
  }
  else {
    Serial.println("data.csv doesn't exist.");
  }
  while(1);
}

void loop() {

}