# DOS-for-Adafruit-M4-Express-boards

I call this sketch Arduino DOS or ADos.  It was made and tested with Adafruit Feather M4 and SD card access via TFT Featherwing.

Once you install this sketch and open the serial monitor you'll have access to your SD card and the onboard Flash memory with a DOS-like set of commands.  You can select which 'drive' you are looking at by entering 'a:' to look at the SD drive or 'f:' to look at the onboard Flash.  If the Onboard Flash is not formatted as a drive you will not be able to select 'f:'.  But you can get the Flash in a usable state by entering 'FLASHERASE' and then 'FLASHFORMAT'.

Notice that you cannot create an entirely new file.  The presumption is that you will put files on the SD card via your host computer.  And then use ADos to copy the files to your flash.  Once the files are organized on your Flash drive you can use other programs to access those files and those other programs won't need an SD card plus they may be able to utilize QSPI speeds to get the data.

In order to get a list of the available commands enter 'help'.

Here is the results 
<pre>
-------------Help-------------
built from : 
C:\Users\Bryon\Documents\Data\PROGRAMS\Arduino\ArduinoDos\ArduinoDos.ino
At :Nov 14 2018 12:26:41
Available RAM 187179

Following Serial commands are available:
 * indicates wildcards ? & * are usable

a:     selects the SD card as the current drive.
f:     selects the Onboard flash as the current drive.
DIR*   Displays a list of files and subdirectories.
CD    <dir> Changes the current directory
MKDIR <dir> Makes a new directory
RMDIR <dir> removes directory and all in and below
TYPE  <filename> Prints a file as text
DEL*  <filename> Deletes files (not <dirs>)
COPY* <filename> copies files, only from one drive to other

FLASHINFO   Prints info about the Flash
<Think before using these last two commands>
FLASHERASE  Erase flash so it is completely blank
FLASHFORMAT Formats drive f: leaving an empty drive

a:/l1>
</pre>

To install you need the source files from this repository and you'll need several libraries. 
See : Adafruit_QSPI-master https://github.com/adafruit/Adafruit_QSPI
And : SPIFlash Libraries.AdaFruit https://github.com/adafruit/Adafruit_SPIFlash

As of 3/7/19 :
Note that a change was made to the SPIFlash library (1.1.0) making it no longer compatible with the QSPI library.  Until that problem is corrected you will need to use version 1.0.8 of the SPI Flash library.
