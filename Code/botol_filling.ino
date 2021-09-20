#include<LiquidCrystal.h> //library LCD
#include <EEPROM.h> //library eeprom

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); //pin LCD

#define echopin A4 // echo pin
#define trigpin A5 // Trigger pin

//int cont = 0;

int in1 = A0;  // tombol 1
int in2 = A1;  // tombol 2
int in3 = A2;  // tombol 3
int in4 = A3;  // tombol 4

int remotePins [] = {A0, A1, A2, A3};

//int set1 = 0;
int stop0 = 0; //kondisi stop0=0 konveyor mati, stop0=1 konveyor hidup
int stop1; //kondisi stop1=0 pompa mati, stop1=1 pompa hidup
int mode1 = 0; //nilai awal mode1 adalah 0

int led = 13;  // lampu pengisian

//int timer = 0;

int pump = 8;  // pompa air
int motor = 9; // motor konveyor

int ir_start = 10; // sensor 1
int ir_stop = 12;  // sensor 2
//int ir_fill = 11;  // tidak di pakai


int dist; //nilai data sensor ultrasonic dalam %
long duration; //nilai data sensor ultrasonic dalam cm

int val1 = 0, val2 = 0, val3 = 0, val4 = 1; //variabel nyimpan angka timer
long result = 0;

void setup() {
  for (int i = 0; i < 5; i ++) {//untuk nilai i lebih besar dari 0 dan lebih kecil dari 5 = 1,2,3,4
    pinMode (remotePins [i], INPUT); //mengatur pin remotePins(1,2,3,4) sebagai input
    digitalWrite(remotePins [i], HIGH); //mengatur kondisi pin remotePins(1,2,3,4) bernilai high
  }

  //pinMode(ir_fill, INPUT);
  pinMode(ir_start, INPUT);//mengatur sensor 1 sebagai input
  pinMode(ir_stop, INPUT);//mengatur sensor 2 sebagai input

  pinMode (trigpin, OUTPUT);//mengatur pin trig sebagai output
  pinMode (echopin, INPUT);//mengatur pin echo sebagai input

  pinMode(motor, OUTPUT);//mengatur pin motor sebagai output
  pinMode(pump, OUTPUT);//mengatur pin pompa sebagai output

  pinMode(led, OUTPUT);//mengatur pin led sebagai output

  lcd.begin(16, 2);//inisiasi LCD dengan ukuran 16x2
  lcd.setCursor(0, 0); lcd.print("   WELCOME  To  ");//menampilkan "   WELCOME  To  " dengan koordinat awal LCD x=0, y=0
  lcd.setCursor(0, 1); lcd.print("  Auto Filling");//menampilkan "  Auto Filling" dengan koordinat awal LCD x=0, y=1
  delay(2000);//menampilkan karakter selama 2 detik

  Serial.begin(9600);//memulai koneksi serial dengan baud rate 9600
  //Write();

  Read();//melakukan kerja fungsi Read()
  //analogWrite(motor, 100);

  for (int i = 0; i < 5; i ++) {//untuk nilai i lebih besar dari 0 dan lebih kecil dari 5 = 1,2,3,4
    dist = data1();// nilai dist sama dengan nilai pada fungsi data1(), fungsi data1()=membaca nilai pengukuran sensor ultrasonic
    data2();//melakukan kerja fungsi data2()=melakukan penskalaan pembacaan ultrasonic
    delay(100);
  }
  lcd.clear();//menghapus layar LCD
}

