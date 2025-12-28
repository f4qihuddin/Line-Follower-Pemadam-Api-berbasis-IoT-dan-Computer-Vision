import paho.mqtt.client as mqtt # type: ignore

# Konfigurasi MQTT
broker = "10.47.90.50" # Ganti dengan broker MQTT Anda
port = 1883 # Default MQTT port
topic = "test/topic" # Topik yang digunakan untuk berkomunikasi

# Fungsi untuk mengirim pesan
def publish_message(message):
  client = mqtt.Client()
  client.connect(broker, port, 60)
  client.publish(topic, message)
  print(f"Pesan '{message}' telah dikirim ke topik '{topic}'")
  client.loop(1)
  client.disconnect()

# Contoh penggunaan
if __name__ == "__main__":
  message = "OFF" # Ganti sesuai perintah yang diinginkan
  publish_message(message)