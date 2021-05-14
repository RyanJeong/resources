void setup()
{
    Serial.begin(9600);
}

void loop()
{
    int a;
    
    a = 0;
    
    Serial.println(a);
    a |= 0x02;
    Serial.println(a);
    a |= (0x04 + 1);
    Serial.println(a);

    delay(5000);
}
