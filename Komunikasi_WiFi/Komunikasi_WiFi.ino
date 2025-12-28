// #include <ESP8266WiFi.h> // Library Khusus NodeMCU
// #include <PubSubClient.h>

// // ==========================================================
// // 1. KONFIGURASI WIFI
// // ==========================================================
// const char* ssid = "Galaxy A206443";
// const char* password = "vexx7191";

// // ==========================================================
// // 2. MQTT BROKER
// // ==========================================================
// const char* mqtt_server = "10.63.199.50";
// const int mqtt_port = 1883;
// const char* topic_target = "fire/detection";

// // --- 3. PIN DEFINITION ---
// #define LED_PIN D4         // LED Bawaan NodeMCU (Active LOW)
// #define PIN_KE_ARDUINO D1  // Kabel sinyal ke Arduino Pin 2

// WiFiClient espClient;
// PubSubClient client(espClient);

// void setup() {
//   Serial.begin(115200);
  
//   pinMode(LED_PIN, OUTPUT);
//   pinMode(PIN_KE_ARDUINO, OUTPUT);

//   // KONDISI AWAL:
//   // LED NodeMCU Mati = HIGH (Karena logikanya terbalik)
//   // Sinyal ke Arduino Mati = LOW
//   digitalWrite(LED_PIN, HIGH);      
//   digitalWrite(PIN_KE_ARDUINO, LOW); 

//   // Tes Fisik LED (Kedip 3x tanda hidup)
//   Serial.println("\n\n--- MULAI SISTEM NODEMCU ---");
//   for(int i=0; i<3; i++) {
//     digitalWrite(LED_PIN, LOW);  delay(100); // Nyala
//     digitalWrite(LED_PIN, HIGH); delay(100); // Mati
//   }

//   setup_wifi();
  
//   client.setServer(mqtt_server, mqtt_port);
//   client.setCallback(callback);
// }

// void setup_wifi() {
//   delay(10);
//   Serial.println();
//   Serial.print("Menghubungkan ke WiFi: ");
//   Serial.println(ssid);

//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);

//   int percobaan = 0;
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//     percobaan++;
//     // Jika lama tidak konek, beri info
//     if (percobaan > 40) { // 20 detik
//        Serial.println("\n[GAGAL] Cek Password / Frekuensi 2.4GHz");
//        percobaan = 0;
//        WiFi.disconnect();
//        WiFi.begin(ssid, password);
//     }
//   }

//   Serial.println("\n✅ WiFi Terhubung!");
//   Serial.print("IP Address: ");
//   Serial.println(WiFi.localIP());
// }

// // void callback(char* topic, byte* payload, unsigned int length) {
// //   String message = "";
// //   for (int i = 0; i < length; i++) message += (char)payload[i];
// //   message.trim();

// //   Serial.print("Pesan: ["); Serial.print(message); Serial.println("]");

// //   if (message == "1") {
// //     // KASUS: ADA API
// //     // 1. Nyalakan LED NodeMCU (LOW = NYALA)
// //     digitalWrite(LED_PIN, LOW);
    
// //     // 2. Kirim Sinyal ke Arduino (HIGH = JALAN)
// //     digitalWrite(PIN_KE_ARDUINO, HIGH); 
    
// //     Serial.println("STATUS: 🔥 BAHAYA -> SINYAL HIGH DIKIRIM KE ARDUINO");
// //   }
  
// //   else if (message == "0") {
// //     // KASUS: AMAN
// //     // 1. Matikan LED NodeMCU (HIGH = MATI)
// //     digitalWrite(LED_PIN, HIGH);
    
// //     // 2. Matikan Sinyal ke Arduino (LOW = DIAM)
// //     digitalWrite(PIN_KE_ARDUINO, LOW);
    
// //     Serial.println("STATUS: ✅ AMAN -> SINYAL LOW DIKIRIM KE ARDUINO");
// //   }
// // }
// void callback(char* topic, byte* payload, unsigned int length) {
//   String message = "";
//   for (int i = 0; i < length; i++) message += (char)payload[i];
//   message.trim();

//   if (message == "1") {
//     digitalWrite(LED_PIN, LOW);          // LED nyala
//     digitalWrite(PIN_KE_ARDUINO, HIGH);  // KIRIM IZIN JALAN
//     Serial.println("🔥 IZIN JALAN DIKIRIM KE ARDUINO");
//   }
// }

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Konek ke Mosquitto...");
//     String clientId = "NodeMCU-Fire-" + String(random(0xffff), HEX);
    
//     if (client.connect(clientId.c_str())) {
//       Serial.println("BERHASIL!");
//       client.subscribe(topic_target);
//     } else {
//       Serial.print("Gagal rc=");
//       Serial.print(client.state());
//       Serial.println(" coba lagi 3 detik");
//       delay(3000);
//     }
//   }
// }

// void loop() {
//   if (!client.connected()) reconnect();
//   client.loop();
// }
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
