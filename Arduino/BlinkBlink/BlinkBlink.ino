int pinLed=13; //variable pour le numéro du pin utilisé

void setup()

{

    pinMode(pinLed,OUTPUT); //le pin 13 en mode sortie de courant

}

void loop()

{

    digitalWrite(pinLed,HIGH); // on passe le pin à +5V

    delay (1000);

    digitalWrite(pinLed,LOW); // on passe le pin à 0V

    delay(1000);

}
