/*** CanSat 2022-2023 Estación de tierra ***/

/* Programa unificado para el proyecto CanSat 22/23
 * Dispositivo: LilyGo T-Beam v1.1
 * Fecha creación: ??/10/22
 *  Por Alejandro Chacón Pérez
  */

/*        -=- Por hacer -=-
 * - Añadir la tarjeta SD.
 */
 /*       -=- PinOut -=-
 MISO --> 4
 MOSI --> 13	
 CLK ---> 14
 CS ----> 2
 SDA ---> 21
 SCL ---> 22
 */

/*      -=- Changelog -=-
 * - ??/10/22 - Creación del proyecto.
 * - 21/11/22 - Finalizada la función de desencriptación de valores de la cadena. Investigar si se puede hacer sin clase String (Spoiler: Si, pero es tarde).
 * - 28/11/22 - Finalizada (otra vez) la función de desencriptar los valores de la cadena. Añadidos detalles para la pantalla. Solo queda añadir la radio y la SD para la versión 1.0 :D
 * - 24/12/22 - Añadida la primera iteración de la señal de nueva cadena recibida. 
 * - 29/12/22 - Finiquitada la función demostrar potencia y cuando llega una cadena nueva.
 * - 13/01/23 - Corrección de algunos errores con los iconos de la potencia.
 * - 14/01/23 - Añadida funcionalidad GPS. Añadidas pantallas de latitud, longitud, nº de satélites y voltaje de la batería. Añadido reloj en la esquina inferior  derecha. V 1.0 lista. 
 * - 18/04/23 - Eliminada la función de borrar el documento, ahora sigue escribiendo encima para no perder data. Añadida la función de guardar la posición GPS en la SD. 
                Añadida la función de la versión en la pantalla de inicio. v 1.1 lista
 * - 19/04/23 - Versión 1.2. Ajustes a la UI.
 * - 23/04/23 - Versión 1.3. Añadida funcionalidad rudimentaria para obtener la disstancia según la RSSI. Añadidos valores para las 3 antenas, cambiable sólo a través de código. 
 * - 24/04/23 - Versión 1.4. Añadida la función de cambiar de antena desde la propia estación, con la pulsación corta-larga. 
 * - 26/04/23 - Versión 1.5. Añadida el almacenamiento y lectura del nº de antena en la EEPROM, para así no tener que volver a configurarlo cada vez que se resetea. Lo mismo con el InvertDisplay.
 */ 


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LoRa.h>
#include <axp20x.h>
#include <TinyGPS++.h>
#include <SD.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128                                                                                    // Ancho del display.
#define SCREEN_HEIGHT 64                                                                                    // Alto del display.

#define logo_HEIGHT 30                                                                                      //Alto del logo.
#define logo_WIDTH 30                                                                                       //Ancho del logo

