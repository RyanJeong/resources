#define TEST "TEST"

#define __CRC__                 "[crc.ino]"
#define CRC_INITCRC             "[initCRC()] "
#define CRC_CHECKCRC            "[checkCRC()] "
#define CRC_UPDATECRC           "[updateCRC()] "
#define CRC_CHECKCRCISINVALID   "[checkCRCIsInvalid()] "
#define SIZE_BUF    64

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    static const char   *func = CRC_UPDATECRC;
    uint8_t             crc;
    uint8_t             i;
    char                _buf[SIZE_BUF];

    putsForDebug(__CRC__, func, TEST);
    putsForDebug(__CRC__, func, "TEST");

    delay(1000);
}

void putsForDebug(const char *file, const char *func, const char *msg)
{
    Serial.print(file);
    Serial.print(func);
    Serial.println(msg);
    Serial.flush();

    return;
}
