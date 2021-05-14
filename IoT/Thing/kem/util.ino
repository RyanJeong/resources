#include <avr/boot.h>
#include "util.h"
#include "rf.h"         /*  DISABLE_RF  */
#include "sensor.h"     /*  ENABLE_ADC, DISABLE_ADC */
#include "kem.h"        /*  tempDbg[SIZE_BUF]   */
#include "interrupt.h"  /*  sleep() */

/*
 *  tempDbg[SIZE_BUF](kem.ino)  : Debug message를 담는 배열
 * 
 *  serialId[SIZE_ID]           : thing의 고유 값을 저장하는 배열
 */
extern char tempDbg[SIZE_BUF];

uint8_t     serialId[SIZE_ID];

void getSerialId(void)
{
    uint8_t i;

    for (i = 0; i < SIZE_ID; ++i) {
        serialId[i] = (uint8_t) boot_signature_byte_get(0x0E + i + (((SIZE_ID == 9) && (i > 5)) ? 1 : 0));
    }

    return;
}

void checkBattery(void)
{
    static char *func = "[void checkBattery(void)]";
    char        tempStr[SIZE_BUF];
    float       voltage;
    
    ENABLE_ADC;
    voltage = (0.04882812 * analogRead(VCC_CHECK));    /*   10.0 * 5.0 / 1024.0 */
    DISABLE_ADC;
    dtostrf(voltage, 4, 2, tempStr);
    sprintf(tempDbg, "Voltage: %s V", tempStr);
    putsForDebug(__UTIL__, func, tempDbg);
    if (voltage < VOLTAGE_THRESHOLD) {
        sprintf(tempDbg, "%x", MSG_LOW_BATTERY_VOLTAGE);
        putsForDebug(__UTIL__, func, MSG_LOW_BATTERY_VOLTAGE);
    }
    handleBuzzerAndLED(STATE_CHECK_BATTERY);

    return;
}

void handleBuzzerAndLED(uint8_t op)
{
    digitalWrite(STATUS1, (op & 01) ? HIGH : LOW);
    op >>= 1;
    digitalWrite(STATUS0, (op & 01) ? HIGH : LOW);
    op >>= 1;
    digitalWrite(BUZZER, (op & 01) ? HIGH : LOW);
    sleep(MILSEC_125);
    digitalWrite(STATUS1, LOW);
    digitalWrite(STATUS0, LOW);
    digitalWrite(BUZZER, LOW);

    return;
}

void putsForDebug(const char *file, const char *func, const char *msg)
{
#ifndef OFF_DEBUG_MSG
    DISABLE_RF;
    Serial.print('\n');
    Serial.print('\r');
    Serial.print(file);
    Serial.print(func);
    Serial.println(msg);
    Serial.flush();
    delay(32);
#endif

    return;
}
