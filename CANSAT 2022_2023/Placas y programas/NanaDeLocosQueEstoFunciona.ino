/*** - Programa de prueba GPS T-Beam mío - ***/

const int boton = 38;

#define SERIAL1_RX 34 // GPS_TX -> 34
#define SERIAL1_TX 12 // 12 -> GPS_RX

void setup() {
  pinMode(boton, INPUT);
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, SERIAL1_RX, SERIAL1_TX);
  delay(300);
  Serial.println("hello");
}

void loop() {
  if(Serial1.available()){
    Serial.write(Serial1.read());
  }
}
