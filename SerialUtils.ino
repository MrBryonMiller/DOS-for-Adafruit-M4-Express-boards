// SerialUtils.ino
// define buffer for Serial.readline
#define INPBUFSIZE  150
char buffer[INPBUFSIZE+1];

char currentdirectory[2][64] = {"/","/"};

void help()
{
Serial.println("-------------Help-------------");
Serial.println("built from : ");
Serial.println(buildinfo);
Serial.print("At :");
Serial.println(builddate);
//Serial.print("Available RAM ");
//Serial.println (freeram());
//Serial.println();
if (checkFlashDrive())
	{
	Serial.print("Flash total size ");
	printIntWithComma(flash.pageSize()*flash.numPages());
	Serial.println(" bytes");
	}
Serial.println();
Serial.println("Following Serial commands are available:");
Serial.println(" * indicates wildcards ? & * are usable");
Serial.println();
Serial.println("a:     selects the SD card as the current drive.");
Serial.println("f:     selects the Onboard flash as the current drive.");
Serial.println("DIR*   Displays a list of files and subdirectories.");
Serial.println("CD    <dir> Changes the current directory");
Serial.println("MKDIR <dir> Makes a new directory");
Serial.println("RMDIR <dir> removes directory and all in and below");
Serial.println("TYPE  <filename> Prints a file as text");
Serial.println("DEL*  <filename> Deletes files (not <dirs>)");
Serial.println("COPY* <filename> copies files, only from one drive to other");
Serial.println();
Serial.println("FLASHINFO   Prints info about the Flash");
Serial.println("<Think before using these last two commands>");
Serial.println("FLASHERASE  Erase flash so it is completely blank");
Serial.println("FLASHFORMAT Formats drive f: leaving an empty drive");
}

void SerialHandler()
{
if (Serial.available() && readline())
     {
    // Serial.print("\nreceived ");
	Serial.println(buffer);
	bool handled=false;
     char *p;
     p = strtok(buffer," ");
	if (!strcmp(p, "help"))
		{
		help();
		handled = true;
		}
	if (!strcmp(p, "flashinfo"))
		{
		Serial.print("Flash chip JEDEC ID: 0x"); 
		Serial.println(flash.GetJEDECID(), HEX);
#ifdef __SAMD51__
          Serial.print("Manufacturer ID ");
          Serial.println(flash.readManufacturerID());
          Serial.print("Device ID ");
          Serial.println(flash.readDeviceID());
#endif
		handled = true;
          }
	if (!strcmp(p, "flashformat"))
		{
		Serial.println("This command will format the Flash!");
		Serial.println("Any data on it will be lost!");
		Serial.println("Enter yes to proceed, anything else to skip.");
		while (!Serial.available());
		readline();
		p = strtok(buffer," ");
		bool format = !strcmp(p, "yes");
		if (format)
			formatflash();
		else Serial.println("Skipping Format");
		handled = true;
		}
	if (!strcmp(p, "flasherase"))
		{
		Serial.println("This command will erase the Flash!");
		Serial.println("Any data on it will be lost!");
		Serial.println("Enter yes to proceed, anything else to skip.");
		while (!Serial.available());
		readline();
		p = strtok(buffer," ");
		bool erase = !strcmp(p, "yes");
		if (erase)
			eraseflash();
		else Serial.println("Skipping Erase");
		handled = true;
		}
	if (!strcmp(p, "f:"))	// change drive to f:
		{
		if (checkFlashDrive())
			defaultDrive = 1;
		handled = true;
          }
     if (!strcmp(p, "a:"))	// change drive to a:
          {
		defaultDrive = 0;
		handled = true;
          }
     if (!strcmp(p, "dir"))
          {
		p = strtok (NULL, " ");
		ADir(p);
		handled = true;
          }
     if (!strcmp(p, "del"))
          {
		p = strtok (NULL, " ");
		ADel(p);
		handled = true;
          }
     if (!strcmp(p, "rmdir"))
          {
		p = strtok (NULL, " ");
		ARmdir(p);
		handled = true;
          }
     if (!strcmp(p, "cd"))
          {
		cdHandler();
		handled = true;
          }
     if (!strcmp(p, "type"))
          {
		p = strtok (NULL, " ");
		AType(p);
		handled = true;
          }
     if (!strcmp(p, "copy"))
          {
		p = strtok (NULL, " ");
		char *p1;
		p1 = strtok (NULL, " ");
		Acopy(p,p1);
		handled = true;
          }
     if (!strcmp(p, "mkdir"))
          {
		p = strtok (NULL, " ");
		AMkdir(p);
		handled = true;
          }
	if (handled)
		Serial.println();
	else	{
		Serial.print("'");
		Serial.print(p);
		Serial.println("' is not recognized");
		}
	prompt();
	}

}

void prompt(void)
{
if (defaultDrive==0)
	Serial.print("a:");
if (defaultDrive==1)
	Serial.print("f:");

Serial.print(currentdirectory[defaultDrive]);
Serial.print(">");
}


void cdHandler()
{
char *sourcearg = strtok (NULL, " ");
if (sourcearg == NULL)
	{
	Serial.print("current directory '");
	Serial.print(currentdirectory[defaultDrive]);
	Serial.println("'");
	return;
	}

char pathbuffer[64] = {0};
char *path = pathbuffer;

getDir(sourcearg,path,currentdirectory[defaultDrive]);
if (sourcearg != NULL)
	{
	if (strlen(path)>1)
		strcat(path,"/");
	strcat(path,sourcearg);
	}
// Serial.println(strlen(sourcearg));
// Serial.println(sourcearg);
// Serial.println(strlen(path));
// Serial.println(path);

// check if dir exists and if so, make it actual
if (defaultDrive == 0)
	{
	if ((strcmp(path,"/") != 0) && !SD.exists(path))
		{
		Serial.print(path);
		Serial.println(" directory not found");
		}
	else strcpy (currentdirectory[0],path);
	}
if (defaultDrive == 1)
	{
	if ((strcmp(path,"/") != 0) && !fatfs.exists(path))
		{
		Serial.print(path);
		Serial.println(" directory not found");
		}
	else strcpy (currentdirectory[1],path);
	}
}


uint16_t readline(void)
{
return readline(buffer, INPBUFSIZE, 50);
}
uint16_t readline(char * buf, uint16_t bufsize, uint16_t timeout)
{
uint16_t replyidx = 0;
while (timeout--) 
     {
     while(Serial.available()) 
          {
          char c = Serial.read();
          //SerialDebug.println(c);
          if (c == '\r') 
               continue;
          if (c == '\n') 
               {
               // the first '\n' is ignored
//               if (replyidx == 0) 
//                    continue;
               timeout = 0;
               break;
               }
          buf[replyidx] = tolower(c);
          replyidx++;
          
          // Buffer is full
          if (replyidx >= bufsize) 
               {
               Serial.println("*overflow*");   // for my debuggin' only!
               timeout = 0;
               break;
               }
          }

     // delay if needed
     if (timeout) 
          delay(1);
     }

buf[replyidx] = 0;  // null term
return replyidx;
}

int printIntWithComma(int i)
{
bool leading = false;
int max = 100000000;
int digits = 0;
while (max)
	{
	int t = i/max;
	if (leading || (t>0) || (max==1))
		{
		Serial.print(t);
		leading = true;
		if ((max==1000000)||(max==1000))
			Serial.print(',');
		digits++;
		}
	else {
		Serial.print(' ');
		if ((max==1000000)||(max==1000))
			Serial.print(' ');
		}
	i = i-t*max;
	max /= 10;
	}
return digits;
}
