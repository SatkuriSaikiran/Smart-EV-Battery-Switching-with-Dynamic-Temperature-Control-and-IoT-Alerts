#define BLYNK_TEMPLATE_ID "TMPL3tO8iD5pp"
#define BLYNK_TEMPLATE_NAME "Smart EV Battery Monitoring and IOT alerts"
#define BLYNK_AUTH_TOKEN "AQIBX5XWF5pW8ehvHXAm4wMM61RxBbnf"

#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h> // Include ESP8266WiFi library

#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define DHTTYPE DHT22
#define DHTPIN D4

char ssid[] = "Flowingcosmos"; // Change this to your WiFi SSID
char pass[] = "bunny216";      // Change this to your WiFi password

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float voltage;
float bat_percentage;
int analogInPin = A0;
int sensorValue;
float calibration = 0.40;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialize WiFi using hardcoded SSID and password
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.config(BLYNK_AUTH_TOKEN);

  dht.begin(); // Initialize DHT sensor

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("KITS EEE  Smart EV"); // Display welcome message
  display.display();
  delay(3000); // Display welcome message for 3 seconds
}

void loop() {
  Blynk.run();

  float t = dht.readTemperature();

  // Check if temperature reading is valid
  if (isnan(t)) {
    Serial.println("Failed to read temperature from DHT sensor!");
    // Handle error, possibly retry reading or display an error message
    t = 0; // Set temperature to 0 to avoid displaying "NaN"
  }

  sensorValue = analogRead(analogInPin);
  voltage = (((sensorValue * 3.3) / 1024) * 2 + calibration);

  bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 100);

  if (bat_percentage >= 100) {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0) {
    bat_percentage = 1;
  }

  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, bat_percentage);
  Blynk.virtualWrite(V3, voltage);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

  Serial.print("Battery Percentage: ");
  Serial.print(bat_percentage);
  Serial.println(" %");

  Serial.print("Analog Value = ");
  Serial.println(sensorValue);
  Serial.print("Output Voltage = ");
  Serial.println(voltage);
  Serial.print("Battery Percentage = ");
  Serial.println(bat_percentage);

  Serial.println();
  Serial.println("****************************");
  Serial.println();
  delay(1000);

  if (bat_percentage <= 30) {
    Serial.println("Battery level below 30%, Charge battery on time");
    Blynk.logEvent("battery_low", "Battery is getting low.... Plugin to charge");
    delay(500);
  }

  if (t >= 37) { // If temperature is higher than 35°C
    Serial.println("High temperature detected. Sending notification.");
    Blynk.setProperty(V4, "color", "#D3435C"); // Set color to red
    Blynk.setProperty(V4, "label", "High Temp!"); // Set label to "High Temp!"
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("HighTemp!Drive Carefully");
    display.display();
    Blynk.logEvent("high_temperature", "HighTempDetected");
    delay(5000); // Display notification for 5 seconds
  } else {
    // Set color back to default
    Blynk.setProperty(V4, "color", "#23C48E");
    Blynk.setProperty(V4, "label", "Temperature");

    // Display temperature and battery percentage
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Temperature: ");
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print(t);
    display.print(" ");
    display.setTextSize(1);
    display.cp437(true);
    display.write(167);
    display.setTextSize(2);
    display.print("C");

    display.setTextSize(1);
    display.setCursor(0, 35);
    display.print("Battery: ");
    display.setTextSize(2);
    display.setCursor(0, 45);
    display.print(bat_percentage);
    display.print(" %");

    display.display();
    delay(1500);
  }
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
