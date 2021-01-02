#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecureBearSSL.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    5

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 2

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// setup() function -- runs once at startup --------------------------------

// WiFi Parameters
const char* ssid = "FRITZ!Box 7312";
const char* password = "87147510181205287046";
const char* host = "https://nms.lecos.de/api/table.json?content=sensors&columns=objid,downtimesince,device,sensor,lastvalue,status,message,priority&filter_status=5&filter_status=4&filter_status=10&filter_status=13&filter_status=14&sortby=priority&username=radtketo&passhash=1836154354";
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
  // Check WiFi Status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected");
    WiFiClientSecure client;
    HTTPClient http;  //Object of class HTTPClient
    client.setInsecure();
    client.connect(host, 443);
    http.begin(client, host);
    int httpCode = http.GET();
    //Check the returning code                                                                  
      // Parsing
      const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
      DynamicJsonBuffer jsonBuffer(bufferSize);
      JsonObject& root = jsonBuffer.parseObject(http.getString());
      // Parameters
      int id = root["id"]; // 1
      const int treesize = root["treesize"];

      if (treesize!=0) {
        uint32_t color=strip.Color(255,   0,   0); // Red
        for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
          strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
          strip.show();                          //  Update strip to match
        }
      }
      else if (treesize==0) {
        uint32_t color=strip.Color(  0, 255,   0); // Green
        for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
          strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
          strip.show();                          //  Update strip to match
        }
      }

      // Output to serial monitor
      Serial.print("Name:");
      Serial.println(treesize);
    http.end();   //Close connection
  }
  // Delay
  delay(60000);
}
