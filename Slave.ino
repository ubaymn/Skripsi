#define pinRxTxControl 4        //kontrol RS485 arduino RxTx pada pin PD4
#define pinRxTxControl2 5        //kontrol RS485 arduino RxTx pada pin PD4
#define RS485Tx    HIGH    //Logika High untuk enable Transmit RS485
#define RS485Rx     LOW     //Logika Low untuk enable Receive RS485
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
int statusRusak = '1'; //Menunjukkan status Rusak (1 ->Rusak atau 0 -> Benar)
int dataMasuk; //Banyak data yang masuk
//int pinPIR = 2; //Set pin PD2 sebagai masukan Interrupt eksternal
//int pinPWMLED = 5; //Set pin PD5 sebagai keluaran PWMLED
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
  pinMode(pinPIR, INPUT); //Set pin PD2 sebagai input
  Serial.begin(9600); //inisialisasi serial
  digitalWrite(pinRxTxControl, RS485Tx); //Set Arduino mode Tx
  digitalWrite(pinRxTxControl2, RS485Tx); //Set Arduino mode Tx
  //Serial.print("Start PLJU");
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(pinPIR), PIR, CHANGE); //Setting interrupt pada pin PD2 dengan mode CHANGE, panggil PIR()
  digitalWrite(pinRxTxControl, RS485Rx); //Set Arduino mode Tx 
  digitalWrite(pinRxTxControl2, RS485Rx); //Set Arduino mode Tx
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
        data[1] = byte(AlamatSlave);
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
        data[18] = ' ';
        data[19] = byte(statusLampu); //status nyala atau mati
        data[20] = ' ';
        data[21] = byte(sumber); //sumber
        data[22] = ' ';
        data[23] = byte(statusPIR); //status PIR
        data[24] = ' ';
        data[25] = byte(statusRusak); //status rusak
        //Serial.println(AlamatSlave);
        //Serial.println(I);
        //Serial.println(V);
        //Serial.println(C);
        //Serial.println(data[19]);
        //Serial.println(data[21]);
        //Serial.println(data[23]);
        digitalWrite(pinRxTxControl, RS485Tx);
        digitalWrite(pinRxTxControl2, RS485Tx);
        for(counter=0; counter<sizeof(data)-2; counter++) {
          Serial.print(data[counter]);
        }
        //Serial.print('\n');
        //Serial.write(45);
        
      }
      if(buffer[0] == Broadcast) {
       if(buffer[1] == '1') {
         statusLampu = 1; //lampu Nyala
         analogWrite(pinPWMLED, 40); //lampu redup
         Serial.println("Lampu Redup");
       }
       if(buffer[1] == '0') {
         statusLampu = 0; //lampu Nyala
         analogWrite(pinPWMLED, 0); //lampu mati
         Serial.println("Lampu Mati");
       }
      }
      memset(buffer, 0, sizeof(buffer));   // Clear contents of Buffer
      Serial.flush();
      digitalWrite(pinRxTxControl, RS485Rx);
      digitalWrite(pinRxTxControl2, RS485Rx);
    }
  }

  //else {
    //Hitung dari Sensor Tegangan
    //RawVolt = double (analogRead(pinArus));
    //V = ((RawVolt)/1023.0)*21.4;
    
    //Hitung dari Sensor Arus
    //RawArus = double(analogRead(pinTegangan));
    //V = ((RawArus)/1023.0)*5000;
    //I = ((ACSoffset-V)/mVperAmp);
  //}
  //digitalWrite(pinRxTxControl, RS485Tx);
  //Serial.print("123\n");
  //delay(1000);
  //digitalWrite(pinRxTxControl, RS485Rx);
  //Serial.println(Serial.read());
  //delay(1000);
}
