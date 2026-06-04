#define BLYNK_TEMPLATE_ID "TMPL6vW_xCGlN"
#define BLYNK_TEMPLATE_NAME "JavizeFAN V2"

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

// --- กำหนดขาพินรีเลย์ ---
#define RELAY_1 D1 // สปีด 1
#define RELAY_2 D2 // สปีด 2
#define RELAY_3 D3 // สปีด 3
#define RELAY_4 D5 // ใช้ขา D5 ตามโค้ดเดิมของคุณ

char auth[] = "XBzHPWNzCK_7bRzeVCQcleCkrb8we3TS";
char ssid[] = "JAOSAMUT_2.4G";
char pass[] = "09864261";

BlynkTimer timer;

// ตัวแปรนับเวลา
long autoOffSeconds = 0;
long autoOnSeconds = 0;
int autoOnTargetSpeed = 1;

// ฟังก์ชันปิดทุกอย่าง
void turnOffAll() {
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);
  
  // อัปเดตสถานะปุ่มในแอปให้ดับหมดก่อน
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 0);
  Blynk.virtualWrite(V5, 0);
}

// ฟังก์ชันเปิดพัดลมตามเบอร์
void setFanSpeed(int speed) {
  turnOffAll(); 
  
  if (speed == 0) {
    Blynk.virtualWrite(V1, 1); // ถ้าสั่งปิด ให้ปุ่มเบอร์ 0 (V1) ติดไฟแจ้งสถานะ
    return;
  }
  
  switch (speed) {
    case 1:
      digitalWrite(RELAY_1, LOW);
      Blynk.virtualWrite(V2, 1); // ปุ่มเบอร์ 1 คือ V2
      break;
    case 2:
      digitalWrite(RELAY_2, LOW);
      Blynk.virtualWrite(V3, 1); // ปุ่มเบอร์ 2 คือ V3
      break;
    case 3:
      digitalWrite(RELAY_4, LOW); // ใช้ D5 แทนตามบอร์ดคุณ
      Blynk.virtualWrite(V5, 1); // ปุ่มเบอร์ 3 คือ V5
      break;
  }
}

// ฟังก์ชันนับเวลาถอยหลัง (รันทุกๆ 1 วินาที)
void timerTick() {
  if (autoOffSeconds > 0) {
    autoOffSeconds--;
    if (autoOffSeconds == 0) {
      Serial.println("Timer Triggered: Auto OFF");
      setFanSpeed(0); 
    }
  }
  
  if (autoOnSeconds > 0) {
    autoOnSeconds--;
    if (autoOnSeconds == 0) {
      Serial.println("Timer Triggered: Auto ON");
      setFanSpeed(autoOnTargetSpeed); 
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  
  turnOffAll(); 
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, timerTick); // สั่งให้นับเวลาทุกๆ 1000ms (1 วินาที)
}

void loop() {
  Blynk.run();
  timer.run(); 
}

// === รับคำสั่งจากปุ่มกดในแอป/เว็บ ===
BLYNK_WRITE(V1) { if (param.asInt()) setFanSpeed(0); } // กด V1 = ปิด (เบอร์ 0)
BLYNK_WRITE(V2) { if (param.asInt()) setFanSpeed(1); else setFanSpeed(0); } // กด V2 = เบอร์ 1
BLYNK_WRITE(V3) { if (param.asInt()) setFanSpeed(2); else setFanSpeed(0); } // กด V3 = เบอร์ 2
BLYNK_WRITE(V5) { if (param.asInt()) setFanSpeed(3); else setFanSpeed(0); } // กด V5 = เบอร์ 3

// === รับคำสั่งตั้งเวลา ===
BLYNK_WRITE(V6) { autoOffSeconds = param.asInt(); }
BLYNK_WRITE(V7) { autoOnSeconds = param.asInt(); }
BLYNK_WRITE(V8) { 
  autoOnTargetSpeed = param.asInt(); 
  if (autoOnTargetSpeed < 1) autoOnTargetSpeed = 1;
  if (autoOnTargetSpeed > 3) autoOnTargetSpeed = 3;
}