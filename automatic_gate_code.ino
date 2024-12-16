#include <WiFi.h>
#include <HTTPClient.h>
#include <Servo.h>
#include <Wire.h>
#include <RTClib.h>

Servo myservo;
RTC_DS1307 rtc;

int pos = 0;
int cm = 0;
bool gateOpen = false;

long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

void setup() {
  myservo.attach(9);
  Serial.begin(9600);

  // Inisialisasi RTC
  if (!rtc.begin()) {
    Serial.println("RTC tidak terdeteksi! Melanjutkan tanpa RTC...");
    return;
  }

  // Cek apakah RTC berjalan, jika tidak set waktu manual
  if (!rtc.isrunning()) {
    Serial.println("RTC tidak berjalan, set waktu sekarang...");

    // Set waktu ke 2024-12-11 10:00:00 (ubah sesuai kebutuhan Anda)
    rtc.adjust(DateTime(2024, 12, 11, 10, 0, 0));  // Set time manually if RTC is not running
    Serial.println("Waktu RTC berhasil disetel.");
  }

  // Set initial state of the gate to be closed
  myservo.write(0); // Initial position of the servo (closed position)
  gateOpen = false; // Gate is initially closed

  // Print initial message that the gate is closed
  Serial.println("Gerbang tertutup");
}

void loop() {
  cm = 0.01723 * readUltrasonicDistance(6, 7);

  // Validasi pembacaan jarak
  if (cm > 0 && cm < 400) {
    if (cm < 30 && !gateOpen) { // Object detected and the gate is closed
      DateTime now = rtc.now();  // Get current time from RTC
      Serial.print("Gerbang terbuka pada: ");
      Serial.print(now.day(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.year(), DEC);  // This will print the current year (e.g., 2024)
      Serial.print(" ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.println(now.second(), DEC);

      // Open the gate (servo moves to 120 degrees)
      for (pos = 0; pos <= 120; pos += 1) {
        myservo.write(pos);
        delay(10); // Speed of servo movement
      }
      gateOpen = true;  // Set the gate as open
      delay(500);
    } 
    else if (cm >= 30 && gateOpen) {  // If object is no longer detected, close the gate
      // Close the gate (servo moves to 0 degrees)
      for (pos = 120; pos >= 0; pos -= 1) {
        myservo.write(pos);
        delay(10);
      }
      gateOpen = false;  // Reset the gate state to closed
      Serial.println("Gerbang tertutup");
      delay(1000);
    }
    else {
      // Gate remains open as long as the object is detected
      delay(500);
    }
  } else {
    Serial.println("Pembacaan sensor tidak valid.");
    delay(500);
  }
}
