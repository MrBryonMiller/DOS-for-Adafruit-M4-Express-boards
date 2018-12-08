/******
"Arduino Dos" aka ADos designed to give access to two "disk Drives"
a: is a drive on an SD Card
F: is a drive on a onboard Flash.

if the onboard Flash is on a Metro/Feather M4 the interface is over QSPI

This program has been developed on using M4 based board but is intended to
work with M0 Metro's.
*******/
#include <SPI.h>
#include <SD.h>


#include "Adafruit_SPIFlash_FatFs.h"
#define SD_CS    5

// Include the FatFs library header to use its low level functions
// directly.  Specifically the f_fdisk and f_mkfs functions are used
// to partition and create the filesystem.
#include "utility/ff.h"

#define FLASH_TYPE     SPIFLASHTYPE_W25Q16BV  // Flash chip type.
                                              // If you change this be
                                              // sure to change the fatfs
                                              // object type below to match.
#ifdef __SAMD51__
#include "Adafruit_QSPI_GD25Q.h"
Adafruit_QSPI_GD25Q flash;

#else
#define FLASH_TYPE     SPIFLASHTYPE_W25Q16BV  // Flash chip type.
                                              // If you change this be
                                              // sure to change the fatfs
                                              // object type below to match.

// #if !defined(SS1)
  // #define FLASH_SS       SS                    // Flash chip SS pin.
  // #define FLASH_SPI_PORT SPI                   // What SPI port is Flash on?
  // #define NEOPIN         8
// #else
  #define FLASH_SS       SS1                    // Flash chip SS pin.
  #define FLASH_SPI_PORT SPI1                   // What SPI port is Flash on?
  #define NEOPIN         40
// #endif

Adafruit_SPIFlash flash(FLASH_SS, &FLASH_SPI_PORT);     // Use hardware SPI

#endif
Adafruit_W25Q16BV_FatFs fatfs(flash);

String buildinfo=__FILE__;
String builddate=__DATE__ " " __TIME__;
int defaultDrive;	// 0= a: (SD Drive)
				// 1= f: (Flash Drive)

bool flashFormatted = false;
bool flashFound = false;
//FlashFile MyFlashFile;
struct AFile   // Advanced file
	{
	SDFile MySDFile;
	::File MyFlashFile;
	};

void setup() 
{
Serial.begin(115200);  //Initialize serial
while(!Serial)  // might as well wait forever since this program is useless with out Monitor
     delay(1);
  
Serial.println("Welcome to ADos"); 
Serial.println("Enter 'help' for help"); 

if (!SD.begin(SD_CS)) 
	{
	Serial.println("SD initialization failed!");
	if (checkFlashDrive())
		defaultDrive = 1;
	}
else checkFlashDrive();

prompt();

}

void loop() 
{
SerialHandler();
}


extern "C" char *sbrk(int i);

int freeram () 
{
char stack_dummy = 0;
return &stack_dummy - sbrk(0);
}
