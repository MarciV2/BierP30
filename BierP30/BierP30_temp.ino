#include <OneWire.h>                        //OneWire Bibliothek einbinden
#include <DallasTemperature.h>              //DallasTemperatureBibliothek einbinden
#include <NonBlockingDallas.h>   


#define ONE_WIRE_BUS 9 
#define TIME_INTERVAL 2000  

// für Temp-Sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemp(&oneWire);
NonBlockingDallas sensorDs18b20(&dallasTemp); 

//maximale Temperatur von Nasskühlung
uint8_t sollTemp=10;



void initTemp(){
  sensorDs18b20.begin(NonBlockingDallas::resolution_12, NonBlockingDallas::unit_C, TIME_INTERVAL);
  //sensorDs18b20.onIntervalElapsed(handleIntervalElapsed);
  sensorDs18b20.onTemperatureChange(handleTemperatureChange);
}

void tempTimerEvent(){

  sensorDs18b20.update();

}

void handleIntervalElapsed(float temperature, bool valid, int deviceIndex){

  Serial.print("Sensor ");
  Serial.print(deviceIndex);
  Serial.print(" temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  /*
   *  DO SOME AMAZING STUFF WITH THE TEMPERATURE
   */
}

//Invoked ONLY when the temperature changes between two sensor readings
void handleTemperatureChange(float temperature, bool valid, int deviceIndex){

  Serial.print("Temperatur: "); 
    Serial.println(temperature);
    //mit Soll-Temp vergleichen
    if(temperature>sollTemp){
      if(refill_ice==false){
        refill_ice=true;
        setBeerTimerBar(100,255,0,0,true);
        displayMessage("Eis nachfuellen", true);
      }
    }
    else
      refill_ice=false;
}

