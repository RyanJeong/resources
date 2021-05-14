#include <avr/boot.h>
#include "kem.h"
#include "util.h"

/*
 *  serialId[SIZE_ID]   : thing의 고유 값을 저장하는 배열
 */
uint8_t serialId[SIZE_ID];

void getSerialId()
{
    uint8_t i;

    for (i = 0; i < SIZE_ID; ++i) {
        serialId[i]         = (uint8_t) boot_signature_byte_get(0x0E + i + (((SIZE_ID == 9) && (i > 5)) ? 1 : 0));
    }

    return;
}

void turnOnLED(void)
{
//  TODO

    return;
}

/*
 *  Do not remove '#ifdef SOFTWARESERIAL_MODE'
 *  SoftwareSerial이 아닌 상태에서 사용하는 것을 보호하기 위함
 */
#ifdef SOFTWARESERIAL_MODE
void putsForDebug(const char *file, const char *func, char *msg)
{
    Serial.print(file);
    Serial.print(func);
    Serial.println(msg);

    return;
}
#endif
