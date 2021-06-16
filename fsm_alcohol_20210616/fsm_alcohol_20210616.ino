#include <Servo.h>
#include <HCSR04.h>

HCSR04 hc(5,6);
Servo myservo;

void setup() {
  Serial.begin(9600);
  myservo.attach(9);
  myservo.write(0);
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
}

void loop() {
  int distance = hc.dist();
  Serial.println(distance);
  int pos = 0;
  if (distance>10&&distance<=20) {
    for (int i=0;i<3;i++) {
      myservo.write(90);
      delay(1000);
      myservo.write(0);
      delay(1000);
    }
    delay(3000);
  }
}
