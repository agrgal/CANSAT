/*** - Programa CanSat 2022/2023 - ***/

/* Programa unificado para el proyecto CanSat 22/23
 * Dispositivo: ESP32 v4
 * Fecha creación: 23/10/22
 * Por Alejandro Chacón Pérez.
  */

/*        -=- Por hacer -=-
 * - Nada :D
 */

/*      -=- Changelog -=-
 * - 23/10/22 - Creación del proyecto
 * - 27/10/22 - Cambios en el programa: He quitado por ahora la librería del  LoRa y el GPS para no liarme y probar bien el GY-87.
 *              Añadida la funcionalidad del GY-87.
 * - 14/11/22 - Cambios para que funcione con la ESP32.
 * - 15/11/22 - Comienzo a añadir las funciones del LoRa 
 * - 17/11/22 - Añadida la función de concatenar los valores en una sola cadena. Queda probar el módulo LoRa
 *              con la ESP32 y la base estaría, aparte del buzzer opcional y el stepper de las semillas.
 * - 05/12/22 - Después del incidente del sensor, comienzo las pruebas del LoRa.
 * - 15/12/22 - Añadida la función del servo y limitados los delays en el loop principal. Ahora los mensajes del Serial van acompañados con un indicador visual.
 *              Ahora se envía el número de cadena para mantener mayor control.
 * - 19/12/22 - Prueba del módulo LoRa. En prinicipio no da error, aunque el servo ha parado de funcionar.
 * - 22/12/22 - EL problema del servo se ha arreglado poniendole la fuente de alimentación externa, con el adaptador para corriente. Añadidas las primeras funcionalidades de comunicación serial
 *              (Help y PinTest).
 * - 13/01/23 - Tras varios días con fallos con el módulo LoRa definitivo, ya funciona correctamente. La clave estaba en unos cortos en la antena y en inicializar tanto la placa y el sensor a la vez.
 *              Se debe usar la misma fuente para alimentar a ambos.
 * - 14/01/23 - Ajuste de comentarios y borrado código innecesario. Ahora la aceleración la saca haciendo el vector resultante de las 3 componentes, y no está limitado a una sola dirección.
 * - 16/03/23 - Versión 1.1: Añadida la función de mostrar la versión del programa. 
 */ 

/*      -=- PinOut -=-
 * Servo →  4
 * MISO →   19  (Naranja))
 * MOSI →   23	(Amarillo)
 * SCK →    18  (Gris)
 * NSS →    5   (Morrarro)
 * RST →    26  (Azul)
 * DIO0 →   25  (Blanco)
 * SDA →    32  
 * SCL →    33  
*/

#include "Adafruit_BMP085.h"
#include "MPU6050.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "LoRa.h"
#include "SPI.h"
#include "ESP32_Servo.h"

#define LOOPTIME 5000
#define SERVOTIME 5000
#define SERVOTIME2 6000
#define version "1.3"                                                                                       //Usamos #define para ahorrar variables innecesarias pero seguir podiendo cambiar los valores facilmente a la hora de editar.


Adafruit_BMP085 bmp;
MPU6050 sensor;
Servo servo1;                                                                                               //Objetos de los dos sensores y el servo.

long frecuency = 915E6;                                                                                     //Frecuencia del LoRa.
float temperatura = 99.99;
unsigned int presion = 999999;
float p0;                                                                                                   //Presión inicial.
float altura = 0;
float alturaPrev = 0;
int cayendo;
float ace[3] = {+39.2, -39.2, +39.2};
float aceTotal;
float ang[3] = {-90, +90, -45.5};                                                                           //Variables para almacenar los valores de la aceleración y los ángulos.
short int ax, ay, az;
short int gx, gy, gz;                                                                                       // Valores sin procesar del acelarómetro y giroscopio.                                                                                                  // Valor de la presión inicial. Se toma al encender el CANSAT.
long tiempo_prev;
float dt;                                                                                                   // Variables que calculan el tiempo (Cálculos aceleromtro/giroscopio)
float ang_x, ang_y, ang_z;
float ang_x_prev, ang_y_prev, ang_z_prev;                                                                   //Variables para los cálculos del girsocopio.

