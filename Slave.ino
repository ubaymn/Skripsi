#define pinRxTxControl 4        //kontrol RS485 arduino RxTx pada pin PD4
#define pinRxTxControl2 5        //kontrol RS485 arduino RxTx pada pin PD4
#define pinRelay 8 //kontrol Relay aktif / nonaktif pada PD8
#define RS485Tx    HIGH    //Logika High untuk enable Transmit RS485
#define RS485Rx     LOW     //Logika Low untuk enable Receive RS485
#define SumberPLN HIGH     //Logika High untuk membuat sumber menjadi PLN
#define SumberBaterai LOW   //Logika Low untuk membuat sumber menjadi Baterai
#define pinPIR 2 //Set pin PD2 sebagai masukan Interrupt eksternal
#define pinPWMLED 6 //Set pin PD6 sebagai keluaran PWMLED

const int bSize = 30;
const int pinArus = A0; //Set pin A0 sebagai masukan Arus
const int pinTegangan = A1; //Set pin A1 sebagai masukan Tegangan
const int pinTeganganBat = A2; //Set pin A1 sebagai masukan Tegangan Baterai
const int AlamatSlave = '2'; //Set alamat Slave
const int Broadcast = '?'; //Set karakter penanda broadcast
char buffer[bSize];
char data[30];
char arus[5];
char tegangan[5];
char kapasitas[5];
int statusLampu = '0'; //Menunjukkan status Lampu (1 ->Nyala atau 0 -> Mati)
int sumber = '0'; //menunjukkan asal sumber daya (1 ->PLN atau 0 -> baterai)
int statusPIR = '1'; //Menunjukan status PIR (1 -> Aktif atau 0 -> NonAktif)
int statusRusak  ; //Menunjukkan status Rusak (0 ->Rusak Baterai dan Lampu, 1 -> Rusak Baterai, 2 -> Rusak Lampu, 3 -> Benar)
int kondisiBaterai; //Menunjukkan status Rusak Baterai (1 -> Benar atau 0 -> Rusak)
int kondisiLampu; //Menunjukkan status Rusak Baterai (1 -> Benar atau 0 -> Rusak)
int dataMasuk; //Banyak data yang masuk
int counter; //Counter
//inisialisasi Parameter Sensor Arus
double mVperAmp = 185; //didapat dari spesifikasi ACS712
double RawArus;
double ACSoffset = 2502; //Saat tak ada arus seharusnya tegangan bernilai 2500
double VArus; //Tegangan dibaca sensor
double I; //Arus bacaan dari sensor
double C; //Kapasitas Baterai
//inisialisasi Parameter Sensor Tegangan
double RawVolt;
double RawVoltBat;
double V;
double VBat;
volatile int val;

void setup() {
  pinMode(pinRxTxControl, OUTPUT); //Set pin PD4 sebagai output
  pinMode(pinRxTxControl2, OUTPUT); //Set pin PD5 sebagai output
  pinMode(pinPWMLED, OUTPUT); //Set pin PD6 sebagai output
  pinMode(pinRelay, OUTPUT); //Set pin PD7 sebagai output
  pinMode(pinPIR, INPUT); //Set pin PD2 sebagai input  
  pinMode(pinArus, INPUT);
  pinMode(pinTegangan, INPUT);
  pinMode(pinTeganganBat, INPUT);
  Serial.begin(9600); //inisialisasi serial
  digitalWrite(pinRxTxControl, RS485Tx); //Set Arduino mode Tx
  digitalWrite(pinRxTxControl2, RS485Tx); //Set Arduino mode Tx
  //Serial.print("Start PLJU");
  //attachInterrupt(digitalPinToInterrupt(pinPIR), PIR, CHANGE); //Setting interrupt pada pin PD2 dengan mode CHANGE, panggil PIR()
  attachInterrupt(0, PIR, CHANGE);
  digitalWrite(pinPIR, LOW);
  digitalWrite(pinRxTxControl, RS485Rx); //Set Arduino mode Rx 
  digitalWrite(pinRxTxControl2, RS485Rx); //Set Arduino mode Rx
  //digitalWrite(pinRelay, SumberBaterai);
  digitalWrite(pinRelay, SumberPLN);
}

