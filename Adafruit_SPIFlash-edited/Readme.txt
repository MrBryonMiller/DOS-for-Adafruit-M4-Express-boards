These files are a near-identical copy of the AdaFruit SPIFlash Libraries.
See https://github.com/adafruit/Adafruit_SPIFlash

Difference is all whole-word occurrences of "File" where replaced with 
"FlashFile".  Additionally DEBUG was turned off to prevent duplicate 
error messages.

The use of "FlashFile" was because I could see of no other way to prevent
conflict with "File" as used in the SD Library.  There is probably some c++
way to resolve this conflict but I am not well enough informed to find it.