long tiempo = 0;                                                                                            //Variable que controla los milisegundos pasados desde el encendido (Bucles).
long tiempoPrevServo;
bool extendido = false;                                                                                     //Variables de control del servo.
long tiempoPrevBucle;                                                                                       //Variable para el timer del bucle.

long ncadena = 0;                                                                                           //Variable del nº de cadena.

int leerSerial;                                                                                             //Variable para la lectura del puerto Serial.

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  delay(4000);
  Serial.print(R"(  
                  - Programa CanSat 2022-2023 -

 |-| Software para el satelite del equipo Hexact para la edicion 2022-2023.
 |-| Vamos Luisma vamos!
 |-| Por Alejandro Chacon Perez :D
 |-| Version: )");
 Serial.println(version);
 Serial.println(" |-------------------------------------------------------------------------|");
  LoRa.setPins(5, 26, 25);                                                                                  //Pines para ss, reset y dio0 (ESP32 V1).
  //LoRa.setPins(18, 23, 26);                                                                               //Pines para ss, reset y dio0 (LILIGO T-Beam).
  if (LoRa.begin(frecuency)) {
    Serial.println(" |o| Modulo LoRa inciado correctamente.");
  }
  else{
    Serial.println(" |x| Error al iniciar el modulo LoRa.");
  }
  Wire.begin(32,33);
  sensor.initialize();
  if(sensor.testConnection()){                                                                              //Iniciamos sensores + posibles mensajes de error.
    Serial.println(" |o| Sensor MPU6050 iniciado correctamente (giroscopio).");
  }
  else{
    Serial.println(" |x| Error al iniciar el sensor MPU6050 (giroscopio).");
  }
  if(bmp.begin()){
    Serial.println(" |o| Sensor BMP180 iniciado correctamente (Temperatura + presion).");
  }
  else{
    Serial.println(" |x| Error al iniciar el sensor BMP180 (Temperatura + presion).");
  }
  servo1.attach(4);                                                                                         //Pin servo.
  servo1.write(170);
  p0 = bmp.readPressure();                                                                                  //Lectura de la presión inicial para calcular la altura relativa.
  Serial.print(" |i| Presioncilla inicial = ");
  Serial.println(p0);
  Serial.println(R"( |-------------------------------------------------------------------------|
 |H| Envia una 'h' o 'H' para recibir ayuda y una lista de comandos.
 |-------------------------------------------------------------------------|)");
  delay(500);
}

void leerGY87(){
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);                                                                        //Leemos los valores RAW de aceleración y angulos.
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
  alturaPrev = altura;
  altura = bmp.readAltitude(p0);
  Serial.println(altura);
  Serial.println(alturaPrev);
  presion = bmp.readPressure();
  temperatura = bmp.readTemperature();                                                                      //Leemos el resto de datos necesarios.
  //float m_az_ax = sqrt(pow(sqrt((pow(ax_ms2, 2)) + (pow(az_ms2, 2))), 2) + (pow(ay_ms2, 2)));             //Calculamos el módulo de los 3 vectores, para así obtener la aceleración que experimenta → NO SE USA POR AHORA.
  aceTotal = sqrt((pow(ace[0], 2)) + (pow(ace[1], 2)) + (pow(ace[2], 2)));
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
  char cadena [48] = "";
  snprintf(cadena, sizeof(cadena), "$%ld?%.2f?%.2f?%.2f?%.2f?%d?%.2f#", ncadena, ang[0], ang[1], aceTotal, temperatura, presion, altura); //Formateamos los valreoes a enviar en 1 sola cadena.
  //Cadena max = $9999?-99.99?-99.99?-99.99?-99.99?999999?-999.9#
  Serial.print(" |>| Cadena para enviar: ");  
  Serial.println(cadena);
  ncadena = ncadena + 1;                                                                                    //Actualizamos el contador de cadenas enviadas. 
  LoRa.beginPacket();
  LoRa.print(cadena);
  LoRa.endPacket();                                                                                         //Enviamos el paquete y cerramos transmisión.
}

