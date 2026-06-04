#define BLYNK_TEMPLATE_ID "TMPL6vW_xCGlN"
#define BLYNK_TEMPLATE_NAME "JavizeFAN V2"

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

// --- กำหนดขาพินรีเลย์ ---
#define RELAY_1 D1 // สปีด 1 (V1)
#define RELAY_2 D2 // สปีด 2 (V2)
#define RELAY_3 D3 // สปีด 3 (V3)
#define RELAY_4 D5 // สปีด 4 (V5) 

char auth[] = "XBzHPWNzCK_7bRzeVCQcleCkrb8we3TS";
char ssid[] = "JAOSAMUT_2.4G";
char pass[] = "09864261";

WidgetLED BT1Blynk(V10);
WidgetLED BT2Blynk(V11);
WidgetLED BT3Blynk(V12);
WidgetLED BT5Blynk(V13);

BlynkTimer timer;

// --- ตัวแปรสำหรับระบบนับเวลา (นับเป็นวินาที) ---
long autoOffSeconds = 0;
long autoOnSeconds = 0;
int autoOnTargetSpeed = 1;

// ฟังก์ชันเคลียร์รีเลย์และอัปเดตหน้าจอแอปให้เป็น Off ทั้งหมด
void turnOffAll() {
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);
  
  BT1Blynk.off();
  BT2Blynk.off();
  BT3Blynk.off();
  BT5Blynk.off();
  
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 0);
  Blynk.virtualWrite(V5, 0);
}

// ฟังก์ชันจัดการเปิดพัดลมตามเบอร์
void setFanSpeed(int speed) {
  turnOffAll(); // ปิดทุกตัวก่อนเสมอเพื่อป้องกันไฟชนกัน
  
  if (speed == 0) return; // ถ้าสั่ง 0 (ปิด) ให้จบการทำงานแค่นี้
  
  switch (speed) {
    case 1:
      digitalWrite(RELAY_1, LOW);
      BT1Blynk.on();
      Blynk.virtualWrite(V1, 1);
      break;
    case 2:
      digitalWrite(RELAY_2, LOW);
      BT2Blynk.on();
      Blynk.virtualWrite(V2, 1);
      break;
    case 3:
      digitalWrite(RELAY_3, LOW);
      BT3Blynk.on();
      Blynk.virtualWrite(V3, 1);
      break;
    case 4: // ใช้ V5
      digitalWrite(RELAY_4, LOW);
      BT5Blynk.on();
      Blynk.virtualWrite(V5, 1);
      break;
  }
}

// --- ฟังก์ชันนี้จะถูกเรียกทุกๆ 1 วินาที (1000ms) ---
void timerTick() {
  // 1. นับเวลาถอยหลังเพื่อปิด (Auto-OFF)
  if (autoOffSeconds > 0) {
    autoOffSeconds--;
    if (autoOffSeconds == 0) {
      Serial.println("Timer Triggered: Auto OFF");
      setFanSpeed(0); 
    }
  }
  
  // 2. นับเวลาถอยหลังเพื่อเปิด (Auto-ON)
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
  
  turnOffAll(); // รีเซ็ตทุกอย่างให้เป็น OFF ก่อนตอนเปิดบอร์ด
  
  Blynk.begin(auth, ssid, pass);
  
  // สั่งให้ timer เรียกใช้ฟังก์ชัน timerTick ทุกๆ 1 วินาที
  timer.setInterval(1000L, timerTick);
}

void loop() {
  Blynk.run();
  timer.run(); // ต้องรัน timer ใน loop เสมอ
}

// ==========================================
// ส่วนรับคำสั่งจากปุ่มเปิด-ปิดปกติบนแอป (Manual)
// ==========================================
BLYNK_WRITE(V1) { if (param.asInt()) setFanSpeed(1); else setFanSpeed(0); }
BLYNK_WRITE(V2) { if (param.asInt()) setFanSpeed(2); else setFanSpeed(0); }
BLYNK_WRITE(V3) { if (param.asInt()) setFanSpeed(3); else setFanSpeed(0); }
BLYNK_WRITE(V5) { if (param.asInt()) setFanSpeed(4); else setFanSpeed(0); }
BLYNK_WRITE(V4) { setFanSpeed(0); } // ปุ่ม ปิดทั้งหมด

// ==========================================
// ส่วนรับคำสั่งตั้งเวลามาจากฝั่ง Web (ส่งมาเป็นวินาที)
// ==========================================
BLYNK_WRITE(V6) { 
  autoOffSeconds = param.asInt(); 
  Serial.print("Set Auto OFF in (sec): "); Serial.println(autoOffSeconds);
}

BLYNK_WRITE(V7) { 
  autoOnSeconds = param.asInt(); 
  Serial.print("Set Auto ON in (sec): "); Serial.println(autoOnSeconds);
}

BLYNK_WRITE(V8) { 
  autoOnTargetSpeed = param.asInt(); 
  if (autoOnTargetSpeed < 1) autoOnTargetSpeed = 1;
  if (autoOnTargetSpeed > 4) autoOnTargetSpeed = 4;
}