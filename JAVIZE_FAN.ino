#define BLYNK_TEMPLATE_ID "TMPL6vW_xCGlN"
#define BLYNK_TEMPLATE_NAME "JavizeFAN V2"

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

// ขารีเลย์
#define BT1 D1 // ช่อง 1 (เว้นไว้ ไม่ใช้งาน)
#define BT2 D2 // ช่อง 2 -> ใช้กับสปีด 1
#define BT3 D3 // ช่อง 3 -> ใช้กับสปีด 2
#define BT5 D5 // ช่อง 4 -> ใช้กับสปีด 3

char auth[] = "XBzHPWNzCK_7bRzeVCQcleCkrb8we3TS";
char ssid[] = "JAOSAMUT_2.4G";
char pass[] = "09864261";

BlynkTimer timer;
long autoOffSeconds = 0;
long autoOnSeconds = 0;
int autoOnTargetSpeed = 1;

// ฟังก์ชันสำหรับสั่งปิดทุกอัน
void turnOffAll() {
  digitalWrite(BT1, HIGH); // ปิด D1 (ถึงจะเว้นไว้ก็สั่งปิดเพื่อความชัวร์)
  digitalWrite(BT2, HIGH); // ปิด D2
  digitalWrite(BT3, HIGH); // ปิด D3
  digitalWrite(BT5, HIGH); // ปิด D5
  
  // รีเซ็ตไฟสถานะปุ่มในแอป Blynk
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 0);
  Blynk.virtualWrite(V5, 0);
}

void setFanSpeed(int speed) {
  turnOffAll(); // ตัดไฟทุกอันก่อนเสมอ
  
  if (speed == 0) {
    return; // สั่งปิด จบการทำงาน
  }
  
  switch (speed) {
    case 1:
      digitalWrite(BT2, LOW); // **ขยับมาใช้ D2 (เว้น D1 ไว้)**
      Blynk.virtualWrite(V2, 1); 
      break;
    case 2:
      digitalWrite(BT3, LOW); // **ขยับมาใช้ D3**
      Blynk.virtualWrite(V3, 1); 
      break;
    case 3:
      digitalWrite(BT5, LOW); // **ใช้ D5 (ช่องนี้ถูกอยู่แล้ว)**
      Blynk.virtualWrite(V5, 1); 
      break;
  }
}

void timerTick() {
  if (autoOffSeconds > 0) {
    autoOffSeconds--;
    if (autoOffSeconds == 0) setFanSpeed(0); 
  }
  if (autoOnSeconds > 0) {
    autoOnSeconds--;
    if (autoOnSeconds == 0) setFanSpeed(autoOnTargetSpeed); 
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BT1, OUTPUT);
  pinMode(BT2, OUTPUT);
  pinMode(BT3, OUTPUT);
  pinMode(BT5, OUTPUT);
  
  turnOffAll(); 
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, timerTick); 
}

void loop() {
  Blynk.run();
  timer.run(); 
}

// === รับคำสั่งจากหน้าเว็บ/แอป ===
BLYNK_WRITE(V1) { if (param.asInt()) setFanSpeed(0); } // กดปุ่มปิด 
BLYNK_WRITE(V2) { if (param.asInt()) setFanSpeed(1); else setFanSpeed(0); } 
BLYNK_WRITE(V3) { if (param.asInt()) setFanSpeed(2); else setFanSpeed(0); } 
BLYNK_WRITE(V5) { if (param.asInt()) setFanSpeed(3); else setFanSpeed(0); } 

// === ระบบตั้งเวลา ===
BLYNK_WRITE(V6) { autoOffSeconds = param.asInt(); }
BLYNK_WRITE(V7) { autoOnSeconds = param.asInt(); }
BLYNK_WRITE(V8) { 
  autoOnTargetSpeed = param.asInt(); 
  if (autoOnTargetSpeed < 1) autoOnTargetSpeed = 1;
  if (autoOnTargetSpeed > 3) autoOnTargetSpeed = 3;
}