void loop() {
  while(Serial.available() > 0){                                                                            //Escuchamos el Serial por si recibimos algún comando.
    leerSerial = Serial.read();
    if((leerSerial == 'p') || (leerSerial == 'P')){                                                         //Si es una P, ejecutamos un testeo de sensores.
      Serial.println(R"( |-----------------------------------------------------------------|
 |P| Prueba de conexion. )");
      if(sensor.testConnection()){
        Serial.println(" |o| Sensor MPU6050 conectado correctamente (giroscopio).");
      }
      else{
        Serial.println(" |x| Error al conectar el sensor MPU6050 (giroscopio).");
      }
      if(bmp.begin()){
        Serial.println(" |o| Sensor BMP180 conectado correctamente (Temperatura + presion).");
      }
      else{
        Serial.println(" |x| Error al conectar el sensor BMP180 (Temperatura + presion).");
      }
      Serial.println(" |-----------------------------------------------------------------|");
    }
    else if ((leerSerial == 'h') || (leerSerial == 'H')){                                                   //Si es una H, enviamos el pinout, la frecuencia, y otros comandos. 
      Serial.print(R"( |--------------------------------------------|
 |H| PinOut:
 |   Servo -> 4
 |   MISO --> 19
 |   MOSI --> 23	
 |   SCK ---> 18
 |   NSS ---> 5
 |   RST ---> 26
 |   DIO0 --> 25
 |   SDA ---> 21
 |   SCL ---> 22
 |--------------------------------------------|
 |i| Frecuencia actual = )");
      Serial.println(frecuency);
      Serial.println(" |--------------------------------------------|");
      Serial.println(R"( |H| Otros comandos:
 |   's' o 'S' -> Alterna el estado del servo.
 |   'p' o 'P' -> Prueba de sensores.
 |   'h' o 'H' -> Comando de ayuda (Este).
 |--------------------------------------------|)");
    }
    else if ((leerSerial == 's') ||  (leerSerial == 'S')){                                                  //Si es una s, alterna el estado del servo (Extendido <-> Retraido).
      switch (extendido){
        case true:
          servo1.write(170);
          Serial.println(" |!| Servo en retraccion.");
          break;
        case false:
          servo1.write(55);
          Serial.println(" |!| Servo en extension.");
          break;
      }
      extendido = !extendido;
    }
  }
  tiempo = millis();                                                                                        //Leemos los milisegundos pasados para controlar los bucles. 
  if((alturaPrev >= (altura + 1)) && (tiempo >= tiempoPrevServo + SERVOTIME)){
    cayendo = cayendo + 1;
    Serial.println(cayendo);
  }                                    
  if((tiempo >= tiempoPrevServo+ SERVOTIME) && (extendido == false) && (cayendo >= 2)){   //Extendemos el servo si ha pasado el tiempo establecido...
    Serial.println(" |!| Servo en extension.");
    servo1.write(55);
    extendido = true;
  }

  if((tiempo  >= tiempoPrevServo + SERVOTIME2) && (cayendo >= 2)){                                                              //...y lo retraemos si ha pasado el 2º tiempo.
    Serial.println(" |!| Servo en retraccion.");
    servo1.write(170);
    tiempoPrevServo = tiempo;
    extendido = false;
  }

  if(tiempo >= tiempoPrevBucle + LOOPTIME){                                                                 //Función de bucle para evitar las interrupciones del delay.
    leerGY87();  
    //printData();
    crearCadena();
    tiempoPrevBucle = tiempo;
  }
}



