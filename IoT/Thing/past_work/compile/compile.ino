#define ENABLE_RF               \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x03);  \
} while (0)

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    funcC();
}

void funcA()
{
#ifdef A
    Serial.println("Hello, world!");

    return;
#endif
}

void funcB()
{
    /*
    Serial.println("Hello, world!");

    return;
    */
}

void funcC()
{
    Serial.print((char) 0xAB); 
    Serial.print((char) 0x03); 

    return;
}

