#include "DHT.h"
#include <Wire.h>
#include <SD.h>
#include "RTClib.h"

#define DHTPIN 3     // capteur sur le PIN 2
#define DHTTYPE DHT22   // DHT 22  (AM2302)

String groupe = "03";
String id = "1";
String DateHeure;
double Latitude = 0;
double Longitude = 0;
double Gyro_yaw = 0;
double Gyro_pitch = 0;
double Gyro_roll = 0;
double Accel_x = 0;
double Accel_y = 0;
double Accel_z = 0;
double Temperature;
int Humidite;
const int chipSelect = 10;

DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 RTC;
File dataFile;

void setup() 
{
  //Capteur de température
  Serial.begin(9600);
  dht.begin();

  //Horloge
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) 
  {
    Serial.println("RTC is NOT running!");
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  //Carte SD
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
}

void loop() 
{
  capteur_temperature(Temperature, Humidite);

  capteur_gyro();

  capteur_gps();

  capteur_horloge(DateHeure);

  carte_SD();

  delay(5000);

}

void capteur_temperature(double &Temperature, int &Humidite)
{
  delay(2000);

  Humidite = dht.readHumidity();
  Temperature = dht.readTemperature();

  Temperature = Temperature + 273.15;

  if (isnan(Humidite) || isnan(Temperature)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  return;
}

void capteur_gyro()
{
  
}

void capteur_gps()
{
  
}

void capteur_horloge(String &DateHeure)
{
  String year;
  String month;
  String day;
  String hour;
  String minute;
  String second;
  
  DateTime now = RTC.now();

  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second();

  DateHeure = year + "-" + month + "-" + day + "T" + hour + ":" + minute + ":" + second + ":" + "000Z";
  
  delay(1000);
}

void carte_SD()
{
  dataFile = SD.open("test.txt", FILE_WRITE);

  dataFile.println("{");
  dataFile.println("\"DATA\":");
  dataFile.println("      [");  
  dataFile.println("            {");
  dataFile.print("                  \"ID\": \""); dataFile.print(groupe); dataFile.print(id); dataFile.println("\",");
  dataFile.print("                  \"DateHeure\": \""); dataFile.print(DateHeure); dataFile.println("\",");
  dataFile.print("                  \"Latitude\": \""); dataFile.print(Latitude); dataFile.println("\",");
  dataFile.print("                  \"Longitude\": \""); dataFile.print(Longitude); dataFile.println("\",");
  dataFile.print("                  \"Gyro_yaw\": \""); dataFile.print(Gyro_yaw); dataFile.println("\",");
  dataFile.print("                  \"Gyro_pitch\": \""); dataFile.print(Gyro_pitch); dataFile.println("\",");
  dataFile.print("                  \"Gyro_roll\": \""); dataFile.print(Gyro_roll); dataFile.println("\",");
  dataFile.print("                  \"Accel_x\": \""); dataFile.print(Accel_x); dataFile.println("\",");
  dataFile.print("                  \"Accel_y\": \""); dataFile.print(Accel_y); dataFile.println("\",");
  //dataFile.print("                  \"Accel_z\": \""); dataFile.print(Accel_z); dataFile.println("\",");
  dataFile.print("                  \"Temperature\": \""); dataFile.print(Temperature); dataFile.println("\",");
  dataFile.print("                  \"Humidite\": \""); dataFile.print(Humidite); dataFile.println("\",");
  dataFile.println("            },");
  dataFile.println("      ]");
  dataFile.println("}");
  dataFile.close();

  Serial.print("Enregistrement Ok");
  delay(10000);
}
