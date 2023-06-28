// _____Anschlüsse______________________
//Ein
int poti = A0; //Zeiteinstellung bzw. Verzögerungszeit
int LDR = A1; // Analoger Pin  LDR (Lichtschranke mit Laserdiode-Fotowiderstand)
const int taster_1 = 2; 
volatile bool ein=0;


//const int taster_2 = 3; 
const int infrarot = 4;
const int magnet   = 5;


//AUS
const int LED_gruen = 8;
const int LED_gelb  = 9;
const int LED_rot   = 10;
const int relais    = 11;  //Relais für die Sirene 12V oder 230V
const int laser_led = 12;

//Zeit
unsigned long einschaltVerzoegerung = 10000;  // Einschaltverzögerung in Millisekunden
unsigned long startzeit = 0;                  // Startzeit der Verzögerung

//_______ Variablen für Zustände______________________________________________________________
bool einmal = false;         // Einschalt Verzögerung aktiv oder nicht
bool vormal = false;              // Alarm Verzögerung aktiv oder nicht
bool ledStatus = false;  
bool vormerk = false;                     // Zustand der LED (ein- oder ausgeschaltet)
bool alarm = false;
bool scharf = false; //Anlage ist zu Beginn nicht scharf geschaltet, die Melder sind nicht im Betrieb

bool licht_merk = false;   //Variable in der das einmalige überschreiten des Meßwertes gespeichert wird
int licht = 0;             //Variable für den gemessenen Wert am Fotowiderstand
bool bewegung_merk = false;
bool magnet_merk = false; //Reedkontakt gibt 1 Signal solange Magnetkontakt und Magnet nebeneinander liegen (Tür geschlossen)

//_____ Bewegungsmelder_____________________________________________________
bool bewegung_status = false; // Variable für den Sensor Status  (PIR-Infrarotsensor)
//____Reedkontakt Schalter________________
bool reed_jetzt=false;
bool reed_vorher=false;

//________________________Funktionen___________________________________________________________
void ein_verz()
{
 int potiWert = analogRead(poti);
 unsigned long einschalt_V = map(potiWert, 0, 1023, 10000, 90000);// Umrechnung des Poti-Wertebereichs auf die gewünschte Einschaltverzögerung
 

  
  if (ein == true && einmal == false && !scharf)// Verzögerung starten, wenn der Taster gedrückt wird und keine Verzögerung aktiv ist
     {
     startzeit = millis();      // Startzeit der Verzögerung festlegen
     einmal = true;   // Verzoegerung aktiv setzen
     digitalWrite(laser_led,HIGH); // LED einschalten
     }

  
  if (einmal == true)// Verzögerungszeit abwarten und LED blinken lassen
     {
     unsigned long vergangeneZeit = millis() - startzeit;
     if (vergangeneZeit < einschalt_V)
        {
        if (vergangeneZeit % 1000 < 500) {
        digitalWrite(LED_gelb, HIGH);  // LED einschalten
      } else {
        digitalWrite(LED_gelb, LOW);   // LED ausschalten
      }
    } else {
      scharf=true;
      einmal = false;     // Verzögerung beenden
      digitalWrite(LED_gelb, LOW);   // LED ausschalten
      
            
    }
  }
}

void vor_alarm()
{
 int potiWert = analogRead(poti);
 unsigned long einschalt_V = map(potiWert, 0, 1023, 10000, 90000);// Umrechnung des Poti-Wertebereichs auf die gewünschte Einschaltverzögerung
 if (vormerk ==false && vormal == false)// Verzögerung starten, wenn der Alarm ausgelöst wird wird und keine Verzögerung aktiv ist
     {
     startzeit = millis();      // Startzeit der Verzögerung festlegen
     vormal = true;   // Verzoegerung aktiv setzen
     }

 if (vormal == true && !einmal)// Verzögerungszeit abwarten und LED blinken lassen
     {
     unsigned long vergangeneZeit = millis() - startzeit;
     if (vergangeneZeit < einschalt_V)
        {
        if (vergangeneZeit % 1000 < 500) {
        digitalWrite(LED_rot, HIGH);  // LED einschalten
      } else {
        digitalWrite(LED_rot, LOW);   // LED ausschalten
      }
    } else {
      vormal = false;  // Verzögerung beenden
      vormerk=true;   // 
      digitalWrite(relais, HIGH); 
      digitalWrite(LED_rot, HIGH);   // LED einschalten
    }
  }
}//Ende vor_alarm


void tuer()
{
  reed_jetzt = digitalRead(magnet);// Aktuellen Zustand des Reedkontakts lesen
  if(reed_jetzt != reed_vorher) 
  {
    delay(100);   // Entprellzeit von 50 ms
    reed_jetzt = digitalRead(magnet);  // Zustand erneut lesen
    if (reed_jetzt != reed_vorher)
    {
      if (reed_jetzt == HIGH)  // Zustandsänderung des Reedkontakts festgestellt
      {
        magnet_merk=true;    // Merker Magnet
        Serial.print("magnet hat ausgelöst");
      } 
      reed_vorher = reed_jetzt;    // Vorherigen Zustand aktualisieren
    }
  }

}

void lichtschranke()
{
  if(licht<900)
  {
   Serial.print("Lichtschranke ausgelöst!");
   licht_merk = true;
  } 
  else
  {
    licht_merk = false;
   
  } 

}

void bewegungsmelder()
{
 bool jetzt_bewegung=digitalRead(infrarot);// aktueller (jetzt) Status des Bewegungsmelders wird an die Variable"jetzt_sensor" übergeben
 if(jetzt_bewegung != bewegung_status)
 {
    bool bewegung_status = jetzt_bewegung; //
    bewegung_merk = true; //

   if(bewegung_merk==true)
   {
     Serial.print("Bewegungsmelder hat ausgelöst");
     
 
   }

     else
   {
    Serial.print("....nix los....");

   }
 }
} //sensor ende

void ein_taster()
{
 ein = true; //Variable auf "true" setzen, wenn Interrupt ausgelöst wurde
}

//_______________________________________________________________________________________________
void setup() 
{
  Serial.begin(9600);
  pinMode(taster_1,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(taster_1), ein_taster, RISING);        

  
  pinMode(infrarot,INPUT);

  pinMode(magnet,  INPUT_PULLUP);
  digitalWrite(magnet,HIGH);

  pinMode(LED_rot, OUTPUT);
  pinMode(LED_gelb, OUTPUT);
  pinMode(LED_gruen, OUTPUT);
  pinMode(laser_led, OUTPUT);
  pinMode(relais, OUTPUT);
}

void loop() 
{

if (ein) 
{
  ein = false;  // Zustandsänderung zurücksetzen, um erneute Auslösung zu verhindern
  
}

while(alarm == false)
{
  // Lichtschranke
  licht =analogRead(LDR); 
  Serial.print("LDR: ");
  Serial.println(licht); //Ausgabe am Serial-Monitor
  
  LED_gelb != LED_rot;
  ein_verz();

  if(scharf == true)
   {
   digitalWrite(LED_gruen,HIGH);
   tuer();
   bewegungsmelder();
   lichtschranke();
   if (magnet_merk && licht_merk || magnet_merk && bewegung_merk || licht_merk && bewegung_merk) //wenn mindestens 2 Melder 1-Signal haben
      {
       digitalWrite(LED_gruen,LOW);
       alarm=true;
       
      }  
   }
}
 
  if(alarm==true  )
  {
    scharf=false;
    vor_alarm();
  }
  
}