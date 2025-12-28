#include <WiFi.h>
#include <PubSubClient.h>

/* ================= WIFI ================= */
const char* ssid = "Galaxy A206443";
const char* password = "vexx7191";

/* ================= MQTT ================= */
const char* mqtt_server = "10.73.95.50";
const int mqtt_port = 1883;
const char* topic_target = "fire/detection";

/* ================= PIN ================= */
#define LED_PIN 2              // LED builtin ESP32
#define PIN_KE_ARDUINO 18      // GPIO aman ke Arduino

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(PIN_KE_ARDUINO, OUTPUT);

  digitalWrite(LED_PIN, LOW);         // LED mati awal
  digitalWrite(PIN_KE_ARDUINO, LOW);  // Sinyal mati

  Serial.println("\n--- ESP32 FIRE SYSTEM ---");

  // Tes LED
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) message += (char)payload[i];
  message.trim();

  if (message == "1") {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(PIN_KE_ARDUINO, HIGH);
    Serial.println("🔥 IZIN JALAN DIKIRIM KE ARDUINO");
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(PIN_KE_ARDUINO, LOW);
    Serial.println("✅ AMAN");
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32-Fire-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(topic_target);
    } else {
      delay(3000);
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
