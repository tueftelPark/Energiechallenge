// Energiechallenge Tüftelcamp 2023 V5 02.07.2023


#include <Arduino.h>

// Inputs
#define StartMessung A0
#define StoppMessung A1
#define Restart A2
#define StartMotor A3

// Outputs
const uint8_t AnzahlLEDs = 10;
const uint32_t LED[AnzahlLEDs] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // LED[0] schaltet ein wenn Grenze1 überschritten wurde. LED's müssen beginnend von 11 dazukommen.
volatile uint8_t AnzahlLEDOn=0;                                       // maximal 9 als Wert, Range 0-9.

const uint8_t Motor = 12;

const uint8_t Lichtschranke = 13;


// Grenzen1-10 grösste nach kleinste
//exponentieller Anstieg e^(x/2.56)*15 
const uint16_t Grenze1 = 500;
const uint16_t Grenze2 = 340;
const uint16_t Grenze3 = 231;
const uint16_t Grenze4 = 156;
const uint16_t Grenze5 = 105;
const uint16_t Grenze6 = 71;
const uint16_t Grenze7 = 48;
const uint16_t Grenze8 = 32;
const uint16_t Grenze9 = 22;
const uint16_t Grenze10 = 15;

// Hilfsvariabeln
volatile uint16_t Umdrehungen = 0;
volatile uint64_t ZeitMessung;



//Variabeln für Start Drehorgel, Zeitangaben in ms
volatile uint16_t LaufzeitOrgel=0;
volatile uint64_t ZeitMessungOrgel;
volatile uint64_t AbschaltungZeitMessung;

const uint16_t OffsetLaufzeitOrgel =10000;

const uint16_t LaufzeitGrenz1 = 20000;
const uint16_t LaufzeitGrenz2 = 18000;
const uint16_t LaufzeitGrenz3 = 16000;
const uint16_t LaufzeitGrenz4 = 14000;
const uint16_t LaufzeitGrenz5 = 12000;
const uint16_t LaufzeitGrenz6 = 10000;
const uint16_t LaufzeitGrenz7 = 8000;
const uint16_t LaufzeitGrenz8 = 6000;
const uint16_t LaufzeitGrenz9 = 4000;
const uint16_t LaufzeitGrenz10 = 2000;

//Funktionsdeklarationen
void InitInput(void);
void InitOutput(void);
void InitSensor(void);
void SignalisierungStart(void);
void SignalisierungStopp(void);
void alleLedEin(void);
void alleLedAus(void);
void CounterLichtschranke(void);
void AktualisierungLED(void);
void warten(uint64_t);
void ZurucksetzenUmdrehungen(void);









void setup()
{
  Serial.begin(9600);
  InitInput();
  InitOutput();
  InitSensor();
}












void loop()
{

  //Beginn Messung Wasserrad oder Wasserrad. Hört auf bei Beätigung Stopp
  if (digitalRead(StartMessung) == LOW)
  {
    Serial.println("Start Messung");
    SignalisierungStart();
    CounterLichtschranke();
    SignalisierungStopp();
  }


// Restart, Laufzeit der Orgel und Anzahl Umdrehungen wird zurückgesetzt und der Stand der LED's wird aktualisiert
  if (digitalRead(Restart) == LOW)
  {
  ZurucksetzenUmdrehungen();
  }


 //Orgel kann nur drehen, wenn es schon eine Anzahl Umdrehungen gibt
 //Motor beginnt für eine gewisse Offsetzeit. 
 //Nach der Offsetzeit. schalten alle 2 Sekunden eine LED aus bis keine mehr leuchtet.
  if (digitalRead(StartMotor) == LOW && Umdrehungen > 0 )
  {
Serial.println("Start Orgel");
    digitalWrite(Motor, HIGH);
    warten(OffsetLaufzeitOrgel);
Serial.println("Offsetzeit durch");


    ZeitMessungOrgel = millis();

    while ((millis() - ZeitMessungOrgel < LaufzeitOrgel) || (digitalRead(Restart) == LOW))
    {
      for (AnzahlLEDOn; AnzahlLEDOn > 0; AnzahlLEDOn--)
      {
        AbschaltungZeitMessung = millis();

        while ((millis() - AbschaltungZeitMessung < 1800) || (digitalRead(Restart) == LOW))
        {
        }
        digitalWrite(LED[AnzahlLEDOn], LOW);
        Serial.print("LED aus");
        Serial.println(AnzahlLEDOn);

        if (digitalRead(Restart) == LOW)
        {
          break;
        }
      }
    }

    Serial.println("Aktualisiert");
      Umdrehungen=0;
    AnzahlLEDOn=0;
  AktualisierungLED();
  digitalWrite(Motor,LOW);
  Serial.print("Zustand Motor");
  Serial.println(digitalRead(Motor));
  }
}













/// @brief Initialisierung aller Input
void InitInput()
{
  pinMode(StartMessung, INPUT_PULLUP);
  pinMode(StoppMessung, INPUT_PULLUP);
  pinMode(Restart, INPUT_PULLUP);
  pinMode(StartMotor, INPUT_PULLUP);
  Serial.println("Initialisierung Inputs");
}

/// @brief Initialisierung aller Outputs + Sicherstellen das alles ausgeschaltet ist
void InitOutput()
{
  //Initialisierung aller LEDS
  for (uint8_t i = 0; i < AnzahlLEDs; i++)
  {
    pinMode(LED[i], OUTPUT);
  }

//Initialisierung aller Motoren
pinMode(Motor,OUTPUT);
  digitalWrite(Motor, LOW);
  alleLedAus();
  Serial.println("Initialisierung Outputs");
}

