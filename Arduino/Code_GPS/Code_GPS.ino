#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SD.h>

File dataFile;

const int chipSelect = 4;
long lat,lon;

SoftwareSerial gpsSerial(1,0);
TinyGPS gps;

void setup() 
{
  Serial.begin(115200);
  gpsSerial.begin(9600);

  //Debut d'initialisation de la carte SD
  Serial.print("Initialisation de la carte SD...");

  //Iniatialisation de la carte SD
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("L'initialisation a echoue");
    return;
  }
  
  Serial.println("Carte Ok");

  dataFile = SD.open("GPS.txt", FILE_WRITE);
  dataFile.print("Start logging on: ");
  dataFile.println("Latitude              Longitude              Time");
  dataFile.close();
  Serial.println("Création fichier Ok");  
}

void loop() 
{
  while(gpsSerial.available())
  { 
    if(gps.encode(gpsSerial.read()))
    { 
      gps.get_position(&lat,&lon);
      
      Serial.print("Position: ");
      Serial.print("coordinaat ");Serial.print(lat/1000000); Serial.print(".");Serial.print(lat%1000000);Serial.print(" "); // Affichage latitude sur le moniteur
      Serial.print(", ");Serial.print(lon/1000000); Serial.print(".");Serial.println(lon%1000000); // Affichage longitude sur le moniteur
      
      dataFile = SD.open("GPS.txt", FILE_WRITE);
      if (dataFile) 
      {
        dataFile.print(lat/1000000); dataFile.print("."); dataFile.print(lat%1000000); dataFile.print(" "); // Affichage latitude sur la carte SD
        dataFile.print("            ");
        dataFile.print(lon/1000000); dataFile.print("."); dataFile.print(lon%1000000); // Affichage longitude sur le carte SD
        dataFile.close();
        Serial.println("Donnée bien stocké");
      }
    }
    else 
    {
      Serial.println("Erreur ouverture du fichier : GPS.txt");
    }
    delay(10000);
  }      
}
