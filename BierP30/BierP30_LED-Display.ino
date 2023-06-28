#include <MFRC522.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <Adafruit_NeoPixel.h>

//Matrix-Display
#define LED_MATRIX_CS 3
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 3
MD_Parola P = MD_Parola(HARDWARE_TYPE, LED_MATRIX_CS, MAX_DEVICES);
uint8_t scrollSpeed = 600;  // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 1500;  // in milliseconds
char defaultMessage[] = "Karte bitte";
char curMessage[16] = { " " };  // used to hold current message

//LED-Stripe
#define LED_PIN 4
#define LED_COUNT 19
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


//Zurücksetzen von Fehlermeldungen/Ende-Meldung
#define DISPLAY_DEFAULT_DELAY 5
long display_start_millis = 0;
bool display_runsOut = false;
bool ledStripe_runsOut=false;

//Led-Stripe Idle-Blinken
#define LED_STRIPE_IDLE_INTERVAL 2
long ledStripe_idleToggle_millis=0;
bool ledStripeIdle=true;
bool ledStripeCurrentlyOn=false;


void initLedMatrix(){
  // Matrix-LED-Modul initiieren
  P.begin();
  P.setSpeed(scrollSpeed);
  P.setTextAlignment(PA_LEFT);
  P.displayClear();
  P.displayReset();
  strcpy(curMessage, defaultMessage);
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, PA_PRINT);
}

void initLedStripe(){
  // LED-Stripe
  strip.begin();             // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();              // Turn OFF all pixels ASAP
  strip.setBrightness(255);  // Set BRIGHTNESS to max
}

// Setzt die LED-Bar auf den angegebenen Prozentsatz (von links), mit der Farbe in RGB und resetted (wenn aktiviert) zusammen mit Display-Text
void setBeerTimerBar(uint8_t percent, uint8_t red, uint8_t green, uint8_t blue, bool runsOut) {
  ledStripeIdle=false;
  ledStripe_runsOut=runsOut;
  uint8_t ledNr = round(percent * LED_COUNT / 100.)+2;
  Serial.print("LedBar: ");
  Serial.println(ledNr);
  for (uint8_t i = 0; i <= LED_COUNT; i++) {
    if (i < ledNr) strip.setPixelColor(LED_COUNT-i, strip.Color(red, green, blue));
    else strip.setPixelColor(LED_COUNT-i, strip.Color(0, 0, 0));
  }
  strip.show();
}

//Leerlaufanimation (grünes blinken)
void doIdle(){
  if(ledStripeCurrentlyOn) setBeerTimerBar(0,0,0,0,false);
  else {
   if(refill_ice) setBeerTimerBar(100,255,0,0,false);
   else setBeerTimerBar(100,0,255,0,false);
  }
  ledStripeCurrentlyOn=!ledStripeCurrentlyOn;
  ledStripeIdle=true;
}

// Zeigt die angegebene Nachricht im Display sofort an
void displayMessage(char msg[], bool runsOut) {
  strcpy(curMessage, msg);
  P.displayClear();
  P.displayReset();
  if (runsOut) {
    display_start_millis = millis();
    display_runsOut = true;
  } else display_runsOut = false;
}


void displayTimerEvent(){
  //Display Anzeige evtl erneut starten
  if (P.displayAnimate())  // If finished displaying message
  {
    //Bei Störungsmeldung evtl zurücksetzen
    if (display_runsOut && millis() > (display_start_millis + DISPLAY_DEFAULT_DELAY * 1000)) {
      displayMessage(defaultMessage, false);
      display_runsOut = false;
      setBeerTimerBar(0,0,0,0,false);
    setLedStripeIdle();
    }
    P.displayReset();  // Reset and display it again
  }
}

//LED-Bar bei Idle-Animation toggeln (bei Auslaufender Meldung auf Idle setzen in displayTimerEvent, damit msg und stripe gleichzeitig geändert werden!)
void ledStripeTimerEvent(){
  
  //Idle-Animation
  if(ledStripeIdle && millis()>(ledStripe_idleToggle_millis+LED_STRIPE_IDLE_INTERVAL*1000)){
    doIdle();
    ledStripe_idleToggle_millis=millis();
  }
}

void setLedStripeIdle(){
  //setzen, sodass nächster timer-durchlauf die LED aktiviert und das Blinken beginnt
  ledStripeIdle=true;
  ledStripe_idleToggle_millis=millis()-(LED_STRIPE_IDLE_INTERVAL+1)*1000;
  ledStripeCurrentlyOn=false;

}
