#include <SPI.h>
#include <WiFiNINA.h>
#include <ezTime.h>
#include <Adafruit_NeoPixel.h>

#include "arduino_secrets.h" 


// Constants
const bool LOG_DEBUG   = true;
const byte LOG_INFO    = 0;
const byte LOG_WARNING = 1;
const byte LOG_ERROR   = 2;
const byte BUTTON_PIN  = 2;
const byte PIR_PIN     = 3;
const byte PIXEL_PIN   = 6;
const byte LED_PIN     = 13;  // Arduino built-in LED
const int  PIXEL_COUNT = 60;  // Number of NeoPixels

// Wifi variables
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int wifiStatus = WL_IDLE_STATUS;

// Timing variables
Timezone myTimezone;

// Pixel variables
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  initLog();
  pinMode(LED_PIN, OUTPUT);  // init artuino LED
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // init button
  pinMode(PIR_PIN, INPUT);  // init PIR motion sensor
  initPixels();
  checkWifiModule();
  checkWifiFirmware();
  //connectWifi();
  //printWifiStatus();
  //ezTimeSetup();
}

void loop() {
  // ezTime event trigger
  events();

  strip.setPixelColor(0, strip.Color(  0, 0,   255));
  strip.setPixelColor(1, strip.Color(  255, 255,   0));
  strip.setPixelColor(2, strip.Color(  255, 0,   255));
  strip.setPixelColor(4, strip.Color(  0, 0,   255));
  strip.setPixelColor(6, strip.Color(  0, 0,   255));
  strip.setPixelColor(7, strip.Color(  0, 0,   255));
  strip.show();
  rainbow(10);
}

void initLog() {
  if(LOG_DEBUG) {
    Serial.begin(9600);
    /*while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }*/
  }
}
void initPixels() {
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)

  // strip.setPixelColor(0, strip.Color(  127, 0,   0));  //  Set pixel's color (in RAM)
  strip.setBrightness(10); // Set BRIGHTNESS (max = 255)

  strip.show();  // Initialize all pixels to [0]-red
}

void checkWifiModule() {
  if (WiFi.status() == WL_NO_MODULE) {
    log("Communication with WiFi module failed!", LOG_ERROR);
    // don't continue
    while (true);
  }
}

void checkWifiFirmware() {
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    log("Please upgrade the firmware", LOG_WARNING);
  }
}

void connectWifi() {
  while (wifiStatus != WL_CONNECTED) {
    log("Attempting to connect to SSID: ", LOG_WARNING);
    
    wifiStatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  log("Connected to wifi", LOG_INFO);
  printWifiStatus();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  log("SSID: ", LOG_INFO);
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  log("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  log("signal strength (RSSI) in dBm:");
  Serial.println(rssi);
}

void ezTimeSetup() {
  // enable ezTime debug info
  setDebug(INFO);
  
  // sync with NTP server
  waitForSync();

  myTimezone.setLocation(F("de"));
  
  log("ezTime sync finished.");
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void log(const char *logmessage) {
  log(logmessage, LOG_INFO);
}

void log(const char *logmessage, byte severity) {
  if(LOG_DEBUG) {
    Serial.print(myTimezone.dateTime(ISO8601));

    switch(severity) {
      case LOG_INFO:
        Serial.print(" - INFO  - ");
        strip.setPixelColor(59, strip.Color(  0, 127, 0));
        strip.show();
        break;
      case LOG_WARNING:
        Serial.print(" - WARN  - ");
        strip.setPixelColor(59, strip.Color(  127, 127, 0));
        strip.show();
        break;
      case LOG_ERROR:
        Serial.print(" - ERROR - ");
        strip.setPixelColor(59, strip.Color(  127, 0,   0));
        strip.show();
        break;
      default:
        Serial.print(" - ");
        break;
    }

    Serial.print(logmessage);
    Serial.print("\n");
    
  }
}
