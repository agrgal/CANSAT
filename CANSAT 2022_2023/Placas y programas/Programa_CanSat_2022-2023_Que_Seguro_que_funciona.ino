/*** - Programa CanSat 2022/2023 - ***/

/* Programa unificado para el proyecto CanSat 22/23
 * Dispositivo: ESP32 v4
 * Fecha creación: 23/10/22
 *  Por Alejandro Chacón Pérez
  */

/*        -=- Por hacer -=-
 * - Comprobar que envio/recibo de radio funciona.
 * - Añadir funcionalidad del stepper para las semillas (Dependiente de la altura, con protección de retorno) 
 */

/*      -=- Changelog -=-
 * - 23/10/22 - Creación del proyecto
 * - 27/10/22 - Cambios en el programa: He quitado por ahora la librería del  LoRa y el GPS para no liarme y probar bien el GY-87.
                Añadida la funcionalidad del GY-87.
   - 14/11/22 - Cambios para que funcione con la ESP32.
   - 15/11/22 - Comienzo a añadir las funciones del LoRa 
   - 17/11/22 - Añadida la función de concatenar los valores en una sola cadena. Queda probar el módulo LoRa
                con la ESP32 y la base estaría, aparte del buzzer opcional y el stepper de las semillas.  
 */ 


#include "Adafruit_BMP085.h"
#include "MPU6050.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "LoRa.h"
#include "SPI.h"



Adafruit_BMP085 bmp;
MPU6050 sensor;

float temperatura = 99.99;
unsigned int presion = 999999;
float p0;
float altura = 999999.9;
float alturaPrev = 999999.9;
float ace[3] = {+39.2, -39.2, +39.2};
float ang[3] = {-90, +90, -45.5};
short int ax, ay, az;
short int gx, gy, gz;                                                                                     // Valores sin procesar del acelarómetro y giroscopio.                                                                                                  // Valor de la presión inicial. Se toma al encender el CANSAT.
long tiempo_prev;
float dt;                                                                                                 // Variables que calculan el tiempo (Cálculos aceleromtro/giroscopio)
float ang_x, ang_y, ang_z;
float ang_x_prev, ang_y_prev, ang_z_prev;
char* mensaje = (R"(  - Programa CanSat 2022-2023 -
Software para el satelite del equipo Hexact para la edicion 2022-2023.

Vamos Luisma vamos!

Por Alejandro Chacon Perez :D

-----------------------------------------------------------------------

 )");



void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println(mensaje);
  LoRa.setPins(18, 23, 26);                      //ss, reset, dio0
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    //while (1);
  }
  LoRa.begin(915E6);
  Wire.begin(32,33);
  sensor.initialize();
  if(sensor.testConnection()){                                                                            //Iniciamos sensores + posibles mensajes de error.
    Serial.println("Sensor MPU6050 iniciado correctamente");
  }
  else{
    Serial.println("Error al iniciar el sensor MPU6050(giroscopio)");
  }
  if(!bmp.begin()){
    Serial.print("Error al iniciar el sensor BMP180(Temperatura + presion)");
  }
  else{
    Serial.println("Sensor BMP180 iniciado correctamente");
  }
  p0 = bmp.readPressure();
  Serial.print("Presioncilla inicial = ");
  Serial.println(p0);
  delay(500);
}

void leerGY87(){
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);
  dt = (millis()-tiempo_prev)/1000.0;
  tiempo_prev = millis();                                                                                   //Calculamos el tiempo pasado entre cada medición. 
  float ang_x_accel = atan(ax/sqrt(pow(ay,2) + pow(az,2)))*(180.0/3.14);
  float ang_y_accel = atan(ay/sqrt(pow(az,2) + pow(ax,2)))*(180.0/3.14);
  float ang_z_accel = atan(az/sqrt(pow(ax,2) + pow(ay,2)))*(180.0/3.14);
  ang_x = 0.98*(ang_x_prev+(gx/131)*dt) + 0.02*ang_x_accel;
  ang_y = 0.98*(ang_y_prev+(gy/131)*dt) + 0.02*ang_y_accel;
  ang_z = 0.98*(ang_z_prev+(gy/131)*dt) + 0.02*ang_z_accel;                                                 //Calculamos la rotación usando tanto el girsocopio como el acelerómetro para eliminar errores.
  ang_x_prev = ang_x;
  ang_y_prev = ang_y;
  ang_z_prev = ang_z;
  ace[2] = az * (9.81/16384.0);
  ace[1] = ay * (9.81/16384.0);
  ace[0] = ax * (9.81/16384.0);                                                                             //Calculamos la aceleración en m/s2 en los tres ejes 
  ang[0] = ang_x_accel;  
  ang[1] = ang_y_accel;
  ang[2] = ang_z_accel;
  altura = bmp.readAltitude(p0);
  presion = bmp.readPressure();
  temperatura = bmp.readTemperature();
  //float m_az_ax = sqrt(pow(sqrt((pow(ax_ms2, 2)) + (pow(az_ms2, 2))), 2) + (pow(ay_ms2, 2)));             //Calculamos el módulo de los 3 vectores, para así obtener la aceleración que experimenta.--> NO SE USA POR AHORA

}

void printData(){
  //Serial.print("Rotacion en X: ");
  Serial.print(ang[0]);
  Serial.print(",");
  //Serial.print("\tRotacion en Y: ");
  Serial.print(ang[1]);
  Serial.print(",");
  //Serial.print("o \tAceleracion de caida(m/s2): ");
  Serial.print(ace[2]);  
  Serial.print(",");
  //Serial.print("\tTemperatura = ");
  Serial.print(temperatura);
  Serial.print(",");
  //Serial.print("C\tPresion = ");
  Serial.print(presion);
  Serial.print(",");
  //Serial.print("\tAltitud respecto al suelo = ");
  Serial.print(altura);
  Serial.println(",");
  //Serial.println("m");
}

void crearCadena(){
  char cadena [44] = "";
  snprintf(cadena, sizeof(cadena), "$%.2f?%.2f?%.2f?%.2f?%d?%.2f", ang[0], ang[1], ace[2], temperatura, presion, altura);  
  Serial.println(cadena);
  //LoRa.beginPacket();
  //LoRa.print(cadena);
  //LoRa.endPacket();
}

void loop() {
  leerGY87();  
  //printData();
  crearCadena();
  delay(1000);
}



