int sensorInterrupt = 0;  // interrupt 0
int sensorPin       = 2; //Digital Pin 2
int solenoidValve = 5; // Digital pin 5
unsigned int SetPoint = 400; //400 milileter
 
/*The hall-effect flow sensor outputs pulses per second per litre/minute of flow.*/
float calibrationFactor = 90; //You can change according to your datasheet
 
volatile byte pulseCount =0;  //hitungan awal volatilitas
 
float flowRate = 0.0; //rata-rata
unsigned int flowMilliLitres =0;
unsigned long totalMilliLitres = 0;
 
unsigned long oldTime = 0;
 
void setup()
{
 
  // inisiasi output pin
  Serial.begin(9600);
  pinMode(solenoidValve , OUTPUT);
  digitalWrite(solenoidValve, HIGH);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
 
  /*The Hall-effect sensor is connected to pin 2 which uses interrupt 0. Configured to trigger on a FALLING state change (transition from HIGH
  (state to LOW state)*/
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING); //you can use Rising or Falling
}
 
void loop() //menggunakan sistem pengulangan
{
 
   if((millis() - oldTime) > 1000)    // Counter proses dihitung per 1 detik
  { 
    // Disable the interrupt while calculating flow rate and sending the value to the host
    detachInterrupt(sensorInterrupt);
 
    // pengkalibrasian sensor per 1 detik
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
 
    oldTime = millis();
 
    flowMilliLitres = (flowRate / 60) * 1000; //pengisian brapa liter per detiknya
 
    // mengkalkulasi total hasil keluaran (liter)
    totalMilliLitres += flowMilliLitres;
 
    unsigned int frac;
 
    // program untuk menampilkan FlowRate atau air yg keluar ada brapa liter per detiknya
    Serial.print("Flow rate: ");
    Serial.print(flowMilliLitres, DEC);  // Print the integer part of the variable
    Serial.print("mL/Second");
    Serial.print("\t");           
 
    // Program untuk menampilkan kuantitas output perdetik
    Serial.print("Output Liquid Quantity: ");        
    Serial.print(totalMilliLitres,DEC);
    Serial.println("mL"); 
    Serial.print("\t");     

        //program pengkondisian, jika totalmililiter lebih dari 40 makan akan mengaktifkan sistem solinoidvalve
    if (totalMilliLitres > 40)
    {
      SetSolinoidValve();
    }
    
// untuk mereset counter
    pulseCount = 0;
 
    // Program yg menginterupsi kalau kerja program hampir selesai
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}
 
//Insterrupt Service Routine
 
void pulseCounter()
{
  // melakukan pulse counter (pengulangan) sesuai dari kuantitas botol yg akan diberikan
  pulseCount++;
}
 
void SetSolinoidValve()
{
  digitalWrite(solenoidValve, LOW);
}
