// _____Anschlüsse______________________
//Ein
int poti = A0; //Zeiteinstellung bzw. Verzögerungszeit
int LDR = A1; // Analoger Pin  LDR (Lichtschranke mit Laserdiode-Fotowiderstand)
const int taster_1 = 2; //Ein-Taster
const int taster_2 = 3; //Aus-Taster
const int infrarot = 4; //Bewegungsmelder
const int magnet   = 5; //Türmagnet
//Aus
const int LED_gruen = 8;
const int LED_gelb  = 9;
const int LED_rot   = 10;
const int relais    = 11;  //Relais für die Sirene 12V oder 230V
//_____ Bewegungsmelder_____________________________________________________
bool bewegung_status = false; // Variable für den Sensor Status  (PIR-Infrarotsensor)


//_____Variablen für Zeit_______________________________

//unsigned long vorher = 0;  // Variable zum Speichern der vorherigen Zeit
//unsigned long start = 0;  // Variable zum Speichern der vorherigen Zeit
unsigned long einschalt_V = 5000; // Verzögerungszeit in Millisekunden (hier 5 Sekunden)
//unsigned long startzeitpunkt = 0;

int verzoegerung = 0; //Verzögerungszeit mittels Poti wählbar zwischen
//int x= 0;          //zähler einzeit

//_______ Variablen für Zustände____________________________________________

bool ein_merk = false;     //anfangswert =0
bool aus_merk = false;     //anfangswert =0
bool alarm_merk = false;  //Zustände der Anlage: - Alarm wurde ausgelöst
bool nix_merk = true;    //Zustände der Anlage: - Anlage ist aus
bool scharf_merk = false; //Zustände der Anlage: - eingeschaltet bzw. scharf geschaltet
bool licht_merk = false;   //Variable in der das einmalige überschreiten des Meßwertes gespeichert wird
int licht = 0;             //Variable für den gemessenen Wert am Fotowiderstand
bool bewegung_merk = false;
bool magnet_merk = false; //Reedkontakt gibt 1 Signal solange Magnetkontakt und Magnet nebeneinander liegen (Tür geschlossen)
bool soll_gelb = false; //soll LED_gelb blinken



//_________________Funktionen______________________________________________
void betriebsbereit()//Anlage aus
{
  if (nix_merk == true)
  {
    digitalWrite (LED_gruen, HIGH);
  }
}

void einschaltung()
{
  if (ein_merk == true)
  {
    //blink_gelb;
    einschalt_Verz(); //Einschaltverzögerung Einstellung Poti
    delay(10000);// mindest Verzögerung 10000ms/10sek
    ein_merk = false;
    scharf_merk = true; //Anlage geht in Betrieb
  }
}

void einschalt_Verz()
{
  int potiWert = analogRead(poti);
  Serial.print("Poti: ");  // Gib den aktuellen Poti-Wert und die Delay-Zeit aus
  Serial.print(potiWert);
  Serial.print(", Zeit: ");

  einschalt_V = map(potiWert, 0, 1023, 0, 10000);// Umrechnung des Poti-Wertebereichs auf die gewünschte Einschaltverzögerung
  delay(einschalt_V);
}

void betrieb()
{
  ein_merk = false;
  scharf_merk = true;
  digitalWrite(LED_gruen, HIGH);
}


void ein_taster() // Ausschalt-Taster
{
  int ein_tast = digitalRead(taster_1); //
  if (ein_tast == LOW && !ein_merk)
  {
    delay(5);   //Taster entprellung
    if (ein_tast == LOW && !ein_merk)
    {
      nix_merk = false;
      ein_merk = true; //einschalt-vorgang eingeleitet
    }

  }
}


void aus_taster()// Ausschalt-Taster
{
  int aus_tast = digitalRead(taster_1); //
  if (aus_tast == LOW)
  {
    delay(5);
    if (aus_tast == LOW && !aus_merk)
    {

    }
  }

}//aus-taster ende

void voralarm()
{
  digitalWrite(LED_gelb, HIGH);
  Serial.print("Voralarm, ein Melder hat ausgelöst");
}



void alarm()
{
  delay(5000);//mindest Verzögerung 5000ms/5sek
  einschalt_Verz();//Einschaltverzögerung, + mindest Verz
  blink_rot();
  nix_merk = false;
  ein_merk = false;
  scharf_merk = false;
  alarm_merk = true;
  digitalWrite(relais, HIGH);

}//alarm ende