/// @brief Initialisierung der Sensoren
void InitSensor()
{
  pinMode(Lichtschranke, INPUT_PULLUP);
  Serial.println("Initialisierung Sensor");
}

/// @brief Signalisierung des Startes einer Messung. Lichter blinken 2 mal lang und als letztes 1 mal kurz
void SignalisierungStart()
{
  Serial.println("Signalisierung Start");
  alleLedEin();
  warten(1000);
  alleLedAus();
  warten(1000);

  alleLedEin();
  warten(1000);
  alleLedAus();
  warten(1000);

  alleLedEin();
  warten(500);
  alleLedAus();
  AktualisierungLED();
}

/// @brief Signalisierung das Messung beendet ist. LED's leuchten 8 mal kurz
void SignalisierungStopp()
{
  Serial.println("Signalisierung Stop");
for(int i=0;i<8;i++)
{
  alleLedAus();
  warten(200);
  alleLedEin();
  warten(200);
}
alleLedAus();
  AktualisierungLED();
}

/// @brief alle LEDS werdem eingeschaltet
void alleLedEin()
{
  for (uint8_t i = 0; i < AnzahlLEDs; i++)
  {
    digitalWrite(LED[i], HIGH);
  }
}

/// @brief alle LEDs werden ausgeschaltet
void alleLedAus()
{
  for (uint8_t i = 0; i < AnzahlLEDs; i++)
  {
    digitalWrite(LED[i], LOW);
  }
}

/// @brief Pro Durchbruch der Lichtschranke beim Wasserrad wird die Umdrehungsvariable um 1 hochgezählt. Zählung wird abgebrochen durch betätigung des Stopptaster
void CounterLichtschranke()
{
  while (!(digitalRead(StoppMessung) == LOW || digitalRead(Restart) == LOW))
  {

    if (digitalRead(Lichtschranke) == LOW)
    {
     while (1)
     {
      if (digitalRead(Lichtschranke) == HIGH || digitalRead(StoppMessung)==LOW || digitalRead(Restart) == LOW)
      {
        break;
      }
     }

       ++Umdrehungen;
   AktualisierungLED();
    }
  }
  if(digitalRead(Restart) == LOW)
  {
    ZurucksetzenUmdrehungen();
  }

}

/// @brief Aktualisierung des Leuchtstands der LEDs gemäss der Umdrehungsanzahl. Zuerst werden alle LEDS ausgeschaltet und dann alle entsprechenden wieder eingeschaltet.
void AktualisierungLED()
{
Serial.println("Aktualisierung LED");
Serial.print("Umdrehungen: ");
Serial.println(Umdrehungen);

  if (Umdrehungen > Grenze1)
  {
    for (uint8_t i = 0; i < 10; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 1 überschritten");
    LaufzeitOrgel=LaufzeitGrenz1;
    AnzahlLEDOn=9;
  }

  else if (Umdrehungen > Grenze2)
  {
    for (uint8_t i = 0; i < 9; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 2 überschritten");
    LaufzeitOrgel=LaufzeitGrenz2;
    AnzahlLEDOn=8;

  }

  else if (Umdrehungen > Grenze3)
  {

    for (uint8_t i = 0; i < 8; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 3 überschritten");
    LaufzeitOrgel=LaufzeitGrenz3;
    AnzahlLEDOn=7;

  }

  else if (Umdrehungen > Grenze4)
  {

    for (uint8_t i = 0; i < 7; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 4 überschritten");
    LaufzeitOrgel=LaufzeitGrenz4;
    AnzahlLEDOn=6;

  }

  else if (Umdrehungen > Grenze5)
  {
    for (uint8_t i = 0; i < 6; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 5 überschritten");
    LaufzeitOrgel=LaufzeitGrenz5;
    AnzahlLEDOn=5;

  }

  else if (Umdrehungen > Grenze6)
  {
    for (uint8_t i = 0; i < 5; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 6 überschritten");
    LaufzeitOrgel=LaufzeitGrenz6;
    AnzahlLEDOn=4;

  }

  else if (Umdrehungen > Grenze7)
  {
    for (uint8_t i = 0; i < 4; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 7 überschritten");
    LaufzeitOrgel=LaufzeitGrenz7;
    AnzahlLEDOn=3;


  }

  else if (Umdrehungen > Grenze8)
  {
    for (uint8_t i = 0; i < 3; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 8 überschritten");
    LaufzeitOrgel=LaufzeitGrenz8;
    AnzahlLEDOn=2;


  }

  else if (Umdrehungen > Grenze9)
  {
    for (uint8_t i = 0; i < 2; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 9 überschritten");
    LaufzeitOrgel=LaufzeitGrenz9;
    AnzahlLEDOn=1;


  }

  else if (Umdrehungen > Grenze10)
  {
    for (uint8_t i = 0; i < 1; i++)
    {
      digitalWrite(LED[i], HIGH);
    }
    Serial.println("Grenze 10 überschritten");
    LaufzeitOrgel=LaufzeitGrenz10;
    AnzahlLEDOn=0;


  }
  
  else
  {
    alleLedAus();
    Serial.println("Keine Grenze überschritten");
  }
}


/// @brief Ersatz für Delay, diese ist nicht blockierend
/// @param WarteZeit Zeit in ms
void warten(uint64_t WarteZeit)
{
  ZeitMessung = millis();
  while (millis() - ZeitMessung < WarteZeit)
  {
  }
}

//Variable Umdrehungen werden zurückgesetzt und LED's werden aktualisiert
void ZurucksetzenUmdrehungen()
{
  Serial.println("Restart");
  Serial.println(Umdrehungen);
    Umdrehungen = 0;
    AktualisierungLED();
    while(digitalRead(Restart)== LOW)
    {}
}