const unsigned char logo [] PROGMEM = {                                                                     //Array de bits para el logo.
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x07, 0x80, 0x00, 0x00, 0x1c, 0xe0, 0x00, 0x00, 0x31, 0xf0, 0x00, 0x00, 0xc7, 0x1c, 0x00, 
	0x03, 0x9c, 0x07, 0x00, 0x07, 0x38, 0x11, 0x80, 0x07, 0x38, 0x1c, 0x80, 0x07, 0x38, 0x46, 0x80, 
	0x07, 0x38, 0x73, 0x80, 0x07, 0x38, 0x73, 0x80, 0x07, 0x3e, 0x73, 0x80, 0x07, 0x38, 0xf3, 0x80, 
	0x07, 0x38, 0x73, 0x80, 0x07, 0x38, 0x73, 0x80, 0x05, 0x18, 0x73, 0x80, 0x04, 0xc0, 0x73, 0x80, 
	0x04, 0x60, 0x73, 0x80, 0x03, 0x00, 0x63, 0x00, 0x01, 0xc1, 0xce, 0x00, 0x00, 0x67, 0x38, 0x00, 
	0x00, 0x1c, 0x60, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char signal0 [] PROGMEM = {
	0x00, 0x00, 0x3c, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x0f, 0x24, 0x00, 0x09, 0x24, 0x00, 
	0x09, 0x24, 0x03, 0xc9, 0x24, 0x02, 0x49, 0x24, 0x02, 0x49, 0x24, 0xf2, 0x49, 0x24, 0x92, 0x49, 
	0x24, 0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0xf3, 0xcf, 0x3c
};

const unsigned char signal1 [] PROGMEM = {
	0x00, 0x00, 0x3c, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x0f, 0x24, 0x00, 0x09, 0x24, 0x00, 
	0x09, 0x24, 0x03, 0xc9, 0x24, 0x02, 0x49, 0x24, 0x02, 0x49, 0x24, 0xf2, 0x49, 0x24, 0xf2, 0x49, 
	0x24, 0xf2, 0x49, 0x24, 0xf2, 0x49, 0x24, 0xf2, 0x49, 0x24, 0xf2, 0x49, 0x24, 0xf3, 0xcf, 0x3c
};

const unsigned char signal2 [] PROGMEM = {
	0x00, 0x00, 0x3c, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x0f, 0x24, 0x00, 0x09, 0x24, 0x00, 
	0x09, 0x24, 0x03, 0xc9, 0x24, 0x03, 0xc9, 0x24, 0x03, 0xc9, 0x24, 0xf3, 0xc9, 0x24, 0xf3, 0xc9, 
	0x24, 0xf3, 0xc9, 0x24, 0xf3, 0xc9, 0x24, 0xf3, 0xc9, 0x24, 0xf3, 0xc9, 0x24, 0xf3, 0xcf, 0x3c
};

const unsigned char signal3 [] PROGMEM = {
	0x00, 0x00, 0x3c, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x0f, 0x24, 0x00, 0x0f, 0x24, 0x00, 
	0x0f, 0x24, 0x03, 0xcf, 0x24, 0x03, 0xcf, 0x24, 0x03, 0xcf, 0x24, 0xf3, 0xcf, 0x24, 0xf3, 0xcf, 
	0x24, 0xf3, 0xcf, 0x24, 0xf3, 0xcf, 0x24, 0xf3, 0xcf, 0x24, 0xf3, 0xcf, 0x24, 0xf3, 0xcf, 0x3c
};

const unsigned char signal4 [] PROGMEM = {
	0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x0f, 0x3c, 0x00, 0x0f, 0x3c, 0x00, 
	0x0f, 0x3c, 0x03, 0xcf, 0x3c, 0x03, 0xcf, 0x3c, 0x03, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 
	0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c
};

const unsigned char signal5 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x07, 0x03, 0x80, 0x18, 0x00, 0x60, 0x00, 0x00, 0x00, 0x1f, 0x03, 0xe0, 0x11, 
	0x02, 0x20, 0x11, 0x22, 0x20, 0x11, 0x22, 0x20, 0x1f, 0x13, 0xe0, 0x00, 0x20, 0x00, 0x00, 0x00, 
	0x40, 0x00, 0x00, 0x20, 0x01, 0xfe, 0x20, 0x03, 0x03, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00
};

const unsigned char sd1 [] PROGMEM = {
	0x00, 0x00, 0x0f, 0xf0, 0x1d, 0x50, 0x35, 0x50, 0x75, 0x50, 0x77, 0xf0, 0x3f, 0xf0, 0x3f, 0xf0, 
	0x3f, 0xf0, 0x77, 0x70, 0x7a, 0xf0, 0x7d, 0xf0, 0x7a, 0xf0, 0x77, 0x70, 0x7f, 0xf0, 0x7f, 0xf0, 
	0x7f, 0xf0, 0x00, 0x00
};

const unsigned char sd2 [] PROGMEM = {
	0x00, 0x00, 0x0f, 0xf0, 0x1d, 0x50, 0x35, 0x50, 0x75, 0x50, 0x77, 0xf0, 0x3f, 0xf0, 0x3f, 0xf0, 
	0x3f, 0xf0, 0x7f, 0xb0, 0x7f, 0x30, 0x6e, 0x70, 0x64, 0xf0, 0x71, 0xf0, 0x7b, 0xf0, 0x7f, 0xf0, 
	0x7f, 0xf0, 0x00, 0x00
};




const int boton = 38;                                                                                       //Pin boton.

int leerSerial;                                                                                             //Variable para la lectura del Serial.

bool invertDisplay = false;                                                                                 //Variable para invertir la pantalla.

int screen = 0;                                                                                             //Control de pantallas (Qué pantalla se está mostrando en cada momento.)
int antenna = 3;

String cadenaAntigua = "";
String cadenaRecibida = "";                                                                                 //Cadenas para las cadenas recien recibidas y anterior, para comparar.
String cadaParte ="";
long i = 0;                                                                                                 //Cadenas de control de las funciones de desencriptado.

long frecuency = 915E6;                                                                                     //Frecuencia usada.

