#define kapasitasGalon      19//liter
#define pulsaPerLiter       450//sesuai spesifikasi sensor water flow
 
#define pinWaterFlow        2
#define pinSensorIR         A0
#define pinPompa            7
#define pinSelenoid         6
 
#define IRAktif             LOW
#define relayAktif          LOW
 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <util/atomic.h>
 
LiquidCrystal_I2C lcd(0x3F, 16, 2);
volatile uint16_t pulseCount;
 
void setup() {
  digitalWrite(pinPompa, !relayAktif);
  digitalWrite(pinSelenoid, !relayAktif);
  pinMode(pinWaterFlow, INPUT);
  pinMode(pinSensorIR, INPUT);
  pinMode(pinPompa, OUTPUT);
  pinMode(pinSelenoid, OUTPUT);
 
  Serial.begin(9600);
  Serial.println(F("Pengisi galon otomatis"));
  Serial.println(F("https://www.semesin.com/project"));
 
 
  Wire.begin();
  Wire.beginTransmission(0x3F);
  if (Wire.endTransmission())
  {
    lcd = LiquidCrystal_I2C(0x27, 16, 2);
  }
  lcd.begin();
 
  lcd.backlight();
  lcd.print("Pengisi galon");
  lcd.setCursor(0, 1);
  lcd.print("Semesin.com");
  delay(3000);
 
 
  lcd.setCursor(0, 0);
  lcd.print("ambil Galon     ");
  while (digitalRead(pinSensorIR) == IRAktif); //deteksi awal : jika ada galon kosongkan dahulu
 
  attachInterrupt(digitalPinToInterrupt(pinWaterFlow), pulseCounter, FALLING);
}
 
void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Letakkan Galon  ");
  while (digitalRead(pinSensorIR) == !IRAktif);
 
  delay(1000);//memastikan galon diletakkan
 
  if (digitalRead(pinSensorIR) == IRAktif)
  {
    lcd.setCursor(0, 0);
    lcd.print("Mengisi Galon...");
 
    pulseCount = 0;
    digitalWrite(pinPompa, relayAktif);
    delay(1000);
    digitalWrite(pinSelenoid, relayAktif);
 
    lcd.setCursor(0, 1);
    lcd.print("                ");
    uint16_t jumlahPulsa;
 
    while ((jumlahPulsa < kapasitasGalon * pulsaPerLiter) && (digitalRead(pinSensorIR) == IRAktif))
    {
      ATOMIC_BLOCK(ATOMIC_FORCEON)
      {
        jumlahPulsa = pulseCount;
      }
 
      lcd.setCursor(0, 1);
      lcd.print((1.0 * jumlahPulsa / pulsaPerLiter));
      lcd.print(" liter");
    }
 
    digitalWrite(pinSelenoid, !relayAktif);
    delay(100);
    digitalWrite(pinPompa, !relayAktif);
 
    lcd.setCursor(0, 0);
    if (jumlahPulsa >= kapasitasGalon * pulsaPerLiter)
    {
      lcd.print("Galon penuh     ");
    }
    else if (digitalRead(pinSensorIR) != IRAktif)
    {
      lcd.print("Galon tidak ada ");
    }
 
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("silahkan ambil  ");
 
    while (digitalRead(pinSensorIR) == IRAktif);
    delay(1000);//memastikan galon sudah diambil
    while (digitalRead(pinSensorIR) == IRAktif);
  }
}
 
void pulseCounter()
{
  pulseCount++;
}
