#include <avr/boot.h>
#include "kem.h"
#include "util.h"

/*
 *  serialId[SIZE_ID]   : thing의 고유 값을 저장하는 배열
 */
uint8_t serialId[SIZE_ID];

void getSerialId()
{
#ifdef SOFTWARESERIAL_MODE
    getSerialId1();
#else
    getSerialId2();
#endif

    return;
}

#ifdef SOFTWARESERIAL_MODE
void getSerialId1()
{
    static const char *func = UTIL_GETSERIALID1;
    size_t  i;

    putsForDebug(__UTIL__, func, MSG_START);
    for (i = 0; i < SIZE_ID; ++i) {
        serialId[i]         = (uint8_t) boot_signature_byte_get(0x0E + i + (((SIZE_ID == 9) && (i > 5)) ? 1 : 0));
    }

    return;
}

#else
void getSerialId2()
{
    size_t i;

    for (i = 0; i < SIZE_ID; ++i) {
        serialId[i]         = (uint8_t) boot_signature_byte_get(0x0E + i + (((SIZE_ID == 9) && (i > 5)) ? 1 : 0));
    }

    return;
}
#endif

void turnOnLED(void)
{
/*  TODO    */

    return;
}

#ifdef SOFTWARESERIAL_MODE
void putsForDebug(const char *file, const char *func, char *msg)
{
    Serial.print(file);
    Serial.print(func);
    Serial.println(msg);

    return;
}
#endif
