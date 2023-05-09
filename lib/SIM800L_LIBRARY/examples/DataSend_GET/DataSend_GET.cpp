#include <Arduino.h>
#include "SIM800L.hpp"

Sim800L GSM;


const String apn = "claro.com.br";                                                          // APN 
const String apn_u = "claro";                                                               // APN-Username 
const String apn_p = "claro";                                                               // APN-Password 
const String url = "http://em800.gensokyo.shop/api/v1/trigger/isca?name=00&lat=00&lon=00";  // Server URL
 


void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando...");

  GSM.begin();
  GSM.reset();
  GSM.setFunctionalityMode(1);

}

void loop() {

   while (true) {
    String status = GSM.RegistrationStatus();
    Serial.println(status);
    if (status == "0,1") {
      break;
    }
  }

  GSM.CONFIG_GPRS(apn ,apn_u, apn_p);
  delay(1000);

  GSM.Activate_GPRS();
  delay(1000);
  
  GSM.Start_HTTP(url);
  delay(1000);

  Serial.println(GSM.Send_HTTP(0));
  delay(1000);

  GSM.Close_Connection();
  delay(1000);
}