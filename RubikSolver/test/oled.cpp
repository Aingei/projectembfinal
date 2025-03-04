#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <MQTT.H>

const char ssid[] = "iloveaut";
const char pass[] = "autloveme";

const char mqtt_broker[] = "test.mosquitto.org";
const char mqtt_topic[] = "iloveaut/rubik/command";
const char mqtt_client_id[] = "rubik_solver"; // must change this string to a unique value
int MQTT_PORT = 1883;

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

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

int counter = 0;

bool lastWiFiStatus = false;
bool lastMQTTStatus = false;

String lastMessage = ""; // Store the last received MQTT message
unsigned long messageDisplayTime = 0;
const unsigned long messageTimeout = 5000; // Display message for 5 seconds

String lastMoveSequence = "";   // Variable to store move sequence
String lastExecutionTime = "";  // Variable to store execution time

void displayWiFiAndMQTTStatus()
{
  bool currentWiFiStatus = (WiFi.status() == WL_CONNECTED);
  bool currentMQTTStatus = client.connected();

  if (currentWiFiStatus != lastWiFiStatus || currentMQTTStatus != lastMQTTStatus)
  {
    lastWiFiStatus = currentWiFiStatus;
    lastMQTTStatus = currentMQTTStatus;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);

    display.setCursor(0, 0);
    display.println(currentWiFiStatus ? "WiFi: Connected" : "WiFi: Connecting...");

    display.setCursor(0, 10);
    display.println(currentMQTTStatus ? "MQTT: Connected" : "MQTT: Connecting...");

    display.display();
  }
}

void connect()
{
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    displayWiFiAndMQTTStatus();
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(mqtt_client_id))
  {
    Serial.print(".");
    displayWiFiAndMQTTStatus();
    delay(1000);
  }

  Serial.println("\nconnected!");
  client.subscribe(mqtt_topic);
}


void messageReceived(String &topic, String &payload)
{
    // Handle move sequence (from topic "iloveaut/rubik/command")
    if (topic == "iloveaut/rubik/command")
    {
        lastMoveSequence = payload; // Store the move sequence
    }

    // Handle execution time (from topic "iloveaut/rubik/time")
    else if (topic == "iloveaut/rubik/time")
    {
        lastExecutionTime = payload; // Store the execution time
    }

    // Display both the move sequence and execution time if both are received
    if (lastMoveSequence != "" && lastExecutionTime != "")
    {
        display.clearDisplay();        // Clear the screen before updating
        display.setTextSize(1);        // Set text size
        display.setTextColor(SH110X_WHITE); // Set text color

        // Display Move Sequence
        display.setCursor(0, 0);  // Set cursor to the top left corner
        display.print("Moves: ");
        display.println(lastMoveSequence);  // Print the move sequence

        // Display Execution Time
        display.setCursor(0, display.getCursorY() + 10);  // Move cursor down
        display.print("Solve Time: ");
        
        float execTimeInSec = lastExecutionTime.toFloat() / 1000.0;
        display.print(execTimeInSec, 3);  // Print the time with 3 decimal places
        display.println(" sec");  // Add "sec" to the execution time

        display.display();  // Update the display
    }
}



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

  displayCenteredText("Welcome", 2, 10, 500);
  displayCenteredText("To", 2, 10, 500);
  displayCenteredText("Rubik", 2, 10, 500);
  displayCenteredText("Solver", 2, 10, 500);
}

void setup()
{
  Serial.begin(9600);
  display.begin(i2c_Address, true);

  WiFi.begin(ssid, pass);

  client.begin(mqtt_broker, MQTT_PORT, net);
  client.onMessage(messageReceived);

  delay(2000);
  welcome();

  connect();

  client.subscribe("iloveaut/rubik/time");
}

void loop()
{
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if (!client.connected())
  {
    connect();
    // displayWiFiAndMQTTStatus();
  }

  displayWiFiAndMQTTStatus();


  if (millis() - messageDisplayTime > messageTimeout)
  {
    displayWiFiAndMQTTStatus();
  }
}
