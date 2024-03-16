//More information at: https://www.aeq-web.com

#include <SparkFun_Ublox_Arduino_Library.h>

#define SERIAL1_RX 34 // GPS_TX -> 34
#define SERIAL1_TX 12 // 12 -> GPS_RX

SFE_UBLOX_GPS NEO6GPS;

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Serial is open now");
  Serial1.begin(9600, SERIAL_8N1, SERIAL1_RX, SERIAL1_TX);
  delay(300);

  do {
    if (NEO6GPS.begin(Serial1)) {
      Serial.println("GPS connection ok");
      NEO6GPS.setUART1Output(COM_TYPE_NMEA);
      NEO6GPS.saveConfiguration();
      Serial.println("Enable NMEA");
      NEO6GPS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_UART1);
      NEO6GPS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_UART1);
      NEO6GPS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_UART1);
      NEO6GPS.disableNMEAMessage(UBX_NMEA_VTG, COM_PORT_UART1);
      NEO6GPS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_UART1);
      NEO6GPS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_UART1);
      NEO6GPS.saveConfiguration();
      Serial.println("NMEA-GGA for AEQ-WEB GPS-Tracker enabled!");
      break;
    }
    delay(1000);
  } while(1);
}

void loop()
{
  if (Serial1.available()) {
    Serial.write(Serial1.read());
  }
} 