#include <SoftwareSerial.h>
#include <TheThingsNetwork.h>

// Define PINS
const byte ledPinYellow = 6;
const byte ledPinRed = 7;
const byte gasAnalogPin = A0;
const byte RXPin = 10;
const byte TXPin = 11;
#define ETEINDRE_LED 0
#define ALLUMER_LED 1
#define CLIGNOTER_LED 2
byte commande;

// Set your DevAddr, NwkSKey, AppSKey and the frequency plan
const char *devAddr = "26011072";
const char *nwkSKey = "B5882EE1202009675E917971350860FF";
const char *appSKey = "4C4E18A868F7BC42C9CC9ACD894F2921";
#define freqPlan TTN_FP_EU868

// Define PINS for connection between the Arduino and the Lora Shield + for debug
SoftwareSerial loraSerial(RXPin, TXPin); // RX, TX
#define debugSerial Serial
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  // Set the baud rate
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Set the pinMode
  pinMode(gasAnalogPin, INPUT);
  pinMode(ledPinYellow, OUTPUT);
  pinMode(ledPinRed, OUTPUT);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  // Set callback for incoming messages
  ttn.onMessage(message);

  debugSerial.println("-- PERSONALIZE");
  ttn.personalize(devAddr, nwkSKey, appSKey);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  commande = ETEINDRE_LED;
}

void loop()
{
  // Definition of variables
  float sensor_volt;
  float sensorValue;
  int valEntiere;
  int valDecimale;
  
  debugSerial.println("-- LOOP");
  debugSerial.print("-- GAS SENSOR : ");

  sensorValue = analogRead(gasAnalogPin);
  sensor_volt = sensorValue/1024*5.0;
  debugSerial.println(sensor_volt);
  
  // New format to send the data to TTN
  // If sensor_volt equals 2.45, it will become 2 variables : valEntiere = 2 and valDecimale = 45
  valEntiere = (int) sensor_volt;
  valDecimale = (sensor_volt - valEntiere)*100;
  debugSerial.print("Partie entiere : ");
  debugSerial.print(valEntiere);
  debugSerial.print("-- Partie decimale : ");
  debugSerial.print(valDecimale);

  // Prepare payload of 1 byte to indicate LED status
  byte payload[2];
  payload[0] = valEntiere;
  payload[1] = valDecimale;

  // send the frame
  ttn.sendBytes(payload, sizeof(payload));

  // Setup an alarm if the sensor detect a high-voltage gas
  // It will light on the red light if there is a danger
  if(sensor_volt > 0.75) {
    digitalWrite(ledPinRed, HIGH);
  } else {
    digitalWrite(ledPinRed, LOW);
  }

  // These functions can be launch by rednode wirelessly OR by the console on theThingsNetwork
  if(commande == 1) {
    debugSerial.println("LED: on");
    digitalWrite(ledPinYellow, HIGH);
  } else if(commande == 0) {
    debugSerial.println("LED: off");
    digitalWrite(ledPinYellow, LOW);
  } else if(commande == 2) {
      debugSerial.println("LED: blink");
      if(digitalRead(ledPinYellow) == 1) {
        digitalWrite(ledPinYellow, LOW);
      } else {
        digitalWrite(ledPinYellow, HIGH);
      }
  }

  // Send single byte to poll for incoming messages. You need to activate it if you want downlink message without using sendBytes() function
  //ttn.poll();

  // Define the periodicity of your sending data function
  delay(4000);
}

// Handle message from node-red or theThingsNetwork
void message(const byte *payload, size_t length, port_t port)
{
  debugSerial.println("-- MESSAGE");

  // Only handle messages of a single byte
  if (length != 1)
  {
    return;
  }

  commande = payload[0];
}
