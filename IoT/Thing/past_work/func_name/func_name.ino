void loop()
{
    Serial.begin(9600);
}

void loop()
{
    A();
    B();
    C();
    delay(1000);
}

void A()
{
    Serial.println(__FUNC__);
}

void B()
{
    Serial.println(__FUNC__);
}

void C()
{
    Serial.println(__FUNC__);
}
