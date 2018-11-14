// "Advanced Utilities"  Advanced refers to the notion that these routines
// deal with either "SD Files' or 'Flash FlashFiles' depending on the drive.
// Routines with names AF... are utility routines
// Routines with name A... are called from serial input processing
char AFfileRead(int drive,AFile &tFile)
{
if (drive)
	{
	return tFile.MyFlashFile.read();
	}
else {
//	tFile.MySDFile = SDfile;
	return tFile.MySDFile.read();
	}
}

bool AFfileAvailable(int drive,AFile &tFile)
{
if (drive)
	return tFile.MyFlashFile.available();
else return tFile.MySDFile.available();
}

bool AFisDirectory(int drive,AFile &tFile)
{
if (drive)
	return tFile.MyFlashFile.isDirectory();
else return tFile.MySDFile.isDirectory();
}

void AFfileClose(int drive,AFile &tFile)
{
if (drive)
	tFile.MyFlashFile.close();
else tFile.MySDFile.close();
}

bool AFremove(int drive,char *filename)
{
if (drive)
	return fatfs.remove(filename);
else return SD.remove(filename);
}

bool AFopenNextFile(int drive,AFile &iFile,AFile &oFile)
{
if (drive)
	{
	oFile.MyFlashFile = iFile.MyFlashFile.openNextFile();
	return oFile.MyFlashFile != 0;
	}
else {
	oFile.MySDFile = iFile.MySDFile.openNextFile();
	return oFile.MySDFile != 0;
	}
}

char * AFname(int drive,AFile &tFile)
{
if (drive)
	return tFile.MyFlashFile.name();
else return tFile.MySDFile.name();
}

int AFsize(int drive,AFile &tFile)
{
if (drive)
	return tFile.MyFlashFile.size();
else return tFile.MySDFile.size();
}

// following procedure cannot be used since it opens a Flash File
// once the scope changes from this opening the class is destructed

bool AFfileOpen(int drive,char *filepath,AFile &tFile,FlashFile temp)
{
if (drive)
	{
	tFile.MyFlashFile = temp;
	return tFile.MyFlashFile != 0;
	}
else {
	tFile.MySDFile = SD.open(filepath);
	return tFile.MySDFile != 0;
	}
}


void ADel(char *match)
{
AFile myFile;
bool bt;
// open fatfs directory regardless even though it may not be needed
//  must be open at scope of this procedure otherwise destructor 
//  will be called
#ifdef DEBUG
if (!defaultDrive && !flashFormatted)
	Serial.println(F("ignore following message (you have already been told)"));
#endif
FlashFile temp = fatfs.open(currentdirectory[defaultDrive]);
if (defaultDrive)
	{
	myFile.MyFlashFile = temp;
	bt = temp != 0;
	}
else {
	myFile.MySDFile = SD.open(currentdirectory[defaultDrive]);
	bt = myFile.MySDFile !=0;
	}
if (!bt)
	{
	Serial.println(F("Directory not found."));
	return;
	}
if (!AFisDirectory(defaultDrive,myFile))
	{
	Serial.println(F("Directory not found"));
	return;
	}

bool isDir;
bool dirReported = false;
char namebuffer[64];
char *ndp=namebuffer;
int count=0;
strcpy (ndp,currentdirectory[defaultDrive]);
if (strcmp(ndp,"/"))	// if not root we need trailing slash
	strcat (ndp,"/");
int lastcp = strlen(ndp);
while (true) 
	{
	AFile Aentry;
	AFopenNextFile(defaultDrive,myFile,Aentry);
	if (defaultDrive)
		{
		if (!Aentry.MyFlashFile)
			break;
		}
	else {
		if (!Aentry.MySDFile)
			break;
		}

	if (!checkmatch(match,AFname(defaultDrive,Aentry)))
		continue;
	isDir = AFisDirectory(defaultDrive,Aentry);
	if (isDir && !dirReported) 
		{
		dirReported = true;
		Serial.println("del will not remove Directories, use rmdir");
		} 
	AFfileClose(defaultDrive,Aentry);
	if (!isDir)
		{
		// take ndp down to just directory
		ndp[lastcp]=0;
		strcat(ndp,AFname(defaultDrive,Aentry));
		Serial.print("deleting ");
		Serial.println(ndp);
		if (AFremove(defaultDrive,ndp))
			count++;
		else Serial.println("Did not delete");
		}
	}
if (count)
	{
	Serial.print(count);
	Serial.println(" file(s) deleted");
	}
else Serial.println("File Not Found");
}

