
String id = "1234567890";
String DateHeure = "2016-06-17T15:53:23:231Z";
double Latitude = 0.1578949;
double Longitude = 0.1578949;
double Gyro_yaw = 5.2644875;
double Gyro_pitch = 0.1470014;
double Gyro_roll = 6.1874977;
double Accel_x = 0.56;
double Accel_y = 0.01;
double Accel_z = 0.47;
double Temperature = 292.15;
int Humidite = 50;

void setup()

{

    //initialisation de la communication

    Serial.begin(9600);

    //affichage du texte

    Serial.println("{");
    Serial.println("\"DATA\":");
    Serial.println("      [");
    
    Serial.println("            {");
    Serial.print("                  \"ID\": \""); Serial.print(id); Serial.println("\",");
    Serial.print("                  \"DateHeure\": \""); Serial.print(DateHeure); Serial.println("\",");
    Serial.print("                  \"Latitude\": \""); Serial.print(Latitude); Serial.println("\",");
    Serial.print("                  \"Longitude\": \""); Serial.print(Longitude); Serial.println("\",");
    Serial.print("                  \"Gyro_yaw\": \""); Serial.print(Gyro_yaw); Serial.println("\",");
    Serial.print("                  \"Gyro_pitch\": \""); Serial.print(Gyro_pitch); Serial.println("\",");
    Serial.print("                  \"Gyro_roll\": \""); Serial.print(Gyro_roll); Serial.println("\",");
    Serial.print("                  \"Accel_x\": \""); Serial.print(Accel_x); Serial.println("\",");
    Serial.print("                  \"Accel_y\": \""); Serial.print(Accel_y); Serial.println("\",");
    Serial.print("                  \"Accel_z\": \""); Serial.print(Accel_z); Serial.println("\",");
    Serial.print("                  \"Temperature\": \""); Serial.print(Temperature); Serial.println("\",");
    Serial.print("                  \"Humidite\": \""); Serial.print(Humidite); Serial.println("\",");
    Serial.println("            },");
    
    Serial.println("      ]");
    Serial.println("}");

}

void loop()

{



}