void bewegungsmelder()
{
  bool jetzt_bewegung = digitalRead(infrarot); // aktueller (jetzt) Status des Bewegungsmelders wird an die Variable"jetzt_sensor" übergeben
  if (jetzt_bewegung != bewegung_status)
  {
    bool bewegung_status = jetzt_bewegung; //
    bewegung_merk = true; //

    if (bewegung_merk == true)
    {
      Serial.print("Bewegungsmelder hat ausgelöst");


    }

    else
    {
      Serial.print("....nix los....");

    }
  }
} //sensor ende





void tuer()
{
  int magnet_jetzt = digitalRead(magnet);
  if (magnet_jetzt == LOW)
  {
    Serial.print("magnet hat ausgelöst");
    magnet_merk = true;
  }
  else
  {
    Serial.print("....nix los....");
  }
}

void lichtschranke()
{
  if (licht < 912)
  {
    Serial.print("Lichtschranke ausgelöst!");
    licht_merk = true;
  }
  else
  {
    licht_merk = false;
    Serial.print("Lichtschranke XXXXXXXXXX");
  }

}

void blink_rot()
{
  digitalWrite(LED_rot, !digitalRead(LED_rot));// LED wird ein- bzw. ausgeschaltet,je nach aktuellen Status der LED soll das Gegenteil erfolgen
  delay(500);
}


void blink_gelb()
{
  digitalWrite(LED_gelb, !digitalRead(LED_gelb));// LED wird ein- bzw. ausgeschaltet,je nach aktuellen Status der LED soll das Gegenteil erfolgen
  delay(500);
}
void ausschaltung()
{
  scharf_merk = false;
  alarm_merk = false;
  Serial.print("anlage wurde ausgeschaltet ");
  nix_merk = true;
}

//__________________________________________________________________________
void setup()
{
  Serial.begin(9600);


  pinMode(taster_1, INPUT);
  pinMode(taster_2, INPUT);
  pinMode(infrarot, INPUT);

  pinMode(magnet,  INPUT_PULLUP);
  digitalWrite(magnet, HIGH);

  pinMode(LED_rot, OUTPUT);
  pinMode(LED_gelb, OUTPUT);
  pinMode(LED_gruen, OUTPUT);
  //pinMode(laser, OUTPUT);
  pinMode(relais, OUTPUT);

}
//_____________________________________________________________________________________________________________________________________________
void loop()
{
  //Zeiteinstellung Poti
  int potiValue = analogRead(poti); // Lese den Wert des Potentiometers aus
  verzoegerung = map(potiValue, 0, 1023, 30000, 600000); // Berechne die Verzögerungszeit aus dem Poti-Wert (0-1023) im Bereich von 30 bis 600 Sekunden

  // Lichtschranke
  licht = analogRead(LDR);
  Serial.print("LDR: ");
  Serial.println(licht); //Ausgabe am Serial-Monitor

  //Beginn Hauptprogramm
  ein_taster(); //Einbinden der Taster  sowie Einschaltbedingung
  aus_taster(); //Einbinden der Taster  sowie Ausschaltbedingung

  int(zustand) = 3;

  switch (zustand)
  {
    case 0:
      betriebsbereit();//Anlage aus
      break;

    case 1:
      einschaltung();//Taster_1 betätigt,Einschaltverzögerung läuft
      break;

    case 2:
      betrieb(); //Anlage ist scharf geschaltet
      break;

    case 3:
      ausschaltung();
  }

  while (scharf_merk == true)
  {
    digitalWrite(LED_gruen, HIGH);
    ein_merk = false;   //Merker für den Einschaltvorgang wird zurück gesetzt, da die Anlage im Betrieb ist
    tuer();             //und die 3 Melder Funktionen aufgerufen
    bewegungsmelder();
    lichtschranke();
    if (magnet_merk && licht_merk || magnet_merk && bewegung_merk || licht_merk && bewegung_merk) //wenn mindestens 2 Melder 1-Signal haben
    {
      alarm(); //Funktion Alarm wird ausgelöst
      alarm_merk = true;
    }

  }//while ende



}//loop ende