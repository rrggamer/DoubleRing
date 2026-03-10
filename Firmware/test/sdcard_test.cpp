#include <SPI.h>
#include "SdFat.h" 

// Define SPI Pins
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23
#define CS_PIN    5

// CHANGED: Use standard SdFat and File objects for FAT32
SdFat sd;
SdFile dataFile;

void setup() {
  Serial.begin(115200);
  delay(1000); 

  Serial.println("\nInitializing 64GB Large FAT32 SD card...");

  // Force SPI pins
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  // Initialize the card at a stable 4MHz
  if (!sd.begin(CS_PIN, SD_SCK_MHZ(4))) {
    Serial.println("Initialization failed! The adapter/card might physically not support SPI.");
    sd.initErrorPrint(&Serial); // This prints the detailed error code
    return;
  }
  
  Serial.println("64GB FAT32 SD card initialized successfully!");

  // Open the file. O_RDWR = Read/Write, O_CREAT = Create if it doesn't exist, O_AT_END = Append
  if (dataFile.open("test_log.txt", O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println("Writing to test_log.txt...");
    dataFile.println("64GB Large FAT32 Test: SUCCESS");
    dataFile.close(); 
    Serial.println("Data written and file closed.");
  } else {
    Serial.println("Error opening test_log.txt");
  }
}

void loop() {
  // Nothing to do here
}