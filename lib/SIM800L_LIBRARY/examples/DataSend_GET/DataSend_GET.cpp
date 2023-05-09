#include <Arduino.h>
#include "SIM800L.hpp"

Sim800L GSM;


const String apn = "";     // APN 
const String apn_u = "";   // APN-Username 
const String apn_p = "";   // APN-Password 
const String url = "";     // Server URL
 

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

  GSM.Send_CONFIG_HTTP(192, 5000, "DATA");
  Serial.println(GSM.Send_HTTP(0));
  delay(1000);
  
  // Post method 
  //GSM.Send_CONFIG_HTTP(192, 5000, "token=" + token + "&disp_id=1&event_data=");
  //Serial.println(GSM.Send_HTTP(1));
  //delay(1000);


  GSM.Close_Connection();
  delay(1000);
}
