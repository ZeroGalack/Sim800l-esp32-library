#include "Arduino.h"
#include "Sim800L.hpp"

HardwareSerial SerialGSM(1);


Sim800L::Sim800L(uint8_t rx, uint8_t tx, uint8_t rst) {

    RX_PIN 		= rx;
    TX_PIN 		= tx;
    RESET_PIN 	= rst;
}

void Sim800L::begin(uint32_t baud) {
    
    pinMode(RESET_PIN, OUTPUT);

    SerialGSM.begin(baud, SERIAL_8N1, RX_PIN, TX_PIN);

    _sleepMode = 0;
    _functionalityMode = 1;

}

void Sim800L::reset()
{
    digitalWrite(RESET_PIN,0);
    delay(1000);
    digitalWrite(RESET_PIN,1);
    delay(1000);
    // wait for the module response

    SerialGSM.println("AT");
    while (true)
    {
        if (_readSerial(1000).indexOf("OK")==-1) SerialGSM.println("AT");
        else break;
    }
    while (_readSerial(1000).indexOf("SMS")==-1 );

    Serial.println("SUCESS RESET");
}

int Sim800L::checkError(String command, int timeout, int error) {
    if (error)
        return error;

    if (timeout < 0)
        timeout = TIME_OUT_READ_SERIAL;

    SerialGSM.println(command);

    if (_readSerial(timeout).indexOf("OK") != -1) {   
        Serial.println("SEND ->: " + command);
        return 0;
    }
    Serial.println("ERROR ->: " + command);
    return 1;
}


/*
 * AT+CFUN=0 Funcionalidade mínima
 * AT+CFUN=1 Funcionalidade completa (padrão)
 * AT+CFUN=4 Flight mode (desativa a função RF)
*/
bool Sim800L::setFunctionalityMode(uint8_t fun) {
    if (fun==0 || fun==1 || fun==4) {
        _functionalityMode = fun;

        switch(_functionalityMode)
        {
        case 0:
            if (!checkError("AT+CFUN=0")) Serial.println("OK");
            break;
        case 1:
            if (!checkError("AT+CFUN=1")) Serial.println("OK");
            break;
        case 4:
            if (!checkError("AT+CFUN=4")) Serial.println("OK");
            break;
        }
    }
    return false;
}
uint8_t Sim800L::getFunctionalityMode() {
    return _functionalityMode;
}

/// GET INFOs ///
String Sim800L::getNumSerie() {    
  SerialGSM.println("AT+CGSN");
  delay(1000);

  while (SerialGSM.available()) {
    String line = SerialGSM.readStringUntil('\n');

    // check if the line starts with a digit
    if (line[0] >= '0' && line[0] <= '9') {
      return line;
    }
  }
  return "ERROR";    
}

float Sim800L::signalQuality() {
    String line = _respostaSerial("AT+CSQ", "+CSQ:", 1000);

    if (line != "ERROR") {
        return line.substring(line.indexOf("+") + 6, line.length()).toFloat();
    }
    else return -100; 
}

String Sim800L::RegistrationStatus() {
    String line = _respostaSerial("AT+CREG?", "+CREG:", 1000);

        if (line != "ERROR") {
            return line.substring(line.indexOf("+") + 7, line.length()-1);
        }
        else return "ERROR"; 
}

String Sim800L::getOperatorsList() {
    // Can take up to 45 seconds
    String line = _respostaSerial("AT+COPS=?", "+COPS:", 15000);

    if (line != "ERROR ->: AT+COPS=?") {
        return line.substring(line.indexOf("+") + 7, line.length());
    }
    else return "ERROR"; 
}

String Sim800L::getOperator() {
    String line = _respostaSerial("AT+COPS?", "+COPS:", 1000);

    if (line != "ERROR ->: AT+COPS?") {
        return line.substring(line.indexOf("+") + 7, line.length());
    }
    else return "ERROR"; 
}


void Sim800L::CONFIG_GPRS(String apn, String apn_u, String apn_p) {
    Serial.println(" --- CONFIG GPRS --- ");

    int error = 0;
    error = checkError("AT+SAPBR=3,1,Contype,GPRS", 2000, error);
    error = checkError("AT+SAPBR=3,1,APN," + apn,   2000, error);
    if (apn_u != "")
    error = checkError("AT+SAPBR=3,1,USER," + apn_u, 2000, error);
    if (apn_p != "") 
    error = checkError("AT+SAPBR=3,1,PWD,"  + apn_p, 2000, error);
    
    if (!error) Serial.println("OK");
}

void Sim800L::Activate_GPRS() {
    Serial.println(" --- Activate GPRS --- ");
    
    int error = 0;
    error = checkError("AT+SAPBR=1,1", 3000, error);
    error = checkError("AT+SAPBR=2,1", 3000, error);
    
    if (!error) Serial.println("OK");
}

