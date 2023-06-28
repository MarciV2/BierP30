#include <SPI.h>
#include <Wire.h>


int BEER_VALVE_OPEN_TIME = 40;
#define BEER_VALVE_RELAIS_PIN 5

long beer_valve_open_millis = 0;
bool beer_active = false;

bool refill_ice = false;



void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  SPI.begin();  // Init SPI bus
  Wire.begin(); 


  initLedMatrix();
  initLedStripe();
  initRFID();
  initTemp();

 
  pinMode(BEER_VALVE_RELAIS_PIN, OUTPUT);
  digitalWrite(BEER_VALVE_RELAIS_PIN, LOW);
}

void loop() {

  //Timer checks
  if (beer_active) {
    //Timer-Bar anpassen
    long beer_millis_elapsed = millis() - beer_valve_open_millis;
    uint8_t beer_timer_percent = 100 - round(beer_millis_elapsed / (BEER_VALVE_OPEN_TIME * 10));
    setBeerTimerBar(beer_timer_percent,255,170,0,false);

    //Zapfphase zu ende -> stoppen
    if (beer_timer_percent <= 0) {
      stopBier();
    }
  }

  displayTimerEvent();
  ledStripeTimerEvent();
  rfidTimerEvent();
  tempTimerEvent();
  
}



// Funktion wird aufgerufen, wenn Autentifizierung mit FreiBierKarte erfolgreich
void startBier() {
  beer_valve_open_millis = millis();
  digitalWrite(BEER_VALVE_RELAIS_PIN, HIGH);
  beer_active = true;
  if(refill_ice) displayMessage("Eis nachfuellen", false);
  else displayMessage("Ozapft is", false);
}


void abortBier(){
  setBeerTimerBar(100,255,0,0,true);
  digitalWrite(BEER_VALVE_RELAIS_PIN, LOW);
  beer_active = false;
}

// Funktion wird aufgerufen, wenn der Timer des Biers abgelaufen ist
void stopBier() {
  Serial.println("Timer abgelaufen, Prost!");
  digitalWrite(BEER_VALVE_RELAIS_PIN, LOW);
  beer_active = false;
  displayMessage("Prost", true);
  setBeerTimerBar(100,0,0,255,true);

}

