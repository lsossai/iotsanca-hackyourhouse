
#include <ESP8266WiFi.h>

char ssid[] = "Asilo 2.4 GHz";      //  your network SSID (name)
char pass[] = "reppolter99";   // your network password
String Device_Token = "db05824d-0dfc-47d6-baeb-8a499b36c3d1";

int sensorPin = A0;       // select the input pin for the analog input
int rawvoltage = 0;       // variable to store the value coming from the sensor
float sensorValue = 0;
String value_string = "";

int status = WL_IDLE_STATUS;

// Initialize the Wifi client library
WiFiClient client;

// server address:
char server[] = "api.tago.io";

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 2L * 1000L; // delay between updates, in milliseconds

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;                     // wait for serial port to connect. Needed for native USB port only
  }
  SetupWifi();
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if TWO seconds have passed since your last connection,
  // then connect again and send data:

  if (millis() - lastConnectionTime > postingInterval) {
    // read the value from the sensor:
    //rawvoltage = analogRead(sensorPin);
    rawvoltage = 500;
    // converting that reading to voltage, for 3.3v voltage
    float voltage = rawvoltage * 3.3;
    voltage /= 1024.0;
    // converting to Celsius
    float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
    //to degrees ((voltage - 500mV) times 100)

    int i = (int) temperatureC;                   //convert data format from float to int
    value_string = String(i);                     //end of conversion, to finally get it in the String format(Celsius)
    Serial.println(value_string);

    // then, send data to Tago
    httpRequest();
  }

}

void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  String PostData = String("{\"variable\":\"teste\", \"value\":") + String(value_string) + String(",\"unit\":\"C\"}");
  String Dev_token = String("Device-Token: ") + String(Device_Token);
  if (client.connect(server, 80)) {                     // we will use non-secured connnection (HTTP) for tests
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /data? HTTP/1.1");
    client.println("Host: api.tago.io");
    client.println("_ssl: false");                        // for non-secured connection, use this option "_ssl: false"
    client.println(Dev_token);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

void SetupWifi() {

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
}