void Sim800L::Start_HTTP(String url) {
    Serial.println(" --- Start HTTP --- ");

    int error = 0;
    error = checkError("AT+HTTPINIT", 3000, error);
    error = checkError("AT+HTTPPARA=CID,1", 3000, error);
    error = checkError("AT+HTTPPARA=URL," + url, 3000, error);
    //checkError("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
    //Serial.println(_readSerial(3000));

    if (!error) Serial.println("OK");
}


//This AT+HTTPDATA command configures the size of data to be sent in an HTTP request.
//For Method, possible values are:
//Data size in bytes, waiting time for sending data in milliseconds.
void Sim800L::Send_CONFIG_HTTP(uint16_t DataSize, uint16_t waitingTime, String Data) {//
    Serial.println(" --- Send CONFIG HTTP --- ");

    int error = 0;
    error = checkError("AT+HTTPDATA=" + String(DataSize) + "," + String(waitingTime), -1, error);
    error = checkError(Data, 4000, error);
    
    if (!error) Serial.println("OK");
}



//AT+HTTPACTION AT Command is used perform HTTP actions such HTTP GET or HTTP post. 
//AT+HTTPACTION is a proprietary Simcom AT command.
//For Method, possible values are:
//0:GET
//1:POST
//2:HEAD
String Sim800L::Send_HTTP(uint8_t method) {
    Serial.println(" --- SEND HTTP --- ");

    checkError("AT+HTTPACTION="+String(method), 4000);
    SerialGSM.println("AT+HTTPREAD");

    return _readSerial(8000);
}
//Error 603, not enough credits

void Sim800L::Close_Connection() {
    Serial.println(" --- Close Connection --- ");

    int error = 0;
    error = checkError("AT+HTTPTERM", 4000, error);
    error = checkError("AT+SAPBR=0,1", 4000, error);

    if (!error) Serial.println("OK");
}


String Sim800L::RTCtime() { 

    SerialGSM.println("at+cclk?");
    String data = _readSerial(3000);

    if ((data.indexOf("ERRO"))!=-1) return "ERROR";

    data =              data.substring(data.indexOf("\"")+1,data.lastIndexOf("\"")-1);
    DateTime.year =     data.substring(0,2).toInt();
    DateTime.month =    data.substring(3,5).toInt();
    DateTime.day =      data.substring(6,8).toInt();
    DateTime.hour =     data.substring(9,11).toInt();
    DateTime.minute =   data.substring(12,14).toInt();
    DateTime.second =   data.substring(15,17).toInt();

    return "OK";
}

String Sim800L::dateNet() {

    String line = _respostaSerial("AT+CIPGSMLOC=2,1", "+CIPGSMLOC:", 3000);

    if (line == "ERROR") return "ERROR";

    if (line.indexOf("601") != -1) return "0";
    
    return line.substring(line.indexOf(":")+2,(line.indexOf("OK")-4));
}

void Sim800L::updateRtc(String apn, String apn_u, String apn_p, String utc) {

    String date = dateNet();

    if (date == "0") {
        CONFIG_GPRS(apn, apn_u, apn_p);
        Activate_GPRS();
        date = dateNet();
        Close_Connection();
    }

    date = date.substring(date.indexOf(",")+1,date.length() - 1);
    date = date.substring(2, date.length());
    date += utc;
    Serial.println(date);

    SerialGSM.println("AT+CCLK=\""+date+"\"");
    Serial.println(_readSerial(3000));
}

//Format: YY/MM/DD,hh:mm:ss
void Sim800L::updateRtcManual(String DateTime, String utc) {
    SerialGSM.println("AT+CCLK=\""+DateTime+utc+"\"");
    Serial.println(_readSerial(3000));
}

void Sim800L::sendSMS(String number, String text) {
    
    SerialGSM.println("AT+CMGF=1"); // Configuring TEXT mode
    Serial.println(_readSerial(1000));
    SerialGSM.println("AT+CMGS=\""+number+"\"");
    Serial.println(_readSerial(1000));
    SerialGSM.println(text);
    Serial.println(_readSerial(1000));
    SerialGSM.write(26);
    Serial.println(_readSerial(1000));

    // Serial.println(_buffer);
    //expect CMGS:xxx   , where xxx is a number,for the sending sms.
    Serial.println("OK");
}

void Sim800L::callNumber(String number) {

    SerialGSM.println("ATD"+number+";");
    Serial.println(_readSerial(1000));
}

//
//PRIVATE METHODS
//
String Sim800L::_readSerial(uint32_t timeout)
{
    delay(timeout);

    String str;

    while(SerialGSM.available())
    {
        if (SerialGSM.available()>0)
        {
            str += (char) SerialGSM.read();
        }
    }
    return str;
}

String Sim800L::_respostaSerial(String Command, String RespStart, uint32_t timeout) {

    SerialGSM.println(Command);
    uint64_t timeOld = millis();

    delay(timeout);

    while (SerialGSM.available()) {
        String line = SerialGSM.readStringUntil('\n');

        if (line.startsWith(RespStart)) {
            return line;
        }
    }
    return "ERROR";
}