void ADir(char *sourcearg)
{
AFile parentDir;
int fileCount=0;
int dirCount=0;
unsigned int totSize=0;
bool bt;
int drive = getDrive(sourcearg);

char pathbuffer[64] = {0};
char *path = pathbuffer;

getDir(sourcearg,path,currentdirectory[drive]);
//Serial.print("filename ");Serial.println(sourcearg);Serial.println(strlen(sourcearg));
//Serial.print("path ");Serial.println(path);


// open fatfs directory regardless even though it may not be needed
//  must be open at scope of this procedure otherwise destructor 
//  will be called
FlashFile temp = fatfs.open(path);
if (drive)
	{
	parentDir.MyFlashFile = temp;
	bt = parentDir.MyFlashFile != 0;
	}
else {
	parentDir.MySDFile = SD.open(path);
	bt = parentDir.MySDFile !=0;
	}
if (!bt)
	{
	Serial.print(F("Directory not found "));
	Serial.println(path);
	return;
	}
if (!AFisDirectory(drive,parentDir))
	{
	Serial.print(F("Dir not found "));
	Serial.println(path);
	return;
	}
	
Serial.print("Directory of ");
if (drive)
	Serial.print("f:");
else Serial.print("a:");

Serial.println(path);

bool isDir;
bool dirReported = false;
AFile childFile;
while (true) 
	{
	if (!AFopenNextFile(drive,parentDir,childFile))
		break;
	if (!checkmatch(sourcearg,AFname(drive,childFile)))
		continue;
	Serial.print(AFname(drive,childFile));
	for (int l = strlen(AFname(drive,childFile));l<15;l++)
		Serial.print(' ');
	isDir = AFisDirectory(drive,childFile);
	if (isDir) 
		{
		Serial.println("     <dir>");
		dirCount++;
		}
	else {// files have sizes, directories do not
		unsigned int size = AFsize(drive,childFile);
		printIntWithComma(size);
		Serial.println();
		totSize += size;
		fileCount++;
		}
	AFfileClose(drive,childFile);
	}
if (dirCount+fileCount)
	{
	Serial.print("        ");
	Serial.print(fileCount);
	Serial.print(" File(s)   ");
	printIntWithComma(totSize);
	Serial.println(" bytes");
	Serial.print("        ");
	Serial.print(dirCount);
	Serial.println(" Dir(s)");
	}
else Serial.println("        Empty");
AFfileClose(drive,parentDir);
}


void AType(char *sourcearg)
{
int drive = getDrive(sourcearg);

char pathbuffer[64] = {0};
char *path = pathbuffer;

getDir(sourcearg,path,currentdirectory[drive]);

if (strcmp(path,"/"))	// if not root we need trailing slash
	strcat (path,"/");
strcat(path,sourcearg);
Serial.print("TYPEing ");
if (drive)
	Serial.print("f:");
else Serial.print("a:");
Serial.println(path);

/* /* Serial.println(ndp);*/
AFile myFile;
bool bt;
#ifdef DEBUG
if (!defaultDrive && !flashFormatted)
	Serial.println(F("ignore following message (you have already been told)"));
#endif
// open fatfs directory regardless even though it may not be needed
//  must be open at scope of this procedure otherwise destructor 
//  will be called
FlashFile temp = fatfs.open(path);
if (drive)
	{
	myFile.MyFlashFile = temp;
	bt = temp != 0;
	}
else {
	myFile.MySDFile = SD.open(path);
	bt = myFile.MySDFile !=0;
	}
if (bt)
	{
	while (AFfileAvailable(drive,myFile)) 
		{
//		char c = myFile.MySDFile.read();
		char c = AFfileRead(drive,myFile);
		Serial.print(c);
		}
	Serial.println();
//	myFile.MySDFile.close();
	AFfileClose(drive,myFile);
	}
else Serial.println("cannot open file");
}

void ARmdir(char *dirname)
{
bool isDir;
char namebuffer[64];
char *ndp=namebuffer;
strcpy (ndp,currentdirectory[defaultDrive]);
if (strcmp(ndp,"/"))	// if not root we need trailing slash
	strcat (ndp,"/");
strcat(ndp,dirname);
if (strlen(ndp))
	{
	AFile myFile;
	Serial.print("removing ");
	Serial.println(ndp);
	bool bt;
	// open fatfs directory regardless even though it may not be needed
	//  must be open at scope of this procedure otherwise destructor 
	//  will be called
	#ifdef DEBUG
	if (!defaultDrive && !flashFormatted)
		Serial.println(F("ignore following message (you have already been told)"));
	#endif
	FlashFile temp = fatfs.open(ndp);
	if (defaultDrive)
		{
		myFile.MyFlashFile = temp;
		bt = temp != 0;
		}
	else {
		myFile.MySDFile = SD.open(ndp);
		bt = myFile.MySDFile !=0;
		}
	if (!bt)
		Serial.println("Could not find directory");
	else {
		isDir = AFisDirectory(defaultDrive,myFile);
		if (isDir) 
			if (defaultDrive)
				fatfs.rmdir(ndp);
			else SD.rmdir(ndp);
		else Serial.println("not a Directory");
		}
	}
else Serial.println("cannot rmdir '/'");
}

