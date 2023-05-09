#include "Arduino.h"

#define DEFAULT_RX_PIN 		16
#define DEFAULT_TX_PIN 		17
#define DEFAULT_RESET_PIN 	32

#define DEFAULT_BAUD_RATE		9600
#define TIME_OUT_READ_SERIAL	1000


class Sim800L {
    private:

        uint32_t _baud;
        bool _sleepMode;
        uint8_t _functionalityMode;

        String _readSerial(uint32_t timeout);
        String _respostaSerial(String Command, String RespStart, uint32_t timeout);

    public:

        uint8_t	RX_PIN;
        uint8_t TX_PIN;
        uint8_t RESET_PIN;

        struct DateTimeStruct {
            String  year;
            String  month;
            String  day;
            String  hour;
            String  minute;
            String  second;
        };
        DateTimeStruct DateTime;

        Sim800L(uint8_t rx = DEFAULT_RX_PIN, uint8_t tx = DEFAULT_TX_PIN, uint8_t rst = DEFAULT_RESET_PIN);

        void begin(uint32_t baud = 9600);
        void reset();
        int checkError(String command, int timeout = TIME_OUT_READ_SERIAL, int error = 0);
        bool getSleepMode();
        bool setFunctionalityMode(uint8_t fun);
        uint8_t getFunctionalityMode();

        String getNumSerie();
        float signalQuality();
        String RegistrationStatus();
        String getOperator();
        String getOperatorsList();

        void CONFIG_GPRS(String apn, String apn_u, String apn_p);
        void Activate_GPRS();
        void Start_HTTP(String url);

        void Send_CONFIG_HTTP(uint16_t DataSize, uint16_t waitingTime, String Data);
        String Send_HTTP(uint8_t method);
        void Close_Connection();

        String RTCtime();
        String dateNet();
        void updateRtc(String apn, String apn_u, String apn_p, String utc);
        void updateRtcManual(String DateTime, String utc);

        void sendSMS(String number, String text);
        void callNumber(String number);
        
};