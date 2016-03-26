#define pinRxTxControl 4        //kontrol RS485 arduino RxTx pada pin PD4
#define pinRxTxControl2 5        //kontrol RS485 arduino RxTx pada pin PD4
#define pinRelay 6 //kontrol Relay aktif / nonaktif pada PD6
#define RS485Tx    HIGH    //Logika High untuk enable Transmit RS485
#define RS485Rx     LOW     //Logika Low untuk enable Receive RS485
#define SumberPLN HIGH     //Logika High untuk membuat sumber menjadi PLN
#define SumberBaterai LOW   //Logika Low untuk membuat sumber menjadi Baterai
#define pinPIR 2 //Set pin PD2 sebagai masukan Interrupt eksternal
#define pinPWMLED 5 //Set pin PD5 sebagai keluaran PWMLED

const int bSize = 30;
const int pinArus = A0; //Set pin A0 sebagai masukan Arus
const int pinTegangan = A1; //Set pin A1 sebagai masukan Tegangan
const int AlamatSlave = '1'; //Set alamat Slave
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
double RawArus = 0;
double ACSoffset = 2500; //Saat tak ada arus seharusnya tegangan bernilai 2500
double VArus = 0; //Tegangan dibaca sensor
double I = 12.3; //Arus bacaan dari sensor
double C = 40.7; //Kapasitas Baterai
//inisialisasi Parameter Sensor Tegangan
double RawVolt = 0;
double V = 10.2;

void setup() {
  pinMode(pinRxTxControl, OUTPUT); //Set pin PD4 sebagai output
  pinMode(pinPWMLED, OUTPUT); //Set pin PD5 sebagai output
  pinMode(pinRelay, OUTPUT); //Set pin PD2 sebagai input
  pinMode(pinPIR, INPUT); //Set pin PD2 sebagai input  
  Serial.begin(9600); //inisialisasi serial
  digitalWrite(pinRxTxControl, RS485Tx); //Set Arduino mode Tx
  digitalWrite(pinRxTxControl2, RS485Tx); //Set Arduino mode Tx
  //Serial.print("Start PLJU");
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(pinPIR), PIR, CHANGE); //Setting interrupt pada pin PD2 dengan mode CHANGE, panggil PIR()
  digitalWrite(pinRxTxControl, RS485Rx); //Set Arduino mode Tx 
  digitalWrite(pinRxTxControl2, RS485Rx); //Set Arduino mode Tx
  digitalWrite(pinRelay, SumberBaterai);
}

void PIR() {
  if(pinPIR == HIGH) {
    analogWrite(pinPWMLED, 255); //lampu terang
    statusPIR = '1';
  }
  else {
    analogWrite(pinPWMLED, 40); //lampu redup
    statusPIR = '0';
  }
}

void loop() {
  if(Serial.available()>0) {
    dataMasuk = Serial.readBytesUntil('\n', buffer, sizeof(buffer));  
    if (dataMasuk>0) {
      //Serial.println(buffer);
      //delay(1000);
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
        //Serial.println(AlamatSlave);
        //Serial.println(I);
        //Serial.println(V);
        //Serial.println(C);
        //Serial.println(data[19]);
        //Serial.println(data[21]);
        //Serial.println(data[23]);
        digitalWrite(pinRxTxControl, RS485Tx);
        digitalWrite(pinRxTxControl2, RS485Tx);
        for(counter=1; counter<sizeof(data); counter++) {
          Serial.print(data[counter]);
        }
      }
      if(buffer[0] == Broadcast) {
       if(buffer[1] == '1') {
         statusLampu = 1; //lampu Nyala
         analogWrite(pinPWMLED, 40); //lampu redup
         Serial.write("Lampu Redup");
       }
       if(buffer[1] == '0') {
         statusLampu = 0; //lampu Nyala
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
    //Hitung dari Sensor Tegangan
    RawVolt = double (analogRead(pinTegangan));
    V = ((RawVolt)/1023.0)*21.4;
    
    //Hitung dari Sensor Arus
    RawArus = double(analogRead(pinArus));
    VArus = ((RawArus)/1023.0)*5000;
    I = ((ACSoffset-VArus)/mVperAmp);
    if (I == 0) {
      kondisiLampu=0;
    }
    else{
      kondisiLampu=1;
    }
    if (C<10) { //Kapasitas Baerai dibawah 10%
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
}