void loop()
{
  if (stop0 == 0) //jika kondisi stop0 sama dengan 0 (konveyor mati), maka
  {
    if (digitalRead (in1) == 0) //jika tombol 1 bernilai nol(tombol ditekan)
    {
      lcd.clear();//menghapus LCD
      mode1 = mode1 + 1; //nilai mode1 = mode1 + 1
      if (mode1 > 4) // jika nilai mode1 lebih dari 4, maka
      {
        mode1 = 0; //nilai mode1 kembali ke 0
        Write(); //melakukan fungsi Write()=menulis data ke eeprom
        Read(); //melakukan fungsi Read()=membaca data dari eeprom
        lcd.setCursor(0, 0); //menentukan posisi awal LCD x=0, y=0
        lcd.print("  Ok Saved Data "); //menampilkan karakter pada LCD dengan referensi posisi awal
        delay(1000); //menampilkan data selama 1 detik
      }
      delay(300); //waktu delay 0,3 detik
    }

    if (digitalRead (in2) == 0) { //jika tombol 2 bernilai nol(tombol ditekan)
      if (mode1 == 1) { // jika nilai mode1=1, maka
        val1 = val1 + 1; // nilai val1 = val1+1 (bertambah 1)
      }
      if (mode1 == 2) {// jika nilai mode1=2, maka
        val2 = val2 + 1;// nilai val2 = val2+1 (bertambah 1)
      }
      if (mode1 == 3) { // jika nilai mode1=3, maka
        val3 = val3 + 1;// nilai val3 = val3+1 (bertambah 1)
      }
      if (mode1 == 4) {// jika nilai mode1=4, maka
        val4 = val4 + 1;// nilai val4 = val4+1 (bertambah 1)
      }
      if (val1 > 9) {//jika nilai val1 lebih dari 9, maka
        val1 = 0; //nilai val1 kembali ke 0
      }
      if (val2 > 9) { //jika nilai val2 lebih dari 9, maka
        val2 = 0; //nilai val2 kembali ke 0
      }
      if (val3 > 9) { //jika nilai val3 lebih dari 9, maka
        val3 = 0; //nilai val3 kembali ke 0
      }
      if (val4 > 9) {//jika nilai val4 lebih dari 9, maka
        val4 = 0; //nilai val4 kembali ke 0
      }
      delay(300); //tunda selama 0,3 detik
    }
  }

  if (mode1 == 0) { //jika mode1 = 0
    if (digitalRead (in3) == 0) { //dan jika tombol 3 = 0 (tombol ditekan), maka
      stop0 = 0; // kondisi stop0 = 0 (sistem berhenti)
    }
    if (digitalRead (in4) == 0) { //dan jika tombol 4 = 0 (tombol ditekan), maka
      stop0 = 1; //kondisi stop0 = 1 (sistem berjalan)
    }

    ///lcd.setCursor(0,0);lcd.print("   WELCOME  To  ");
    //lcd.setCursor(0,1);lcd.print("     A W F S    ");
    //delay(1000);

    lcd.setCursor(0, 0); //menentukan posisi awal LCD x=0, y=0
    lcd.print("Tank Level="); //menampilkan karakter "Tank Level=" pada LCD dengan referensi posisi awal
    lcd.print(dist); //menampilkan karakter dari dist
    lcd.print("%   "); //menampilkan karakter dari "%"

    lcd.setCursor(0, 1);//menentukan posisi awal LCD x=0, y=1
    lcd.print("Fill Time =");//menampilkan karakter "Fill Time =" pada LCD dengan referensi posisi awal
    lcd.print(result);//menampilkan karakter dari result
    lcd.print("   ");//menampilkan karakter dari "   "
  }
  else //atau jika nilai if (mode1 = 0) tidak terpenuhi, maka
  {
    lcd.setCursor(0, 0);//menentukan posisi awal LCD x=0, y=0
    lcd.print("Set Time= "); // menampilkan karakter "Set Time= " pada LCD dengan referensi posisi awal
    lcd.print(val4);//menampilkan karakter dari val4
    lcd.print(val3);//menampilkan karakter dari val3
    lcd.print(val2);//menampilkan karakter dari val2
    lcd.print(val1);//menampilkan karakter dari val1

    if (mode1 == 4) { //jika mode1 sama dengan 4, maka
      lcd.setCursor(10, 1); //menentukan posisi awal LCD x=10, y=1
      lcd.print("-"); //menampilkan karakter dari "-"
    }
    if (mode1 == 3) { //jika mode1 sama dengan 3, maka
      lcd.setCursor(11, 1); //menentukan posisi awal LCD x=11, y=1
      lcd.print("-"); //menampilkan karakter dari "-"
    }
    if (mode1 == 2) { //jika mode1 sama dengan 2, maka
      lcd.setCursor(12, 1); //menentukan posisi awal LCD x=12, y=1
      lcd.print("-"); //menampilkan karakter dari "-"
    }
    if (mode1 == 1) { //jika mode1 sama dengan 1, maka
      lcd.setCursor(13, 1); //menentukan posisi awal LCD x=13, y=1
      lcd.print("-");//menampilkan karakter dari "-"
    }
  }

  if (stop0 == 1) { // jika kondisi stop0=1 (sistem bekerja), maka
    if (digitalRead (ir_stop) == 1) { //dan jika nilai dari sensor 2 = 1 (tidak mendeteksi objek), maka
      analogWrite(motor, 170); //menggerakkan motor konveyor sebesar 170
      if (digitalRead (ir_start) == 0) { //jika sensor ir_start = 0 (mendeteksi objek),
        if (stop1 == 0) { //dan jika konsdisi pompa mati, maka
          stop1 = 1; // nilai stop1 menjadi 1 (kondisi pompa menyala)
          analogWrite(motor, 0); // motor berhenti
          delay(300);//tunda 0,3 detik
          digitalWrite(pump, HIGH);//pompa menyala
          digitalWrite(led, HIGH);//led menyala
          delay(result);// menunda sesuai dengan nilai result
          digitalWrite(pump, LOW);//pompa mati
          digitalWrite(led, LOW);//led mati
          delay(100);//tunda 0,1 detik
          analogWrite(motor, 170);//motor konveyor bergerak sebesar 170
          delay(1000);//tunda 1 detik
          if (digitalRead (ir_start) == 0) //jika sensor1 = 0 (mendeteksi objek), maka
            analogWrite(motor, 170);//motor konveyor bergerak sebesar 170
        }
        if (digitalRead (ir_start) == 0)//jika sensor1 =0 (mendeteksi objek) , maka
        {
          stop1 = 0; // kondisi stop1 = 0 (kondisi pompa berhenti)
        }
      }
    }

    else {//atau jika nilai dari sensor 2 tidak sama dengan 1 (mendeteksi objek), maka
      analogWrite(motor, 0);//motor berhenti
      delay(100);//tunda 0,1 detik
    }
  }
  else {//atau jika stop0 tidak sama dengan 1, maka
    analogWrite(motor, 0);//motor berhenti
  }
}
void Read() {
  val1 = EEPROM.read(11); val2 = EEPROM.read(12); val3 = EEPROM.read(13); val4 = EEPROM.read(14);//membaca nilai eeprom dengan alamat adress tertentu
  result = val4 * 1000 + val3 * 100 + val2 * 10 + val1; //menyimpan nilai dai val1,2,3,4 dengan variabel result
  Serial.println(result);//menampilkan nilai result pada serial monitor
}

void Write() {
  EEPROM.write(11, val1); EEPROM.write(12, val2); EEPROM.write(13, val3); EEPROM.write(14, val4);
  //menulis data val1,2,3,4 pada eeprom
}


void data2() {
  dist = map(dist, 2, 30, 100, 0);//melakukan penskalaan nilai dist
  if (dist > 100) {//jika nilai dist lebih besar dari 100, maka
    dist = 100;//nilai dist sama dengan 100
  }
  if (dist < 0) {//jika nilai dist lebih kecil dari 0, maka
    dist = 0;//nilai dist sama dengan 0
  }
}

long data1() {// membaca nilai sensor ultrasonic
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  duration = pulseIn (echopin, HIGH);
  return duration / 29 / 2; //mengembalikan nilai pembacaan sensor 
}
