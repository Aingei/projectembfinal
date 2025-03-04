#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.H>

const char ssid[] = "iloveaut";
const char pass[] = "autloveme";

const char mqtt_broker[] = "test.mosquitto.org";
const char mqtt_topic[] = "iloveaut/rubik/command";
const char mqtt_client_id[] = "rubik_solver_inside";
int MQTT_PORT = 1883;

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

const int stepPins[] = {32, 19, 26, 5, 14, 16};
const int dirPins[] = {25, 18, 27, 17, 12, 4};




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
    // client.unsubscribe("/hello");
}

// Move Execution Function
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

// Individual Move Functions
void U(bool prime = false) { moveMotor(32, 25, !prime, 50); }
void R(bool prime = false) { moveMotor(19, 18, !prime, 50); }
void Move_F(bool prime = false) { moveMotor(26, 27, !prime, 50); }
void D(bool prime = false) { moveMotor(5, 17, !prime, 50); }
void L(bool prime = false) { moveMotor(14, 12, !prime, 50); }
void B(bool prime = false) { moveMotor(16, 4, !prime, 50); }


void executeMoveString(String moveSequence) {
    // Split the moveSequence string into individual moves

    unsigned long startTime = millis(); // Start time tracking

    int start = 0;
    for (int i = 0; i < moveSequence.length(); i++) {
        if (moveSequence[i] == ' ' || i == moveSequence.length() - 1) {
            // Extract the move substring
            String move = moveSequence.substring(start, i + (i == moveSequence.length() - 1));
            start = i + 1;

            // Determine the move type and execute directly
            if (move == "U") U();
            else if (move == "R") R();
            else if (move == "F") Move_F();
            else if (move == "D") D();
            else if (move == "L") L();
            else if (move == "B") B();
            else if (move == "U'") U(true);
            else if (move == "R'") R(true);
            else if (move == "F'") Move_F(true);
            else if (move == "D'") D(true);
            else if (move == "L'") L(true);
            else if (move == "B'") B(true);
            else if (move == "U2") { U(); U(); }
            else if (move == "R2") { R(); R(); }
            else if (move == "F2") { Move_F(); Move_F(); }
            else if (move == "D2") { D(); D(); }
            else if (move == "L2") { L(); L(); }
            else if (move == "B2") { B(); B(); }

            delay(500);
        }
    }

    unsigned long endTime = millis(); // End time tracking
    unsigned long executionTime = endTime - startTime; // Calculate execution time

    String timeMessage = String(executionTime);
    client.publish("iloveaut/rubik/time", timeMessage);
}

void messageReceived(String &topic, String &payload)
{
    Serial.println("incoming: " + topic + " - " + payload);

    executeMoveString(payload);
}

void setup()
{
    delay(1000);

    for (int i = 0; i < 6; i++)
    {
        pinMode(stepPins[i], OUTPUT);
        pinMode(dirPins[i], OUTPUT);
    }

    Serial.begin(9600);
    WiFi.begin(ssid, pass);
    client.begin(mqtt_broker, MQTT_PORT, net);
    client.onMessage(messageReceived);

    connect();

}

// Loop Function (Unused for now)
void loop()
{
    client.loop();
    delay(10);

    if (!client.connected())
    {
        connect();
    }

    if (millis() - lastMillis > 2000)
    {
        // lastMillis = millis();
        // client.publish(mqtt_topic, "Counter = " + String());
    }
}