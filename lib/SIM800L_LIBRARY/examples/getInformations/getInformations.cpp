#include "SIM800L.hpp"
#include <Arduino.h>              


Sim800L GSM;
/*
 * In alternative:
 * Sim800L GSM;                       // Use default pinout
 * Sim800L GSM(RX, TX, RESET);        
 * Sim800L GSM(RX, TX, RESET, LED);
 */

void setup(){
  Serial.begin(9600);
  Serial.println("Iniciando...");

  GSM.begin();
  GSM.reset();


  Serial.println("GET PRODUCT INFO: ");
  Serial.println(GSM.getNumSerie());

  //normal to take
  Serial.println("GET OPERATORS LIST: ");
  Serial.println(GSM.getOperatorsList());

  Serial.println("GET OPERATOR: ");
  Serial.println(GSM.getOperator());
  
}

void loop(){
  
}

