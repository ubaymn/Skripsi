double RawVolt = 0;
const int analogVolt = A1;
double Vteg = 0;
int relay = 6;
int pwm = 9;
int pir_pin = 2; // PIR pin
int val = 0;
const int analogIn = A0;
double mVperAmp = 185;
double RawValue = 0;
double ACSoffset = 2619;
double V = 0;
double I = 0;


void setup() {
  // put your setup code here, to run once:
//analogReference(EXTERNAL);

pinMode(relay, OUTPUT);    
pinMode(pwm, OUTPUT);
pinMode(pir_pin, INPUT);
Serial.begin(9600);
}


void loop() {
  // put your main code here, to run repeatedly:
val = digitalRead(pir_pin);
RawVolt = double (analogRead(analogVolt));
Vteg = (((RawVolt)/1023.0)*0.38)*25;
RawValue = double(analogRead(analogIn));
V = ((RawValue)/1023)*5000;
I = ((ACSoffset-V)/mVperAmp);

if (val>0){
  digitalWrite(relay,HIGH);
  analogWrite(pwm,255);
}
else{
  //digitalWrite(relay,LOW);
  analogWrite(pwm,0);
}
Serial.print(" RW = ");
Serial.print(RawVolt);
Serial.print(" V = ");
Serial.print(Vteg,3);
Serial.print(" RWArus = ");
Serial.print(RawValue);
Serial.print(" I = ");
Serial.print(I,3);
Serial.print(" PIR = ");
Serial.print(val,3);
Serial.print(" \n");
}
