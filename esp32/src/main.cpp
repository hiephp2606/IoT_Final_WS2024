#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Cấu hình MQTT
const char* mqtt_server = "192.168.28.105"; // MQTT broker
const char* mqtt_topic = "home/sensors";      // Topic gửi dữ liệu
WiFiClient espClient;
PubSubClient client(espClient);

// Cấu hình WiFi
const char* ssid = "P2.8";
const char* password = "244466666";

// Cảm biến DHT11
#define DHTPIN 15  // Chân kết nối DATA của DHT11
#define DHTTYPE DHT11
#define DHTTEMPTHRESH 30
DHT dht(DHTPIN, DHTTYPE);

// MQ2
#define MQ2PIN 36  // Chân analog kết nối với MQ2
#define MQ2THRESH 2000


// Buzzer
#define BUZZERPIN 26
#define LEDPIN 27

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void setup() {
  Serial.begin(115200);

  // Kết nối WiFi
  setup_wifi();

  // Cấu hình MQTT
  client.setServer(mqtt_server, 1883);

  // Khởi tạo DHT11
  dht.begin();

  // Cấu hình chân Buzzer
  pinMode(BUZZERPIN, OUTPUT);
  digitalWrite(BUZZERPIN, LOW);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Đọc dữ liệu từ DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Đọc giá trị từ MQ2
  int gasValue = analogRead(MQ2PIN);

  // Kiểm tra điều kiện và điều khiển Buzzer
  if (gasValue > MQ2THRESH || temperature > DHTTEMPTHRESH) {  // Giá trị ngưỡng (tuỳ chỉnh)
    digitalWrite(BUZZERPIN, HIGH);
    digitalWrite(LEDPIN, HIGH);
  } else {
    digitalWrite(BUZZERPIN, LOW);
    digitalWrite(LEDPIN, LOW);
  }

  // Gửi dữ liệu lên MQTT
  char message[50];
  snprintf(message, 50, "Temp: %.2f, Humidity: %.2f, Gas: %d", temperature, humidity, gasValue);
  client.publish(mqtt_topic, message);

  Serial.println(message);
  delay(1000);  // Gửi dữ liệu mỗi 1 giây*/
}
