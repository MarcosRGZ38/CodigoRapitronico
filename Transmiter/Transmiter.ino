// 4 Channel Transmitter | 4 Kanal Verici

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

const uint64_t pipeOut = 0xE9E8F0F0E1LL;   //IMPORTANT: The same as in the receiver 0xE9E8F0F0E1LL | Bu adres alıcı ile aynı olmalı
RF24 radio(CE_PIN, CSN_PIN); // select CE,CSN pin | CE ve CSN pinlerin seçimi

struct Signal {
  byte throttle;
  byte pitch;
  byte roll;
  byte yaw;
};

Signal data;

void ResetData() 
{
  data.throttle = 127; // Motor Stop (254/2=127)| Motor Kapalı (Signal lost position | sinyal kesildiğindeki pozisyon)
  data.pitch = 127; // Center | Merkez (Signal lost position | sinyal kesildiğindeki pozisyon)
  data.roll = 127; // Center | merkez (Signal lost position | sinyal kesildiğindeki pozisyon)
  data.yaw = 127; // Center | merkez (Signal lost position | sinyal kesildiğindeki pozisyon)
}

void setup()
{
//Start everything up
  Serial.begin(9600);

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1){} // hold in infinite loop -> reconnect 
  }
  radio.openWritingPipe(pipeOut);
  radio.stopListening(); //start the radio comunication for Transmitter | Verici olarak sinyal iletişimi başlatılıyor
  ResetData();
}

// Joystick center and its borders | Joystick merkez ve sınırları

int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
  val = map(val, lower, middle, 0, 128);
  else
  val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}

void loop()
{
// Control Stick Calibration | Kumanda Kol Kalibrasyonları
// Setting may be required for the correct values of the control levers. | Kolların doğru değerleri için ayar gerekebilir.

  data.throttle = map( analogRead(A1), 0, 1024, 0, 255 );  // potenciometro de velocidades
  data.roll = map( analogRead(A2), 0, 1024, 0, 255 );      // "true" or "false" for servo direction | "true" veya "false" servo yönünü belirler
  data.pitch = map( analogRead(A3), 0, 1024, 0, 255 );     // "true" or "false" for servo direction | "true" veya "false" servo yönünü belirler
  data.yaw = map( analogRead(A4), 0, 1024, 0, 255 );       // "true" or "false" for servo direction | "true" veya "false" servo yönünü belirler

  radio.write(&data, sizeof(Signal));

  Serial.print("  CH_1  ");
  Serial.print(data.throttle);

  Serial.print("  CH_2  ");
  Serial.print(data.pitch);


  Serial.print("  CH_3  ");
  Serial.print(data.roll);


  Serial.print("  CH_4  ");
  Serial.print(data.yaw);
  Serial.println();
}
