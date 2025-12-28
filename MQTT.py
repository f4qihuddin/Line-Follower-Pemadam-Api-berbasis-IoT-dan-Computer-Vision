import cv2
import numpy as np
import paho.mqtt.client as mqtt
import time

# MQTT config
BROKER = ""   # IP broker
PORT = 1883
TOPIC = "fire/detection"

client = mqtt.Client()
client.connect(BROKER, PORT, 60)
client.loop_start()

cap = cv2.VideoCapture(0)

fire_detected = False

while True:
    ret, frame = cap.read()
    if not ret:
        break

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Range warna api (2 range merah)
    lower1 = np.array([0, 120, 70])
    upper1 = np.array([10, 255, 255])
    lower2 = np.array([170, 120, 70])
    upper2 = np.array([180, 255, 255])

    mask1 = cv2.inRange(hsv, lower1, upper1)
    mask2 = cv2.inRange(hsv, lower2, upper2)
    mask = mask1 + mask2

    fire_pixels = cv2.countNonZero(mask)

    if fire_pixels > 3000:
        if not fire_detected:
            print("🔥 API TERDETEKSI")
            client.publish(TOPIC, "FIRE")
            fire_detected = True
    else:
        if fire_detected:
            print("✅ TIDAK ADA API")
            client.publish(TOPIC, "NO_FIRE")
            fire_detected = False

    cv2.imshow("Fire Detection", mask)
    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
client.disconnect()
