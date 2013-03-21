#include <Adafruit_VC0706.h>
#include <SD.h>
#include <SoftwareSerial.h>         
#define chipSelect 10
Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);

void setup() {

  // When using hardware SPI, the SS pin MUST be set to an
  // output (even if not connected or used).  If left as a
  // floating input w/SPI on, this can cause lockuppage.
#if !defined(SOFTWARE_SPI)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  if(chipSelect != 53) pinMode(53, OUTPUT); // SS on Mega
#else
  if(chipSelect != 10) pinMode(10, OUTPUT); // SS on Uno, etc.
#endif
#endif

  Serial.begin(9600);
  Serial.println("VC0706 Camera snapshot test");
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }  
  
  // Try to locate the camera
  if (cam.begin()) {
    Serial.println("Camera Found:");
  } else {
    Serial.println("No camera found?");
    return;
  }
  cam.setImageSize(VC0706_640x480);        // biggest
 
  delay(5000);
  
}

void loop() {
  int filenumber;
  
   Serial.println("Taking a picture");
  delay(1000);
 
  if (SD.exists("PICTURES/filelist.txt"))
  {
   File filelist = SD.open("PICTURES/filelist.txt", FILE_WRITE);
   filenumber = filelist.position();
   filelist.write("0");
   filelist.close();
   
  } else {
    SD.mkdir("PICTURES");
    File filelist = SD.open("PICTURES/filelist.txt", FILE_WRITE); 
    filelist.write("0");
    filelist.close();
  }
   
  if (! cam.takePicture()) 
    Serial.println("Failed to snap!");
  else 
    Serial.println("Picture taken!");
  
    char filename[20];
    String iterator = String(filenumber);
    String myFilenameString = "PICTURES/" + iterator + ".JPG";
    myFilenameString.toCharArray(filename, 20);
     /* if (! SD.exists(filename)) {
      return;
    }  */
    
  Serial.println("About to write filename: ");
  Serial.println(filename);
  // Open the file for writing
 File imgFile = SD.open(filename, FILE_WRITE);

  // Get the size of the image (frame) taken  
  uint16_t jpglen = cam.frameLength();
  Serial.print("Storing ");
  Serial.print(jpglen, DEC);
  Serial.print(" byte image.");

  int32_t time = millis();
  pinMode(8, OUTPUT);
  // Read all the data up to # bytes!
  byte wCount = 0; // For counting # of writes
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(64, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
    imgFile.write(buffer, bytesToRead);
    if(++wCount >= 64) { // Every 2K, give a little feedback so it doesn't appear locked up
      Serial.print('.');
      wCount = 0;
    }
    //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
    jpglen -= bytesToRead;
  }
  imgFile.close();

  time = millis() - time;
  Serial.println("done!");
  Serial.print(time); Serial.println(" ms elapsed");
  //Resets the camera so that the next image can be captured without simply re-downloading the image that is already in the camera's memory
  cam.reset();
  
  
}






