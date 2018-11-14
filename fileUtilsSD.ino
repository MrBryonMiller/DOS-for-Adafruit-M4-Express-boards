//fileUtilsSD.ino


void copyAtoF(char *sourcepath,char *filename,char *destpath)
{
File current;
//if ((current = SD.open(adirectory)) == NULL) 
if ((current = SD.open(sourcepath)) == NULL) 
	{
	Serial.println(F("Source Directory not found"));
	return;
	}
int count=0;
while (true) 
	{
	File entry =  current.openNextFile();
	if (!entry)
		break;
	if (!checkmatch(filename,entry.name()))
		continue;
	if (entry.isDirectory()) 
		continue;
//	entry.close();
	copyAtoF1(sourcepath,destpath,entry.name());
	count++;
	}
if (!count)
	Serial.println("File Not Found");
Serial.println();
}


void copyAtoF1(char *sourcepath,char *destpath,char *filename)
{
char Flashfullname[80];
char SDfullname[80];
//strcpy (Flashfullname,fdirectory);
strcpy (Flashfullname,destpath);
// add '/' to separate unless already at root
if (strlen(Flashfullname) > 1)
	strcat (Flashfullname,"/");
strcat (Flashfullname,filename);
// Serial.print("Checking if  ");Serial.print(Flashfullname);Serial.println(" exists");

// find if file is already there, if so skip
FlashFile MyFlashFile = fatfs.open(Flashfullname);
if (MyFlashFile)
	{
	Serial.print("f:");
	Serial.print(Flashfullname);
	Serial.println(" already exists. Not copying.");
	MyFlashFile.close();
	return;
	}
strcpy (SDfullname,sourcepath);
if (strlen(SDfullname) > 1)
	strcat (SDfullname,"/");
strcat (SDfullname,filename);
Serial.print("full source name a:");
Serial.println(SDfullname);
File MySDFile = SD.open(SDfullname);
if (MySDFile) 
	{
	// Check if destination directory exists (note root directory always exists)
	// Note you should _not_ add a trailing slash (like '/test/') to directory names!
	// You can use the same exists function to check for the existence of a file too.
	if ((strlen(destpath)>1) && !fatfs.exists(destpath)) 
		{
		Serial.println("destination directory not found.");
		return;
		}
	Serial.println("Copying file from a: to f:");
	int moved = 0;
	size_t max = MySDFile.size();  
	int target = max/30;
	uint8_t buf[64];
	MyFlashFile = fatfs.open(Flashfullname,FILE_WRITE);
	// draw status line
	Serial.println("|--|--|--|--|--|--|--|--|--|--|");
	size_t n,k=0;
	while ((n = MySDFile.read(buf, sizeof(buf))) > 0) 
		{
		MyFlashFile.write(buf, n);
		moved += n;
		if (moved > target)
			{
			target += max/30;
			Serial.print('.');
			k++;
			}
		}
	MyFlashFile.close();
	for (n=k;n<31;n++)
		Serial.print('.');
	
	Serial.println("\nFile has been copied");
	MySDFile.close();
	}
else Serial.println("error opening file on a:");
}
