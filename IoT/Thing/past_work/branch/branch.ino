#define TEST

#undef TEST
void setup()
{
    Serial.begin(9600);
#ifndef TEST
    exit(0);
#endif
}

void loop()
{
    Serial.println("Loop()");
    delay(1000);
}