void PIR() {
  val = digitalRead(pinPIR);
  if(val == HIGH) {
    analogWrite(pinPWMLED, 255); //lampu terang
    statusPIR = '1';
    Serial.print ("masuk interrupt terang\n");
    delay(100);
  }
  else {
    analogWrite(pinPWMLED, 40); //lampu redup
    statusPIR = '0';
    Serial.print ("masuk interrupt redup\n");
    delay(100);
  }
}
void loop() {
  digitalWrite(pinRxTxControl, RS485Rx); //Set Arduino mode Rx 
  digitalWrite(pinRxTxControl2, RS485Rx); //Set Arduino mode Rx  
  if(Serial.available()>0) {
    dataMasuk = Serial.readBytesUntil('\n', buffer, sizeof(buffer));  
    Serial.print (buffer);
    if (dataMasuk>0) {
      Serial.println(buffer);
      delay(1000);
      if(buffer[0] == AlamatSlave && buffer[1] == '@') {
        //Update data Alamat, Arus, Tegangan, Kapasitas, StatusRusak, Sumber, PIR
        //Serial.print("diterima 1@");
        data[0] = ' ';
        data[1] = byte(AlamatSlave); //data yang pertama akan dibaca oleh Pi adalah data[1]
        data[2] = ' ';
        dtostrf(I, 5, 2, arus);
        for (counter=0;counter<5;counter++) {
          data[counter+3]=arus[counter];
        }
        data[7] = ' ';
        dtostrf(V, 5, 2, tegangan);
        for (counter=0;counter<5;counter++) {
          data[counter+8]=tegangan[counter];
        }
        data[12] = ' ';
        dtostrf(C, 5, 2, kapasitas);
        for (counter=0;counter<5;counter++) {
          data[counter+13]=kapasitas[counter];
        }
        data[17] = ' ';
        data[18] = byte(statusLampu); //status nyala atau mati
        data[19] = ' ';
        data[20] = byte(sumber); //sumber
        data[21] = ' ';
        data[22] = byte(statusPIR); //status PIR
        data[23] = ' ';
        data[24] = byte(statusRusak); //status rusak
        data[25] = ' ';
        digitalWrite(pinRxTxControl, RS485Tx);
        digitalWrite(pinRxTxControl2, RS485Tx);
        for(counter=1; counter<sizeof(data); counter++) {
          Serial.print(data[counter]);
        }
      }
      if(buffer[0] == Broadcast) {
       if(buffer[1] == '1') {
         statusLampu = 1; //lampu Nyala
         analogWrite(pinPWMLED, 1); //lampu redup
         Serial.write("Lampu Redup");
       }
       if(buffer[1] == '0') {
         statusLampu = 0; //lampu mati
         analogWrite(pinPWMLED, 0); //lampu mati
         Serial.write("Lampu Mati");
       }
      }
      memset(buffer, 0, sizeof(buffer));   // Clear contents of Buffer
      Serial.flush();
      digitalWrite(pinRxTxControl, RS485Rx);
      digitalWrite(pinRxTxControl2, RS485Rx);
    }
  }

  else {
    //Hitung dari Sensor Tegangan Baterai
    RawVoltBat = double (analogRead(pinTeganganBat));
    VBat = ((RawVoltBat)/1024.0)*16.35;
    //Kapasitas Baterai
    C = (VBat/9.5)*100;
    
    //Hitung rata2 dari Sensor Tegangan
    for(counter=0;counter<150;counter++)
    {
     RawVolt += double (analogRead(pinTegangan));
    }
    RawVolt = RawVolt/150;
    V = ((RawVolt)/1024.0)*24.5;
    
    //Hitung rata2 dari Sensor Arus
    for(counter=0;counter<150;counter++)
    {
     RawArus += double (analogRead(pinArus));
    }
    RawArus = RawArus/150;
    VArus = ((RawArus)/1024.0)*5000;
    I = ((ACSoffset-VArus)/mVperAmp);
    if (I == 0) {
      kondisiLampu=0;
    }
    else{
      kondisiLampu=1;
    }
    if (C<90) { //Kapasitas Baerai dibawah 10%
      digitalWrite(pinRelay, SumberPLN);
      sumber = '1'; //Sumber PLN
      if (C==0){
        kondisiBaterai=0;
      }
      else{
        kondisiBaterai=1;
      }
    }
    else{
      digitalWrite(pinRelay, SumberBaterai);
      sumber = '0'; //Sumber Baterai
      kondisiBaterai=1;  
    }
    if (kondisiLampu==0){
      if (kondisiBaterai == 0){
        statusRusak='0';
      }
      else if (kondisiBaterai == 1){
        statusRusak='1';
      }
    }
    else if (kondisiLampu == 1){
      if (kondisiBaterai == 0){
        statusRusak='2';
      }
      else if (kondisiBaterai == 1){
        statusRusak='3';
      }
    }
  }
  Serial.print("I:");
  Serial.print(I);
  Serial.print(" V:");
  Serial.print(V);
  Serial.print(" C:");
  Serial.print(C);
  Serial.print(" Sumber:");
  Serial.print(sumber);
  Serial.print(" Status PIR:");
  Serial.print(statusPIR);
  Serial.print("\n");
}
