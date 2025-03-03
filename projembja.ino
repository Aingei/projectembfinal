#include <Arduino.h>

#include <WiFi.h>
#include <MQTT.h>

// const int stepPins[] = {4, 16, 17, 5, 18, 19};
// const int dirPins[] = {12, 14, 27, 26, 25, 33};

const int stepPins[] = {32, 19, 26, 5, 14, 16};
const int dirPins[] = {25, 18, 27, 17, 12, 4};

#define STEP_PIN_1 32 // Motor 1 (U)
#define DIR_PIN_1 25

#define STEP_PIN_2 19 // Motor 2 (R)
#define DIR_PIN_2 18

#define STEP_PIN_3 26 // Motor 3 (F)
#define DIR_PIN_3 27

#define STEP_PIN_4 5 // Motor 4 (D)
#define DIR_PIN_4 17

#define STEP_PIN_5 14 // Motor 5 (L)
#define DIR_PIN_5 12

#define STEP_PIN_6 16 // Motor 6 (B)
#define DIR_PIN_6 4

const char ssid[] = "iloveaut";
const char pass[] = "autloveme";

const char mqtt_broker[] = "test.mosquitto.org";
const char mqtt_topic[] = "testjaja/command";
const char mqtt_topic1[] = "testjaja/commanddata";
const char mqtt_client_id[] = "arduino_group_pha"; // must change this string to a unique value
int MQTT_PORT = 1883;

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

const int enable_motor5 = 35;

void connect()
{
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("\nconnecting...");
    while (!client.connect(mqtt_client_id))
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nconnected!");

    client.subscribe(mqtt_topic);
}


void moveMotor(int stepPin, int dirPin, bool clockwise, int steps)
{
    digitalWrite(dirPin, clockwise ? HIGH : LOW);
    for (int i = 0; i < steps; i++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
    }
}

void U(bool prime = false) { moveMotor(STEP_PIN_1, DIR_PIN_1, !prime, 50); }
void R(bool prime = false) { moveMotor(STEP_PIN_2, DIR_PIN_2, !prime, 50); }
void Move_F(bool prime = false) { moveMotor(STEP_PIN_3, DIR_PIN_3, !prime, 50); }
void D(bool prime = false) { moveMotor(STEP_PIN_4, DIR_PIN_4, !prime, 50); }
void L(bool prime = false) { moveMotor(STEP_PIN_5, DIR_PIN_5, !prime, 50); }
void B(bool prime = false) { moveMotor(STEP_PIN_6, DIR_PIN_6, !prime, 50); }

void U2()
{
    U();
    U();
}
void R2()
{
    R();
    R();
}
void Move_F2()
{
    Move_F();
    Move_F();
}
void D2()
{
    D();
    D();
}
void L2()
{
    L();
    L();
}
void B2()
{
    B();
    B();
}

void solveSequence()
{

    U();
    delay(100);

    R();
    delay(100);

    Move_F();
    delay(100);

    D();
    delay(100);

    L();
    delay(100);

    B();
    delay(100);

    delay(1000);

    B(true);
    delay(100);

    L(true);
    delay(100);

    D(true);
    delay(100);

    Move_F(true);
    delay(100);

    R(true);
    delay(100);

    U(true);
    delay(100);
}

void messageReceived(String &topic, String &payload)
{
    Serial.println("incoming: " + topic + " - " + payload);
    if (topic == mqtt_topic)
    {
        if (payload == "U"){
            U();
            Serial.println("U");
        }else if (payload == "U'")
            U(true);
            
        else if (payload == "U2")
            U2();

        else if (payload == "R"){
            R();
            Serial.println("R");
        }else if (payload == "R'")
            R(true);
        else if (payload == "R2")
            R2();

        else if (payload == "F")
            Move_F();
        else if (payload == "F'")
            Move_F(true);
        else if (payload == "F2")
            Move_F2();

        else if (payload == "D")
            D();
        else if (payload == "D'")
            D(true);
        else if (payload == "D2")
            D2();

        else if (payload == "L")
            L();
        else if (payload == "L'")
            L(true);
        else if (payload == "L2")
            L2();

        else if (payload == "B")
            B();
        else if (payload == "B'")
            B(true);
        else if (payload == "B2")
            B2();
    }
}


void setup()
{

    Serial.begin(9600);
    WiFi.begin(ssid, pass);

    delay(1000);

    for (int i = 0; i < 6; i++)
    {
        pinMode(stepPins[i], OUTPUT);
        pinMode(dirPins[i], OUTPUT);
    }

    // pinMode(enable_motor5, OUTPUT);
    // digitalWrite(enable_motor5, HIGH);

    delay(1000);

    solveSequence();

    client.begin(mqtt_broker, MQTT_PORT, net);
    client.onMessage(messageReceived);

    connect();

    Serial.println("Ready to receive MQTT commands!");
}

void loop()
{
     if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi lost, reconnecting...");
        WiFi.begin(ssid, pass);
        delay(5000);  // Wait before retrying
    }

    if (!client.connected()) {
        Serial.println("MQTT disconnected, reconnecting...");
        connect();
    }

    client.loop();
    // const int steps = 2000;
    // const int delayMicros = 500;
    // const int delayMillis = 1000;

    // // Move all motors forward
    // for (int m = 0; m < 6; m++) {
    //   digitalWrite(dirPins[m], HIGH);
    //   for (int i = 0; i < steps; i++) {
    //     digitalWrite(stepPins[m], HIGH);
    //     delayMicroseconds(delayMicros);
    //     digitalWrite(stepPins[m], LOW);
    //     delayMicroseconds(delayMicros);
    //   }
    //   delay(delayMillis);
    // }  

    // // Move all motors backward
    // for (int m = 0; m < 6; m++) {
    //   digitalWrite(dirPins[m], LOW);
    //   for (int i = 0; i < steps; i++) {
    //     digitalWrite(stepPins[m], HIGH);
    //     delayMicroseconds(delayMicros);
    //     digitalWrite(stepPins[m], LOW);
    //     delayMicroseconds(delayMicros);
    //   }
    //   delay(delayMillis);
    // }
}
