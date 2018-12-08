//fileUtilsFlash.ino

bool checkFlashDrive(void)
{
#ifdef __SAMD51__
if (flash.begin()) 
#else
if (flash.begin(FLASH_TYPE))
#endif
	{
	flashFound = true;
	fatfs.activate();
	if (fatfs.begin()) 
		{
		flashFormatted = true;
		return true;
		}
	else Serial.println("Flash found, not formatted.");
	}
else Serial.println("Flash not Found");
return false;
}



void copyFtoA(char *sourcepath,char *filename,char *destpath)
{
bool perform = false;
::File Flashroot = fatfs.open(sourcepath);
if (Flashroot) 
	{
	if (Flashroot.isDirectory())
		perform = true;
	}
else	Serial.println(F("Source Directory not found"));
	
int count=0;
while (perform) 
	{
	::File entry =  Flashroot.openNextFile();
	if (!entry)
		break;
	if (!checkmatch(filename,entry.name()))
		continue;
	if (entry.isDirectory()) 
		continue;
	Serial.println(entry.name());
	copyFtoA1(sourcepath,destpath,entry.name());
	count++;
	}
if (!count)
	Serial.println("File Not Found");
Serial.println();
}


void copyFtoA1(char *sourcepath,char *destpath,char *filename)
{
char Flashfullname[80];
char SDfullname[80];
//strcpy (SDfullname,adirectory);
strcpy (SDfullname,destpath);
if (strlen(SDfullname) > 1)
	strcat (SDfullname,"/");
strcat (SDfullname,filename);

// Serial.print("Checking if  ");Serial.print(SDfullname);Serial.println(" exists");

// find if file is already there, if so skip
SDFile MySDFile = SD.open(SDfullname);
if (MySDFile)
	{
	Serial.print("a:");
	Serial.print(SDfullname);
	Serial.println(" already exists. Not copying.");
	MySDFile.close();
	return;
	}
strcpy (Flashfullname,sourcepath);
// add '/' to separate unless already at root
if (strlen(Flashfullname) > 1)
	strcat (Flashfullname,"/");
strcat (Flashfullname,filename);
Serial.print("full source name f:");
Serial.println(Flashfullname);
::File MyFlashFile = fatfs.open(Flashfullname);
if (MyFlashFile) 
	{
	// Check if destination directory exists (note root directory always exists)
	// Note you should _not_ add a trailing slash (like '/test/') to directory names!
	// You can use the same exists function to check for the existence of a file too.
	if ((strlen(destpath)>1) && !SD.exists(destpath)) 
		{
		Serial.println("destination directory not found.");
		return;
		}
	Serial.println("Copying file from f: card to a:");
	int moved = 0;
	size_t max = MyFlashFile.size();  
	int target = max/30;
	uint8_t buf[64];
	MySDFile = SD.open(SDfullname,FILE_WRITE);
	// draw status line
	Serial.println("|--|--|--|--|--|--|--|--|--|--|");
	size_t n,k=0;
	while ((n = MyFlashFile.read(buf, sizeof(buf))) > 0) 
		{
		MySDFile.write(buf, n);
		moved += n;
		if (moved > target)
			{
			target += max/30;
			Serial.print('.');
			k++;
			}
		}
	MySDFile.close();
	for (n=k;n<31;n++)
		Serial.print('.');
	
	Serial.println("\nFile has been copied from f: to a:");
	MyFlashFile.close();
	}
else Serial.println("error opening file on f:");
}

// A C program to match wild card characters 
// From https://www.geeksforgeeks.org/wildcard-character-matching/
// The first string may contain wildcard characters 
bool checkmatch(char *first, char * second) 
{ 
if (first == NULL)
	return true;
// If we reach at the end of both strings, we are done 
if (*first == '\0' && *second == '\0') 
	return true; 

// Make sure that the characters after '*' are present 
// in second string. This function assumes that the first 
// string will not contain two consecutive '*' 
if (*first == '*' && *(first+1) != '\0' && *second == '\0') 
	return false; 

// If the first string contains '?', or current characters 
// of both strings match 
if (*first == '?' || *first == tolower(*second)) 
	return checkmatch(first+1, second+1); 

// If there is *, then there are two possibilities 
// a) We consider current character of second string 
// b) We ignore current character of second string. 
if (*first == '*') 
	return checkmatch(first+1, second) || checkmatch(first, second+1); 
return false; 
} 

/* I needed fourmatflash because formatflash does not show up
	in the notepad++ function list.
void fourmatflash()
{}
*/
bool formatflash()
{
fatfs.activate();

// Partition the flash with 1 partition that takes the entire space.
Serial.println("Partitioning flash with 1 primary partition...");
DWORD plist[] = {100, 0, 0, 0};  // 1 primary partition with 100% of space.
uint8_t buf[512] = {0};          // Working buffer for f_fdisk function.
FRESULT r = f_fdisk(0, plist, buf);
if (r != FR_OK) 
	{
	Serial.print("Error, f_fdisk failed with error code: "); 
	Serial.println(r, DEC);
	return false;
	}
Serial.println("Partitioned flash!");

// Make filesystem.
Serial.println("Creating and formatting FAT filesystem (this takes ~60 seconds)...");
r = f_mkfs("", FM_ANY, 0, buf, sizeof(buf));
if (r != FR_OK) 
	{
	Serial.print("Error, f_mkfs failed with error code: "); Serial.println(r, DEC);
	return false;
	}
Serial.println("Formatted flash!");

// Finally test that the filesystem can be mounted.
if (!fatfs.begin()) 
	{
	Serial.println("Error, failed to mount newly formatted filesystem!");
	return false;
	}

Serial.println("Flash chip successfully formatted with new empty filesystem!");
return true;
}

bool eraseflash()
{
Serial.println("Erasing flash chip");
Serial.println("Note you will see stat and other debug output printed repeatedly.");
Serial.println("Let it run for ~30 seconds until the flash erase is finished.");
Serial.println("An error or success message will be printed when complete.");

#ifdef __SAMD51__
flash.chipErase();
#else
flash.EraseChip();
#endif



Serial.println("Successfully erased chip!");
return true;
}
