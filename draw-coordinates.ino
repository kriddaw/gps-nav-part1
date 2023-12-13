#include <ezButton.h>
#include <ezBuzzer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <TinyGPSPlus.h>

TinyGPSPlus gps;
#define ss Serial1 // first built with software serial on UNO

#define i2c_Address 0x3c // initialize with the I2C addr 0x3C Typically eBay OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ezButton btn(5); // connect button to pin 5
ezBuzzer buzzer(9); // connect buzzer to pin 9

const int numlatLongs = 11;
float latLongs[numlatLongs][2] = {
  {36.240344, -115.319698}, 
  {36.241099, -115.318955}, 
  {36.241239, -115.318950}, 
  {36.241473, -115.318679}, 
  {36.241694, -115.318673}, 
  {36.242070, -115.318453}, 
  {36.242444, -115.318649}, 
  {36.242260, -115.319588}, 
  {36.241583, -115.319489}, 
  {36.240702, -115.319730}, 
  {36.240350, -115.319743}
};
float maxLat = -90.0;
float minLat = 90.0;
float maxLong = 180.0;
float minLong = -180.0;

float aspectX = 1.0;
float aspectY = 1.0;

float currentLat = latLongs[0][0];
float currentLong = latLongs[0][1];
int course = 0;
int course2 = 0;
int distanceBetween = 0;

int wayPoint = 0;

void setup() {

  Serial.begin(115200); // for debugging
  ss.begin(9600);
  btn.setDebounceTime(100);
  minmaxLatLong(); // set minimum and maximum dimensions on map

  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
 
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Initializing");
  display.display();
  delay(1000);
}

void loop() {

  btn.loop();
  buzzer.loop();
  if (btn.isPressed()) {
    wayPoint += 1;
    if (wayPoint > numlatLongs-1) wayPoint = 0;
    buzzer.beep(50);
  }

  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      getPositionInfo();
    }
  }

  display.clearDisplay();

  for (int i = 0; i < numlatLongs - 1; i++) {
    display.drawLine(
      map(longify(latLongs[i][1]), longify(minLong), longify(maxLong), (63*aspectX), 0), 
      map(longify(latLongs[i][0]), longify(minLat), longify(maxLat), 63*aspectY, 0), 
      map(longify(latLongs[i+1][1]), longify(minLong), longify(maxLong), 63*aspectX, 0), 
      map(longify(latLongs[i+1][0]), longify(minLat), longify(maxLat), 63*aspectY, 0), 
      SH110X_WHITE
    );
  }

  display.fillCircle(
    map(longify(currentLong), longify(minLong), longify(maxLong), (63*aspectX), 0), 
    map(longify(currentLat), longify(minLat), longify(maxLat), 63*aspectY, 0), 
    2, 
    SH110X_WHITE
  );

  display.drawCircle(
    map(longify(latLongs[wayPoint][1]), longify(minLong), longify(maxLong), (63*aspectX), 0), 
    map(longify(latLongs[wayPoint][0]), longify(minLat), longify(maxLat), 63*aspectY, 0), 
    3, 
    SH110X_WHITE
  );
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(65,0);
  display.print("Crs :");
  display.print(course);
  display.setCursor(65,15);
  display.print("Crs2:");
  display.print(course2);
  display.setCursor(65,30);
  display.print("Dist:");
  display.print(distanceBetween);
  display.setCursor(65,43);
  display.print("  ");
  display.print(currentLat,5);
  display.setCursor(65,52);
  display.print(currentLong,5);

  display.display();
}

void getPositionInfo(){
  if (gps.location.isValid()) {
		currentLat = gps.location.lat();
		currentLong = gps.location.lng();
    distanceBetween = gps.distanceBetween(
      currentLat, 
      currentLong, 
      latLongs[wayPoint][0], 
      latLongs[wayPoint][1]
    );
    course2 = gps.courseTo(
      currentLat, 
      currentLong, 
      latLongs[wayPoint][0], 
      latLongs[wayPoint][1]
    );
	}
  if (gps.course.isValid()) {
    course = int(gps.course.deg());
	}
}

long longify(float x) {
  return x * 1000000;
}

void minmaxLatLong() {
  /* I had to play around with the signs and the 
  < and > operators to get my map to properly display.
  You may have to do the same if you try this and your map
  is backwards or upside down. */
  for (int i = 0; i < numlatLongs; i++) {
    if (latLongs[i][0] < minLat) minLat = latLongs[i][0];
    if (latLongs[i][0] > maxLat) maxLat = latLongs[i][0];
    if (latLongs[i][1] > minLong) minLong = latLongs[i][1];
    if (latLongs[i][1] < maxLong) maxLong = latLongs[i][1];
  }
  float height = abs(maxLat-minLat);
  float width = abs(maxLong-minLong);
  // constrain and manage the map in the proper direction
  if (height > width) {
    aspectX = width / height;
  } else {
    aspectY = height / width;
  }
}