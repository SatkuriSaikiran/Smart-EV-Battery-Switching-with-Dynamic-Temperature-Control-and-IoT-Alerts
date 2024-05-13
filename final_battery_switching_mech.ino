#define BLYNK_TEMPLATE_ID "TMPL3rpiqCmmI"
#define BLYNK_TEMPLATE_NAME "Battery Switching and Cooling System"
#define BLYNK_AUTH_TOKEN "0jNw4UWlhD-OUTAyiShLednf199z7t8f"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPIN D5
#define DHTTYPE DHT22
#define FAN_PIN  D2
#define RELAY_1  D3  // First relay control pin
#define RELAY_2  D4  // Second relay control pin

WidgetLED FAN(V0);

char auth[] = "0jNw4UWlhD-OUTAyiShLednf199z7t8f"; // Updated Blynk authentication token
char ssid[] = "Flowingcosmos";
char pass[] = "bunny216";

float humDHT = 0;
float tempDHT = 0;
int Val = 0;

bool thresholdSet = false; // Flag to indicate if threshold has been set

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);

  Serial.println(F("DHTxx test!"));
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initially turning off the relays and the fan
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
}

BLYNK_WRITE(V3)
{
  Val = param.asInt();
  Serial.print(" The Threshold value is: ");
  Serial.println(Val);
  Serial.println();
  thresholdSet = true; // Set the flag indicating threshold has been set
}

void loop() {
  Blynk.run();

  delay(2000);

  humDHT = dht.readHumidity();
  tempDHT = dht.readTemperature();

  if (!thresholdSet) {
    // If threshold is not set yet, do nothing and return
    return;
  }

  if (isnan(humDHT) || isnan(tempDHT))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print(F("Temperature: "));
  Serial.print(tempDHT);
  Serial.print(F("°C "));
  Serial.println();
  Serial.print(F("Humidity: "));
  Serial.print(humDHT);
  Serial.print(F("%"));
  Serial.println();
  
  Serial.println("***********************");
  Serial.println();

  if (Val > tempDHT)
  {
    digitalWrite(FAN_PIN, HIGH);
    FAN.off();
    digitalWrite(RELAY_1, HIGH);  // Turn on first relay
    digitalWrite(RELAY_2, LOW);   // Turn off second relay
  }
  else {
    digitalWrite(FAN_PIN, LOW);
    FAN.on();
    digitalWrite(RELAY_1, LOW);   // Turn off first relay
    digitalWrite(RELAY_2, HIGH);  // Turn on second relay
  } 

  Blynk.virtualWrite(V1, tempDHT);
  Blynk.virtualWrite(V2, humDHT);
}
