#include <Arduino.h>
#include "SIM800L.hpp"

Sim800L GSM;
/*
 * In alternative:
 * Sim800L GSM;                       // Use default pinout
 * Sim800L GSM(RX, TX, RESET);        
 * Sim800L GSM(RX, TX, RESET, LED);
 */

const String apn = "zap.vivo.com.br";                                       // APN 
const String apn_u = "vivo";                                                // APN-Username 
const String apn_p = "vivo";                                                // APN-Password 
const String url = "http://www.exemplo.com/teste?name=00&lat=00&lon=00";    // Server URL
 


void setup() {
    Serial.begin(9600);
    Serial.println(" Iniciando...");

    GSM.begin();
    GSM.reset();
    GSM.setFunctionalityMode(1);

}

void loop() {
    //Update manual data RTC
    //Format: YY/MM/DD,hh:mm:ss, UTC
    GSM.updateRtcManual("18/03/03,21:00:00", "-03");
    delay(1000);

    //Update RTC Sim800L
    GSM.RTCtime();
    delay(1000);

    //Update RTC with GSM connection
    //APN, APN_user, APN_password, UTC
    GSM.updateRtc(apn, apn_u, apn_p, "-03");
    delay(1000);

    //Update RTC Sim800L
    GSM.RTCtime();
    delay(1000);

    //Print DataTime RTC
    Serial.println("year: "+GSM.DateTime.year);
    Serial.println("month: "+GSM.DateTime.month);
    Serial.println("day: "+GSM.DateTime.day);
    Serial.println("hour: "+GSM.DateTime.hour);
    Serial.println("minute: "+GSM.DateTime.minute);
    Serial.println("second: "+GSM.DateTime.second);
}