#include <SPI.h>
  #include <MFRC522.h>
  #include <LiquidCrystal.h>
  #include <Servo.h>
  
  // RFID
  #define SS_PIN 10
  #define RST_PIN 9
  MFRC522 rfid(SS_PIN, RST_PIN);
  
  // LCD (RS, E, D4, D5, D6, D7)
  LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
  
  // Servo
  Servo myServo;
  #define SERVO_PIN 8
  
  
  // UID thẻ hợp lệ
  byte validUID[] = {0xBB, 0xF7, 0x43, 0x02}; 
  
  void setup() {
    Serial.begin(115200);
    SPI.begin();
    rfid.PCD_Init();
  
    lcd.begin(16, 2);
    lcd.print("Quet the...");
  
    myServo.attach(SERVO_PIN);
    myServo.write(180);  // đóng cửa ban đầu
  }
  
 void loop() {
  // ==== ƯU TIÊN: NHẬN DỮ LIỆU TỪ ESP8266 ====
  while (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "OBSTACLE:DETECTED") {
      lcd.clear();
      lcd.print("Co Chuyen Dong");
      delay(2000);
      lcd.clear();
      lcd.print("Quet the...");
    }
  }
  // ==== ĐỌC THẺ RFID ====
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;  // Để sau khi xử lý LCD tránh bị thoát sớm
  }

  // ==== KIỂM TRA UID ====
  bool isValid = true;
  Serial.print("UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
    if (rfid.uid.uidByte[i] != validUID[i]) isValid = false;
  }
  Serial.println();

  // ==== HÀNH ĐỘNG ====
  if (isValid) {
    lcd.clear();
    lcd.print("Welcome!");
    myServo.write(90);
    delay(5000);
    myServo.write(180);
  } else {
    lcd.clear();
    lcd.print("Sai the!");
    delay(2000);
  }

  lcd.clear();
  lcd.print("Quet the...");

  rfid.PICC_HaltA();
}
