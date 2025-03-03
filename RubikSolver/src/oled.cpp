

// for a 128x64 size display using I2C to communicate

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c // initialize with the I2C addr 0x3C Typically eBay OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16

void displayCenteredText(const char *text, int textSize, int yOffset, int delayTime)
{
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(SH110X_WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int x = (SCREEN_WIDTH - w) / 2;
  int y = (SCREEN_HEIGHT - h) / 2 + yOffset;

  // Smooth fade-in effect
  for (int brightness = 0; brightness <= 255; brightness += 20)
  {
    display.clearDisplay();
    display.setCursor(x, y);
    display.println(text);
    display.display();
    delay(30); // Adjust for speed
  }

  delay(delayTime);
}

void welcome()
{
  while (true)
  {
    displayCenteredText("Welcome", 2, 10, 500);
    displayCenteredText("To", 2, 10, 500);
    displayCenteredText("Rubik", 2, 10, 500);
    displayCenteredText("Solver", 2, 10, 500);
  }
}

void setup()
{
  Serial.begin(9600);
  display.begin(i2c_Address, true);
  delay(2000);
  welcome(); // Start the looping welcome function
}

void loop()
{
}
