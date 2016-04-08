#include <Arduino.h>
#line 1
#line 1 "C:\\Users\\asus\\Desktop\\Arduino\\Serial\\Serial.ino"
#define pinRxTxControl 4        //kontrol RS485 arduino RxTx pada pin PD4
#define pinRxTxControl2 5        //kontrol RS485 arduino RxTx pada pin PD4
#define RS485Tx    HIGH    //Logika High untuk enable Transmit RS485
#define RS485Rx     LOW     //Logika Low untuk enable Receive RS485
char buffer[30];
int dataMasuk;
#line 7 "C:\\Users\\asus\\Desktop\\Arduino\\Serial\\Serial.ino"
void setup();
#line 16 "C:\\Users\\asus\\Desktop\\Arduino\\Serial\\Serial.ino"
void loop();
#line 7
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(pinRxTxControl, OUTPUT); //Set pin PD4 sebagai output
pinMode(pinRxTxControl2, OUTPUT); //Set pin PD5 sebagai output
digitalWrite(pinRxTxControl, RS485Rx); //Set Arduino mode Tx 
digitalWrite(pinRxTxControl2, RS485Rx); //Set Arduino mode Tx
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print ("1@\n");
//Serial.print(Serial.read());
delay(1000);
//if(Serial.available()>0) {
//    Serial.print(Serial.read());
//    dataMasuk = Serial.readBytesUntil('\n', buffer, sizeof(buffer));  
//    Serial.print (buffer);
//}
}

