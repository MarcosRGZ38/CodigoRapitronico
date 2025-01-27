#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include<wire.h>

#define pantalla true  //poner true si esta conectadad
                       // poner false si no 

#if pantalla == true
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #define SCREEN_i2c_ADDR 0x3D
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define OLED_RST_PIN 3
  Adafruit_SSD1306 display(128, 64, &wire, OLED_RST_PIN)
#endif 

#define Enviar 1
#define Recibir 2

const uint32_t clave = 0xE54F0F0E2;
RF24 radio(Enviar, Recibir);

struct Senial {
  uint8_t direccion;
  uint8_t aceleracion;
}

Senial paquete;

void reinicializar(){
  paquete.direccion = 127;
  paquete.aceleracion = 127;
}

int mapPotenciometerValues(int val, int lower, int upper, int middle, bool reverse){
  val = constraint(val, lower, upper);
  if (val < middle)
    val = map(val, lower, middle, 0 , 128);
  else 
    val = map(val, middle, upper, 0, 128);
  return (reverse ? 255 - val : val);
}

void setup(){
  #if pantalla == true
    // Codigo de la pantalla oled
    wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_i2c_ADDR);
    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Welcome");
    display.display();

    delay(100);
  #endif
  Serial.begin(14400);

  if(!radio.begin()) {
    Serial.println(F("La radio no funciona adecuadamente"));
    while(1){}
  }

  radio.openWritingPipe(clave);
  radio.stopListening();
  ResetData();
}

void loop(){
  #if pantalla == true
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0,0);
    display.println("Direccion");
    display.println(paquete.direccion);
    display.println("Aceleracion");
    display.println(paquete.aceleracion);

    display.display();
    delay(500);
  #endif

  paquete.direccion = map(analogRead(A0), 0, 1024, 0, 255);
  paquete.aceleracion = map(analogRead(A1), 0, 1024, 0, 255);
  radio.write(&paquete, sizeof(Senial));


  Serial.println(F("Canal 1"));
  Serial.print(paquete.direccion);
  Serial.println("Canal 2");
  Serial.print(paquete.aceleracion);

}