bool mostrado = false;
bool cadenaNew = false;                                                                                     //Variables para el control de display de los datos. 
bool saved = true;
bool sdInsertada = false;

int lastRSSI = 999;                                                                                         //Variable que almacena la potencia de la última cadena recibida. 
long tiempo;

long tiempoPrev;
long tiempoPrevSD;
long tiempoPrevBoton;                                                                                       //Variables de control de los milisegundos para los bucles. 
bool botonPulsado = false;
bool botonPulsadoLargo = false;                                                                             //Variables de control de la pulsación de los botones.

File archivo;


float valores[8];                                                                                           //Array tipo float para almacenar todos los valores.

#define OLED_RESET     -1                                                                                   // Pin para resetear la pantalla (-1 porque no tiene).
#define HSPI_SCLK 14
#define HSPI_MISO 4
#define HSPI_MOSI 13
#define HSPI_CS 2
#define EEPROM_SIZE 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
AXP20X_Class axp;
TinyGPSPlus gps;
SPIClass * hspi = NULL;                                                                                            //Objetos de los sensores y actuadores.

void showData(char* text1, float var1, char* unidad1, char* text2, float val2, char* unidad2, bool flecha = true){  //Función showData(), que nos permite ahorrar lineas de código y mostrar los valores más facilmente.
  display.clearDisplay();                                                                                   //Limpiamos la pantalla para que no se solape con lo mostrado antes.
  display.setCursor(15, 1);
  display.setTextSize(1);
  display.println(text1);
  display.setCursor(10, 11);
  display.setTextSize(2);
  if(text1 == "Lat:"){                                                                                      //Si la primera variable a mostrar es latitud, aumentamos el nº de cifras decimales a mostrar.
    display.print(var1, 3);
  }
  else if(text1 == "Antena no:"){
  display.print(var1, 0);    
  }
  else{                                                                                                     //Sino, mostramos una por defecto.
    display.print(var1, 1);
  }
  display.setTextSize(1);
  display.print(unidad1);
  display.setCursor(15, 31);
  display.println(text2);
  display.setTextSize(2);
  display.setCursor(10, 41);
  if(text2 == "Long:"){                                                                                     //Al igual que antes, si la variable es la longitud, mostramos 3 cifras decimales.
    display.print(val2, 3);
  }
  else if(text2 == "Presion:"){                                                                             //Si es la presión, no mostramos decimales por falta de espacio.
    display.print(val2, 0);
  }
  else{
    display.print(val2, 1);                                                                                 //Y sino, por defecto, mostramos 1 cifra decimal.
  }
  display.setTextSize(1);
  display.print(unidad2);
  display.drawLine(95, -1, 95, 65, WHITE);
  display.drawLine(95, 30, 128, 30, WHITE);
  display.drawLine(95, 53, 128, 53, WHITE);
  if((lastRSSI <= 25) && (lastRSSI >= 0)){                                                                 //Los siguientes If's comprueban la potencia de la última cadena, y muestran el icono correspondiente. 
    display.drawBitmap(100, 0, signal4, 22, 16, WHITE);
  }
  else if ((lastRSSI <= 50) && (lastRSSI >=26)){
    display.drawBitmap(100, 0, signal3, 22, 16, WHITE);
  }
  else if ((lastRSSI <= 75) && (lastRSSI >=51)){
    display.drawBitmap(100, 0, signal2, 22, 16, WHITE);
  }
  else if ((lastRSSI <= 100) && (lastRSSI >=76)){
    display.drawBitmap(100, 0, signal1, 22, 16, WHITE);
  }
  else if ((lastRSSI <= 125) && (lastRSSI >=101)){
    display.drawBitmap(100, 0, signal0, 22, 16, WHITE);
  }
  else if(lastRSSI > 125){
    display.drawBitmap(100, 0, signal5, 22, 16, WHITE);
  }
  if(flecha == true){                                                                                      //Si la cadena es nueva, dibujamos una flecha para indicar que acaba de llegar.
    display.drawLine(101, 20, 101, 24, WHITE);
    display.drawLine(100, 23, 102, 23, WHITE);
  }
  if(sdInsertada == true){
    display.drawBitmap(106, 32, sd2, 13, 18, WHITE);
  }
  else{
    display.drawBitmap(106, 32, sd1, 13, 18, WHITE);
  }
  display.setCursor(105, 20);
  display.print(lastRSSI);
  display.setCursor(97, 55);
  display.print(gps.time.hour());
  display.print(":");
  display.println(gps.time.minute());
  display.drawCircle(2, 12, 2, WHITE);
  display.drawCircle(2, 20, 2, WHITE);
  display.drawCircle(2, 28, 2, WHITE);
  display.drawCircle(2, 36, 2, WHITE);
  display.drawCircle(2, 44, 2, WHITE);
  display.drawCircle(2, 52, 2, WHITE);
  switch (screen){
    case 0:
      display.fillCircle(2, 12, 2, WHITE);
    break;

    case 1:
      display.fillCircle(2, 20, 2, WHITE);
    break;

    case 2:
      display.fillCircle(2, 28, 2, WHITE);
    break;

    case 3:
      display.fillCircle(2, 36, 2, WHITE);
    break;
    
    case 4:
      display.fillCircle(2, 44, 2, WHITE);
    break;
    case 5:
      display.fillCircle(2, 52, 2, WHITE);
  }
  display.display();                                                                                        //Maraña de funciones de la OLED.
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------


void setup() {
  delay(600);
  Serial.begin(9600);
  Serial.println(R"(  
                      - Programa CanSat Tierra 2022-2023 -

 |-| Software para la estación de tierra del equipo Hexact para la edicion 2022-2023.
 |-| Vamos Luisma vamos!
 |-| Por Alejandro Chacon Perez :D
 |-----------------------------------------------------------------------------------|)");
  Wire.begin(21,22);
  EEPROM.begin(EEPROM_SIZE);
  Serial1.begin(9600, SERIAL_8N1, 34, 12);                                                                   //Inicializamos la conexión serial con el GPS. 
  if(axp.begin(Wire, AXP192_SLAVE_ADDRESS) == AXP_FAIL) {
    Serial.println(" |x| Error al inicializar el chip de carga AXP192");                                    //Inicializamos sensores + posibles mensajes de error. 
  }
  else{
    Serial.println(" |o| Chip de carga AXP192 iniciado con exito.");
  }
  if(display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(" |o| Display OLED SSD1306 iniciado con exito.");
  }
  else{
    Serial.println(" |x| Error al iniciar el display OLED SSD1306.");
  }
  pinMode(boton, INPUT);
  LoRa.setPins(18, 23, 26);
  if (LoRa.begin(frecuency)) {
    Serial.println(" |o| Modulo LoRa inciado con exito.");
  }
  else{
    Serial.println(" |x| Error al iniciar el modulo LoRa.");
  }
  hspi = new SPIClass(HSPI);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_CS);
  pinMode(HSPI_CS, OUTPUT);
  if(SD.begin(HSPI_CS,*hspi)){
    Serial.println(" |o| Tarjeta SD iniciado con exito.");
    sdInsertada = true;
  }
  else{
    Serial.println(" |x| Error al iniciar la tarjeta SD");
    sdInsertada = false;
  }
  Serial.println(R"( |-----------------------------------------------------------------------------------|
 |H| Envía una 'h' o 'H' para recibir ayuda y una lista de comandos.
 |-----------------------------------------------------------------------------------|)");

  invertDisplay = EEPROM.read(1);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.invertDisplay(invertDisplay);
  display.drawBitmap(90, 5, logo, 30, 30, WHITE);
  display.setTextSize(2);
  display.setCursor(0,20);
  display.println("HEXACT");
  display.drawLine(0, 37, 100, 37, WHITE);
  display.setCursor(5, 40);
  display.setTextSize(1);
  display.cp437(true);
  display.print("Estaci");
  display.write(162);
  display.print("n de tierra");
  display.setCursor(100, 56);
  display.print("V1.6");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();                                                                                        //Mensaje inicial + icono.


  archivo = SD.open("/data.csv", FILE_WRITE);                                                               //Abrimos el archivo en la SD y comprobamos si ya tenía datos escritos.
  archivo.seek(archivo.size());
  if(archivo.size() > 10){
    Serial.println(" |!| La tarjeta SD tiene datos anteriores a este encendido.");
    archivo.print("unnumerocomotextoxdxd");
    archivo.print(';');
    archivo.print("0");
    archivo.print(';');
    archivo.print("0");
    archivo.print(';');
    archivo.print("0");
    archivo.print(';');
    archivo.print("0");
    archivo.print(';');
    archivo.print("0");
    archivo.print(';');
    archivo.print("0");
    archivo.print(';');
    archivo.print("0");
    archivo.print(';');
    archivo.print("0");
    archivo.print(';');
    archivo.print("0");
    archivo.print(';');
    archivo.print('\n');    
    archivo.close();
  }
  else{
    archivo.print("Cadena nº");
    archivo.print(';');
    archivo.print("Ángulo en x");
    archivo.print(';');
    archivo.print("Ángulo en y");
    archivo.print(';');
    archivo.print("Aceleración");
    archivo.print(';');
    archivo.print("Temperatura");
    archivo.print(';');
    archivo.print("Presión");
    archivo.print(';');
    archivo.print("Altura");
    archivo.print(';');
    archivo.print("RSSI");
    archivo.print(';');
    archivo.print("Latitud");
    archivo.print(';');
    archivo.println("Longitud");
    archivo.close();
  }
  antenna = EEPROM.read(0);                                                                                 //Leemos el valor de la antena guardado en la EEPROM para saber cual estaba seleccionada desde antes.
  
} 


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void loop() {
  tiempo = millis();                                                                                        //Actualizamos los milisegundos pasados desde el encendido.
  if((tiempo >= tiempoPrev + 750) && (cadenaNew == true)){
    cadenaNew = false;                                                                                      //Si ha pasado 750 milisegundos desde que se recibió la última cadena, ya no mostramos la flechita.
  }
  while(Serial.available() > 0){                                                                            //Escuchamos el serial por si llega algo.
    leerSerial = Serial.read();
    if((leerSerial == 'h') || (leerSerial == 'H')){                                                         //Si es una 'h' o 'H', mostramos un mensaje de ayuda. 
      Serial.print(R"( |H| PinOut:
 |   MISO --> 4
 |   MOSI --> 13	
 |   CLK ---> 14
 |   CS ----> 2
 |   SDA ---> 21
 |   SCL ---> 22
 |----------------------------------------------------------------|
 |i| Frecuencia actual = )");
      Serial.println(frecuency);
      Serial.println(R"( |----------------------------------------------------------------|
 |H| Otros comandos:
 |   'i' o 'I' -> Invierte el color de la pantalla (Blanco/Negro).
 |   'h' o 'H' -> Comando de ayuda (Este). 
 |----------------------------------------------------------------|)");
    }
    else if(leerSerial == 'i' || leerSerial == 'I'){                                                        //Si es una 'i' o 'I', alterna el color de la pantalla (Modo claro/oscuro).
      invertDisplay = !invertDisplay;
      EEPROM.write(1, invertDisplay);
      EEPROM.commit();
      display.invertDisplay(invertDisplay);
    }
  }
  descifrarCadena();                                                                                        //Llama a la función de descifrar la cadena recibida. 
  valores[7] = distancia(lastRSSI);
  if(saved == false){
    archivo = SD.open("/data.csv", FILE_WRITE);
    if(archivo){
      Serial.println(" |.| Escribiendo en la SD...");
      archivo.seek(archivo.size());
      archivo.print(valores[0]);
      archivo.print(';');
      archivo.print(valores[1]);
      archivo.print(';');
      archivo.print(valores[2]);
      archivo.print(';');
      archivo.print(valores[3]);
      archivo.print(';');
      archivo.print(valores[4]);
      archivo.print(';');
      archivo.print(valores[5]);
      archivo.print(';');
      archivo.print(valores[6]);
      archivo.print(';');
      archivo.print(valores[7]);
      archivo.print(';');
      archivo.print(gps.location.lat(), 4);
      archivo.print(';');
      archivo.print(gps.location.lng(), 4);
      archivo.print('\n');
      Serial.print(" |?| Tamaño del archivo: ");
      Serial.println(archivo.position());
      archivo.close();
      Serial.println(" |o| Datos guardados en la tarjeta SD.");
    }
  saved = true;
  }

  gps.encode(Serial1.read());                                                                               //Lee los datos recibidos del gps. 
  if(digitalRead(boton) == LOW){                                                                            //Si lee el boton, cambia de pantalla. (El botón esta pull-up).
    if(botonPulsado == false){
      botonPulsado = true;
      tiempoPrevBoton = tiempo;
    }
    if((tiempo - tiempoPrevBoton > 1250) && (botonPulsadoLargo == false) && (screen == 5)){
      botonPulsadoLargo = true;
      antenna = antenna + 1;
      if(antenna > 3){
        antenna = 1;
      }
      EEPROM.write(0, antenna);
      EEPROM.commit();
      Serial.print(" |a| Antena seleccionada: ");
      Serial.println(antenna);
      botonPulsado = false;
    }
  }
  else{
     if(botonPulsado == true){
      if(botonPulsadoLargo == true){
        botonPulsadoLargo = false;
      }
      else{
        screen = screen + 1;
        if(screen > 5){
          screen = 0;
        }
        Serial.print(" |o| Pantalla: ");
        Serial.println(screen);
        delay(200);
      }
      botonPulsado = false; 
    }
  }

  switch (screen){                                                                                          //Switch de las pantallas, llama a la funcione showData() con distintos argumentos cada vez.
    case 0:
      showData("Angulo en X:", valores[1], "o",  "Angulo en Y:", valores[2], "o", cadenaNew);
      break;
    case 1:
      showData("Temp:", valores[4], "o", "Altura:", valores[6], "m", cadenaNew);
      break;
    case 2:
      showData("Accel:", valores[3], "m/s2",  "Presion:", valores[5], "Pa", cadenaNew);
      break;
    case 3:
      showData("Lat:", gps.location.lat(), "o", "Long:",gps.location.lng(), "o", cadenaNew);
      break;
    case 4:
      showData("Satelites:",gps.satellites.value(),"","Voltaje:",axp.getBattVoltage(),"mV",cadenaNew);
      break;
    case 5:
      showData("Antena no:", antenna, "", "Distancia", valores[7], "m", cadenaNew);      
      break;
  }
  delay(10);
  
}

