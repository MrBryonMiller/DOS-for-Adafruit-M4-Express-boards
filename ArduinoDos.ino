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


// I had to modify the FatFs library to remove a conflict with class "File" 
//  defined in it, as well as SD.h
#include "Adafruit_SPIFlash_FatFs.h"
#define SD_CS    5

// Include the FatFs library header to use its low level functions
// directly.  Specifically the f_fdisk and f_mkfs functions are used
// to partition and create the filesystem.
#include "utility/ff.h"

#ifdef __SAMD51__
#define FLASH_TYPE     SPIFLASHTYPE_W25Q16BV  // Flash chip type.
                                              // If you change this be
                                              // sure to change the fatfs
                                              // object type below to match.
#include "Adafruit_QSPI_GD25Q.h"
Adafruit_QSPI_GD25Q flash;

Adafruit_W25Q16BV_FatFs fatfs(flash);

#endif

String buildinfo=__FILE__;
String builddate=__DATE__ " " __TIME__;
int defaultDrive;	// 0= a: (SD Drive)
				// 1= f: (Flash Drive)

bool flashFormatted = false;
bool flashFound = false;
//FlashFile MyFlashFile;
struct AFile   // Advanced file
	{
	File MySDFile;
	FlashFile MyFlashFile;
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
