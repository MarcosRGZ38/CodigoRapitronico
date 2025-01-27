//  4 Channel Receiver | 4 Kanal Alıcı
//  PWM output on pins D2, D3, D4, D5 (Çıkış pinleri)

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

int ch_width_1 = 0;
int ch_width_2 = 0;
int ch_width_3 = 0;
int ch_width_4 = 0;

Servo ch1;
Servo ch2;
Servo ch3;
Servo ch4; // señales PWM ???

int LED = 6;

bool isLedOn = false;

struct Signal {
  byte throttle;      
  byte pitch;
  byte roll;
  byte yaw;
};

Signal data;

#define CE_PIN 7
#define CSN_PIN 8

const uint64_t pipeIn = 0xE9E8F0F0E1LL;
RF24 radio(CE_PIN, CSN_PIN);

void ResetData()
{
// Define the inicial value of each data input. | Veri girişlerinin başlangıç değerleri
// The middle position for Potenciometers. (254/2=127) | Potansiyometreler için orta konum
  data.throttle = 10; // Motor Stop | Motor Kapalı
  data.pitch = 20;  // Center | Merkez
  data.roll = 30;   // Center | Merkez
  data.yaw = 40;   // Center | Merkez

  if (isLedOn == false) {
    digitalWrite(LED, HIGH);
    isLedOn = true;
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); // Ensure the LED starts off

  //Set the pins for each PWM signal | Her bir PWM sinyal için pinler belirleniyor.
  ch1.attach(2);
  ch2.attach(3);
  ch3.attach(4);
  ch4.attach(5);

  //Configure the NRF24 module
  ResetData();

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {
      digitalWrite(LED, HIGH);
      delay(300);
      digitalWrite(LED, LOW);
      delay(300);

    }  // hold in infinite loop
  }

  Serial.println(F("Rapi-Tronic receiver online")); //jeje
  
  radio.openReadingPipe(1,pipeIn);
  
  radio.startListening(); //start the radio comunication for receiver | Alıcı olarak sinyal iletişimi başlatılıyor

  while (!radio.available()) {    //If no signal is stablished, LED blinks
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(100);

    }  // hold in infinite loop


}

unsigned long lastRecvTime = 0;

void recvData()
{
  while ( radio.available() ) {
    radio.read(&data, sizeof(Signal));
    lastRecvTime = millis();   // receive the data | data alınıyor
    //Serial.println("Data");

    if ( isLedOn == true) {
      digitalWrite(LED, LOW);
      isLedOn = false;
      Serial.print("1");
    }
  }
}
void loop()
{

  recvData();
  unsigned long now = millis();
  if ( now - lastRecvTime > 1000 ) {
    ResetData(); // Signal lost.. Reset data | Sinyal kayıpsa data resetleniyor
    Serial.print("Ahhhhhhhhhhh");
  }

  ch_width_1 = map(data.throttle, 0, 255, 1000, 2000);     // pin D2 (PWM signal)
  ch_width_2 = map(data.roll,    0, 255, 1000, 2000);     // pin D3 (PWM signal)
  ch_width_3 = map(data.pitch,     0, 255, 1000, 2000);     // pin D4 (PWM signal)
  ch_width_4 = map(data.yaw,      0, 255, 1000, 2000);     // pin D5 (PWM signal)

  // Write the PWM signal | PWM sinyaller çıkışlara gönderiliyor
  ch1.writeMicroseconds(ch_width_1);
  ch2.writeMicroseconds(ch_width_2);
  ch3.writeMicroseconds(ch_width_3);
  ch4.writeMicroseconds(ch_width_4);



  Serial.print("  CH_1  ");
  Serial.print(data.throttle);

  Serial.print("  CH_2  ");
  Serial.print(data.roll);


  Serial.print("  CH_3  ");
  Serial.print(data.pitch);


  Serial.print("  CH_4  ");
  Serial.print(data.yaw);
  Serial.println();

}
