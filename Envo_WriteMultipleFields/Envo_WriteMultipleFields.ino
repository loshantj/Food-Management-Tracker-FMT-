#include <ESP8266WiFi.h>
#define SECRET_SSID 
#define SECRET_PASS
#define SECRET_CH_ID 
#define SECRET_WRITE_APIKEY 
#include "ThingSpeak.h" 
#include "DHT.h"
#include <Wire.h>
#include <BH1750.h>

#define DHTPIN 0   // Define the digital pin where the DHT11 is connected
#define DHTTYPE DHT11   // DHT11 is the type of sensor being used

DHT dht(DHTPIN, DHTTYPE);
const int tilt=16;

char ssid[] = "TJ30 PRO";   // your network SSID (name) 
char pass[] = "11111111";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
BH1750 lightMeter;

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "GVGNGTLN204MTDCP";

// Initialize our values
int number1 = 0;
int number2 = 0;
int number3 = 0;
int number4 = 0;
String myStatus = "";

void setup() {
  const int tilt=2;
  Serial.begin(115200);  // Initialize serial
  dht.begin();
  Wire.begin();
  lightMeter.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  float temperature = dht.readTemperature();  // Read temperature in degrees Celsius
  float humidity = dht.readHumidity();        // Read humidity as a percentage
  float lux = lightMeter.readLightLevel();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
 
 int sensorValue = digitalRead(tilt);
 Serial.println(sensorValue);


  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  // set the fields with the values
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, lux);
  ThingSpeak.setField(4, sensorValue);

  // figure out the status message
  if(number1 > number2){
    myStatus = String("field1 is greater than field2"); 
  }
  else if(number1 < number2){
    myStatus = String("field1 is less than field2");
  }
  else{
    myStatus = String("field1 equals field2");
  }
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // change the values
  number1++;
  if(number1 > 99){
    number1 = 0;
  }
  number2 = random(0,100);
  number3 = random(0,100);
  number4 = random(0,100);
  
  delay(10000); // Wait 20 seconds to update the channel again
}
