#include <Arduino.h>
#include "SIM800L.hpp"

Sim800L GSM;

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando...");

  GSM.begin();
  GSM.reset();
  GSM.setFunctionalityMode(1);

  while (true) {
    String status = GSM.RegistrationStatus();
    Serial.println(status);
    if (status == "0,1") {
      break;
    }
  }

  GSM.sendSMS("+5524992830830", "ola teste kkk");
  delay(1000);
}

void loop() {

}