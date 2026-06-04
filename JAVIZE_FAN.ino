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

void setFanSpeed(int speed) {
  // 1. ส่วนฮาร์ดแวร์: ทำงานรวดเดียวจบในระดับไมโครวินาที
  // ต้องตัดไฟทุกอันก่อนเสมอเพื่อป้องกันขดลวดมอเตอร์พัดลมช็อต (Break before make)
  digitalWrite(BT1, HIGH);
  digitalWrite(BT2, HIGH);
  digitalWrite(BT3, HIGH);
  digitalWrite(BT5, HIGH);
  
  // สั่งเปิดเบอร์ใหม่ทันที (พัดลมจะเปลี่ยนเบอร์สมูท ไม่สะดุด)
  if (speed == 1) digitalWrite(BT2, LOW);
  if (speed == 2) digitalWrite(BT3, LOW);
  if (speed == 3) digitalWrite(BT5, LOW);

  // 2. ส่วนซอฟต์แวร์: อัปเดตหน้าจอ Blynk หลังจากสับสวิตช์พัดลมเสร็จแล้ว
  Blynk.virtualWrite(V1, speed == 0 ? 1 : 0);
  Blynk.virtualWrite(V2, speed == 1 ? 1 : 0);
  Blynk.virtualWrite(V3, speed == 2 ? 1 : 0);
  Blynk.virtualWrite(V5, speed == 3 ? 1 : 0);
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
  
  // ปิดรีเลย์ทุกตัวตอนบอร์ดเพิ่งเปิด
  digitalWrite(BT1, HIGH);
  digitalWrite(BT2, HIGH);
  digitalWrite(BT3, HIGH);
  digitalWrite(BT5, HIGH);
  
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, timerTick); 
}

void loop() {
  Blynk.run();
  timer.run(); 
}

// === รับคำสั่งจากหน้าเว็บ/แอป ===
BLYNK_WRITE(V1) { if (param.asInt()) setFanSpeed(0); } 
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