void AMkdir(char *dirname)
{
char namebuffer[64];
char *ndp=namebuffer;
strcpy (ndp,currentdirectory[defaultDrive]);
if (strcmp(ndp,"/"))	// if not root we need trailing slash
	strcat (ndp,"/");
strcat(ndp,dirname);
Serial.println(ndp);
bool made = defaultDrive?fatfs.mkdir(ndp):SD.mkdir(ndp);
if (made)
	Serial.println("directory made");
else Serial.println("Error, failed to create directory!");
}

void Acopy(char *sourcearg,char *destarg)
{
if ((sourcearg==NULL) || (destarg==NULL))
	{
	Serial.println("copy requires two arguments!");
	return;
	}
// Serial.println(sourcearg);
// Serial.println(destarg);
//get source and destination
int sourcedrive = getDrive(sourcearg);
int destdrive = getDrive(destarg);
if (sourcearg==NULL)
	{
	Serial.println("copy requires source file name");
	return;
	}

// Serial.println(sourcearg);
// Serial.println(destarg);

if (sourcedrive==destdrive)
	{
	Serial.println ("Source drive cannot be the same as Destination Drive");
	return;
	}
char sourcepathbuffer[64] = {0};
char *sourcepath = sourcepathbuffer;

getDir(sourcearg,sourcepath,currentdirectory[sourcedrive]);

char destpathbuffer[64] = {0};
char *destpath = destpathbuffer;

getDir(destarg,destpath,currentdirectory[destdrive]);


//Serial.println("Source     >");Serial.print(sourcedrive);Serial.print(' ');Serial.print(sourcepath);Serial.print(' ');Serial.println(sourcearg);
//Serial.println("Destination>");Serial.print(destdrive);Serial.print(' ');Serial.print(destpath);Serial.print(' ');Serial.println(destarg);
if (strcmp(sourcearg,destarg) && strcmp(destarg,"*"))
	{
	Serial.println("Destination filename must be '*' or same as source filename");
	return;
	}
if (sourcedrive)
	copyFtoA(sourcepath,sourcearg,destpath);
else copyAtoF(sourcepath,sourcearg,destpath);
}

int getDrive(char * &arg)
{
int drive = defaultDrive;
if ((arg!=NULL) && strlen(arg) >= 2)
	{
	if ((arg[0] == 'a')  && (arg[1] == ':'))
		{
		drive = 0;
		arg = arg+2;
		}
	if ((arg[0] == 'f')  && (arg[1] == ':'))
		{
		drive = 1;
		arg = arg+2;
		}
	}
if (strlen(arg)==0)
	arg=NULL;
return drive;
}

void getDir(char * &arg,char * &buff,char * current)
{
bool atEnd;
//Serial.print("arg ");Serial.println(arg);
char * pch;
pch=strrchr(arg,'/');
bool separator = (pch != NULL);
if (separator)
	{
	atEnd = (pch == (arg+strlen(arg)-1));
	if (atEnd)
		{
		strcpy(buff,arg);
		arg = NULL;
		}
	else {
		strncpy(buff,arg,pch+1-arg);
		arg=pch+1;
		}
	}
if (!strcmp(arg,".."))
	{
	arg = NULL;
	strcat(buff,"../");
	}
//Serial.print("arg ");Serial.println(arg);
//Serial.print("buff ");Serial.println(buff);
	
// now back up a directory in current for each "../" in buff
char namebuffer[64];
char *ndp=namebuffer;
strcpy(ndp,current);
//Serial.print("default directory ");Serial.println(ndp);
while ((buff[0]== '.') && (buff[1]== '.') && (buff[2]== '/'))
	{
	buff=buff+3;
	char *end;
	end=strrchr(ndp,'/');
	// in case we were only one level down make sure root has '/'
	if (end==namebuffer)
		end[1] = 0;
	else end[0] = 0;
	}
//Serial.print("default after backing up ");Serial.println(ndp);
// now ndp contains starting directory
// add in new directory 
if ((buff==NULL) || (buff[0]!='/'))
	{
	if (buff != NULL)
		{
		strcat(ndp,"/");
		strcat(ndp,buff);
		}
	if (ndp[strlen(ndp)-1] != '/')
		strcat (ndp,"/");
	}
else strcpy (ndp,buff);
//Serial.print("default after add in ");Serial.println(ndp);
if ((strlen(ndp) > 1) && (ndp[strlen(ndp)-1] == '/'))
	ndp[strlen(ndp)-1] = 0;
// there are ways that we can have "//" at beginning - fix it
if ((ndp[0]=='/') && (ndp[1]=='/'))
	ndp++;
strcpy(buff,ndp);
if (strlen(buff)==0)
	buff=NULL;
}
