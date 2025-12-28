#include <Servo.h>

/* ==========================================================
   STATUS MISI
   ========================================================== */
bool misiAktif  = false;   // diaktifkan CV / fallback
bool sudahPadam = false;

/* ==========================================================
   PUTAR BALIK
   ========================================================== */
bool sedangPutarBalik = false;
unsigned long waktuPutarBalik = 0;
const unsigned long DURASI_PUTAR_BALIK = 3000;

/* ==========================================================
   SERVO STEERING
   ========================================================== */
#define SERVO_PIN        9
#define SERVO_KIRI     180
#define SERVO_TENGAH    90
#define SERVO_KANAN     0

Servo steeringServo;

/* ==========================================================
   KOMANDO DARI ESP32 / CV
   ========================================================== */
#define PIN_KOMANDO_WIFI 2   // LOW = izin jalan (INPUT_PULLUP)

/* ==========================================================
   SENSOR IR
   ========================================================== */
#define IR_SENSOR_OUTER_RIGHT A4
#define IR_SENSOR_RIGHT       A3
#define IR_SENSOR_CENTER      A2
#define IR_SENSOR_LEFT        A1
#define IR_SENSOR_OUTER_LEFT  A0

#define THRESHOLD_IR 500

/* ==========================================================
   SENSOR API
   ========================================================== */
#define FLAME_KANAN 3
#define API_TERDETEKSI LOW

/* ==========================================================
   RELAY KIPAS
   ========================================================== */
#define RELAY_KIPAS 5
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

/* ==========================================================
   MOTOR DRIVER
   ========================================================== */
#define ENA 11
#define IN1 8
#define IN2 7

#define ENB 6
#define IN3 10
#define IN4 12

/* ==========================================================
   KECEPATAN
   ========================================================== */
int SPEED_LURUS = 75;
int MAX_PWM     = 100;

/* ==========================================================
   LINE FOLLOWER
   ========================================================== */
int error = 0;
int lastError = 0;

/* ==========================================================
   SETUP
   ========================================================== */
void setup() {
  Serial.begin(9600);

  pinMode(PIN_KOMANDO_WIFI, INPUT_PULLUP); // ANTI FLOATING

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(FLAME_KANAN, INPUT_PULLUP);
  pinMode(RELAY_KIPAS, OUTPUT);
  matikanKipas();

  steeringServo.attach(SERVO_PIN);
  steeringServo.write(SERVO_TENGAH);

  // 🔥 FALLBACK: robot tetap jalan walau WiFi mati
  misiAktif = false;

  Serial.println("✅ Robot siap dan AKTIF");
}

/* ==========================================================
   LOOP
   ========================================================== */
void loop() {

  /* ===============================
     TRIGGER DARI ESP32 / CV
     =============================== */
  if (digitalRead(PIN_KOMANDO_WIFI) == HIGH) {
    misiAktif = true;
  }

  /* ===============================
     JIKA TIDAK ADA MISI → DIAM
     =============================== */
  if (!misiAktif) {
    rotateMotor(0, 0);
    return;
  }

  /* ===============================
     DETEKSI API (STOP HANYA SAAT ADA API)
     =============================== */
  if (!sudahPadam && digitalRead(FLAME_KANAN) == API_TERDETEKSI) {

    Serial.println("🔥 API TERDETEKSI");

    rotateMotor(0, 0);
    nyalakanKipas();
    delay(2000);

    while (digitalRead(FLAME_KANAN) == API_TERDETEKSI) {
      nyalakanKipas();
      rotateMotor(0, 0);
    }
    sudahPadam = true;
    if (sudahPadam && digitalRead(FLAME_KANAN) != API_TERDETEKSI)
    {
      matikanKipas();
      sudahPadam = true;

      Serial.println("✅ API PADAM → PUTAR BALIK");
      aksiPutarBalik();

      delay(4500);

      steeringServo.write(SERVO_KANAN);
      rotateMotor(100, -100);

      delay(1000);
    }
  }

  /* ===============================
     PUTAR BALIK BERDASARKAN WAKTU
     =============================== */
//   if (sedangPutarBalik) {
//     if (millis() - waktuPutarBalik >= DURASI_PUTAR_BALIK) {
//       sedangPutarBalik = false;
//       steeringServo.write(SERVO_TENGAH);
//       rotateMotor(0, 0);
//     }
//     return;
//   }

  /* ===============================
     CEK HOME → STOP TOTAL
     =============================== */
  if (sudahPadam &&
      analogRead(IR_SENSOR_CENTER) > THRESHOLD_IR &&
      analogRead(IR_SENSOR_RIGHT) > THRESHOLD_IR &&
      analogRead(IR_SENSOR_OUTER_RIGHT) > THRESHOLD_IR &&
      analogRead(IR_SENSOR_LEFT) > THRESHOLD_IR &&
      analogRead(IR_SENSOR_OUTER_LEFT) > THRESHOLD_IR) {

    rotateMotor(0, 0);
    steeringServo.write(SERVO_TENGAH);
    aksiPutarBalik();
    Serial.println("🏠 HOME - MISI SELESAI");
    while (1);
  }

  /* ===============================
     JALANKAN LINE FOLLOWER
     =============================== */
  jalankanLineFollower();
}

/* ==========================================================
   PUTAR BALIK
   ========================================================== */
void aksiPutarBalik() {
      sedangPutarBalik = true;
      waktuPutarBalik = millis();

      steeringServo.write(SERVO_KIRI);
      rotateMotor(-100, 100);
}

/* ==========================================================
   RELAY KIPAS
   ========================================================== */
void nyalakanKipas() {
  digitalWrite(RELAY_KIPAS, RELAY_ON);
}

void matikanKipas() {
  digitalWrite(RELAY_KIPAS, RELAY_OFF);
}

/* ==========================================================
   LINE FOLLOWER
   ========================================================== */
void jalankanLineFollower() {

  int posisi = bacaSensor();

  if (posisi == -99) error = lastError;
  else error = posisi;

  lastError = error;

  int belokFactor = abs(error);
  int speed = SPEED_LURUS - (belokFactor * 15);
  speed = constrain(speed, 75, MAX_PWM);

  rotateMotor(speed, speed);

  int sudutServo = SERVO_TENGAH - (error * 15);
  sudutServo = constrain(sudutServo, SERVO_KANAN, SERVO_KIRI);
  steeringServo.write(sudutServo);
}

/* ==========================================================
   SENSOR IR
   ========================================================== */
int bacaSensor() {

  if (analogRead(IR_SENSOR_CENTER)      > THRESHOLD_IR) return  0;
  if (analogRead(IR_SENSOR_LEFT)        > THRESHOLD_IR) return -2;
  if (analogRead(IR_SENSOR_RIGHT)       > THRESHOLD_IR) return  2;
  if (analogRead(IR_SENSOR_OUTER_LEFT)  > THRESHOLD_IR) return -4;
  if (analogRead(IR_SENSOR_OUTER_RIGHT) > THRESHOLD_IR) return  4;

  return -99;
}

/* ==========================================================
   MOTOR CONTROL
   ========================================================== */
void rotateMotor(int rightSpeed, int leftSpeed) {

  rightSpeed = constrain(rightSpeed, -180, 180);
  leftSpeed  = constrain(leftSpeed,  -180, 180);

  digitalWrite(IN1, rightSpeed < 0);
  digitalWrite(IN2, rightSpeed > 0);

  digitalWrite(IN3, leftSpeed > 0);
  digitalWrite(IN4, leftSpeed < 0);

  analogWrite(ENA, abs(rightSpeed));
  analogWrite(ENB, abs(leftSpeed));
}
