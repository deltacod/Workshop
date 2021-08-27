#include <Servo.h>
#include <HCSR04.h>

HCSR04 hc(5,6);    //超音波感測器腳位(trig, echo)
Servo myservo;
int pinLED = 13;   //裝一顆LED燈防行動電源自動斷電
int pinServo = 9;    //伺服馬達腳位

void setup() {
  Serial.begin(9600);
  
  myservo.attach(pinServo);
  myservo.write(0);
  
  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, 1);
}

void loop() {
  int distance = hc.dist();
  if (distance>10&&distance<=20) {
    delay(1000);   //一秒後重新偵測一次距離，要兩次都在範圍內才噴酒精。
    
    distance = hc.dist();
    if (distance>10&&distance<=20) {
      int j=1;   //省酒精，只噴一次。
      for (int i=0;i<j;i++) {
        myservo.write(90);
        delay(1200);
        myservo.write(0);
        delay(1200);
      }
      delay(3000);      
    }
  }
}