String parte(String data, char separator, int index){                                                       //Funcion para desglosar las cadenas:
  int found = 0;                                                                                            //Nº de "trozos" encontrados.
  int strIndex[] = {0, -1};                                                                                 //"Coordenadas" del último trozo encontrado.
  int maxIndex = data.length()-1;                                                                           //Máx. caracteres que tiene la cadena.

  for(int i=0; i<=maxIndex && found<=index; i++){                                                           //Bucle for, se repite por cada caracter de la cadena o mientras el nº de trozos encontrados no sea mayor que el que le especificamos.
	if(data.charAt(i) == separator || i == maxIndex){                                                         //Si el caracter es un separador o se ha alcanzado el máximo de caracteres...
    	found++;                                                                                              //Es que hemos encontrado un trozo gordo, así que añadimos 1 al contador de trozos.
    	strIndex[0] = strIndex[1]+1;                                                                          //La primera coordenada es 0.
    	strIndex[1] = (i == maxIndex) ? i+1 : i;                                                              //???
	  }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";                                       //Devuelve una sub-cadena con el último trozo encontrado.
}

void descifrarCadena(){
  int packetSize = LoRa.parsePacket();
    if (packetSize) {
      Serial.print(" |>| Cadena recibida: ");
      cadenaRecibida = "";
      while (LoRa.available()) {
        cadenaRecibida = cadenaRecibida + (char)LoRa.read();
      }
      mostrado = false;
      Serial.println(cadenaRecibida);
      lastRSSI = abs(LoRa.packetRssi());
      Serial.print(" |>| Potencia de la cadena: ");
      Serial.println(lastRSSI);
      cadenaNew = true;
      saved = false;
      tiempoPrev = tiempo;

      display.display();
    }
    cadaParte = parte(parte(cadenaRecibida,'$',1),'#',0);                                                   // Elimina el caracter final e inicial.
    if ((cadaParte.length()>0) || (cadenaRecibida != cadenaAntigua)){                                       //Si sigue habiendo algo (La cadena no estaba vacía), 
 	    cadaParte = cadaParte.substring(0,cadaParte.length()-1);                                              // Quita último caracter.
 	    i = 0;
 	    while((parte(cadaParte,'?',i).length()>0) && (mostrado == false)) {                                   //Mientras 
        //Serial.println(parte(cadaParte,'?',i)+ " --> "+(String) i);
        valores[i] = parte(cadaParte,'?',i).toFloat();                                                      //Guarda los datos en las variables.
        i++;
 	    }
       mostrado = true;
    }
    cadaParte="";                                                                                           // Limpia la variable.
    cadenaAntigua = cadenaRecibida;                                                                         //Actualiza la cadena antigua.
}

float distancia(int potencia){
  float d;
  switch (antenna){
    case 1:
      d = (-1841.3272 / (120.5466 - potencia));
      break;
    case 2:
      d = (-1253.373 / (109.4044 - potencia));
      break;
    case 3:
      d = (-1467.1059 / (121.5731 - potencia));
      break;
  }
  return(d);
}





