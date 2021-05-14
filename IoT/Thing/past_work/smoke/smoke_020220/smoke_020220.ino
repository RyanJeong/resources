#define ABS(x)      (x * ((x > 0) - (x < 0)))

#define port        A0      // smoke sensor analog input   
#define STATUS0     A2      // LED 1
#define STATUS1     A4      // LED 2 

#define BUZZER      A5      // sound 

#define IR_TX       2       // D2: IR LED TX port
#define Shutter     15      // D15 광학셔터 개폐용 출력 포트 

#define RELAY           A3   // relay 구동 포트 
#define VCC_CHECK       A6   // 베터리 전압 체크 아나로그 입력 포트 

#define deltaTime       40   // [us]
#define samplingTime    290  // IR LED ON time [us]
#define sleepTime       9670 // [us]

# define initial_voltage 0.5    // initialize 
//counter------------------------------------------------------

int fire_detection_lapse = 2;    // 2: 화재 지속 판정 유지 time [2 second]
int reset_lapse_counter  = 5;    // 60:  초기 power ON 시 화재 비감지 시간 [60 second]

//flag---------------------------------------------------------
int reset_counter = 0;    // 전원 ON reset 시 initial reset counter
int detect_counter = 0;   // fire 발생 감지 지속 시간 측정 카운터
int fire_alarm = 0;       // 화재 통보 플레그
int fire_decision = 0;    // 화재 발생 판정 플레그

int mode = 0;             // surveillance=0 & test=1
float alpha = 0.6;        // 노이즈 필터 계수
float beta  = 1.0 - alpha; // 노이즈 필터 계수

float filter_out;
float data_adc;
float smoke_voltage;             //  Smoke 센서 ADC값을 5V로 변환

float past_threshold  = initial_voltage;      // 전원 ON 초기치 평균값
float smoke_threshold = initial_voltage;     // 전원 ON 초기치 평균값
float smoke_threshold_high = initial_voltage;  // [volt]  smoke threshold voltage maximum
float past_data       = initial_voltage;

float smoke_detection_voltage = 0.02;     // [Volt] smoke voltage rising difference
float noise_threshold         = 0.015;    // [Volt] for noise voltage maximum reference

float OFF_threshold = initial_voltage + 0.1;             // 0.5 [Volt]: sensor OFF test mode voltage difference
float ON_threshold  = initial_voltage + 1.0;             // 1.5 [Volt]: sensor ON test mode voltage difference

int millisecond;
int smoke_detection = 0;
//-------------------------
void shutter_on()
{
  digitalWrite(Shutter, HIGH);
}

//-------------------------
void shutter_off()
{
  digitalWrite(Shutter, LOW);
}

//-------------------------
void LED_ON()
{
  digitalWrite(IR_TX, HIGH);
}

//-------------------------
void LED_OFF()
{
  digitalWrite(IR_TX, LOW);
}
//------------------------
float smoke_sampling()                    // adc sampling data
{
  digitalWrite(IR_TX, LOW);
  digitalWrite(IR_TX, HIGH);              // IR LED ON
  delayMicroseconds(samplingTime);        // samplingTime(280) 마이크로 초만큼 지연

  float data_adc = analogRead(A0);        // 아날로그 값

  delayMicroseconds(deltaTime);           // deltaTime(40) 마이크로 초만큼 지연
  digitalWrite(IR_TX, LOW);               // IR LED OFF
  delayMicroseconds(sleepTime);           // sleepTime(9680) 마이크로 초만큼 지연
  return data_adc;
}
//-------------------------
void out_of_order()       // 고장 통보
{
  Serial.print(" sensor damage ");
  Serial.println("  ");
  //digitalWrite(BUZZER, HIGH);    // alarm sound
  digitalWrite(STATUS0, HIGH);   // LED
  digitalWrite(STATUS1, HIGH);   // LED
  delay(10);
  //digitalWrite(BUZZER, LOW);
  digitalWrite(STATUS0, LOW);    // LED
  digitalWrite(STATUS1, LOW);    // LED
  delay(10);
}
//-------------------------
void low_battery_alarm()     // 베터리 체크
{
  Serial.print(" low battery alarm ");
  //digitalWrite(BUZZER, HIGH);    // alarm sound
  digitalWrite(STATUS0, HIGH);   // LED
  digitalWrite(STATUS1, HIGH);   // LED
  delay(10);
  // digitalWrite(BUZZER, LOW);
  digitalWrite(STATUS0, LOW);    // LED
  digitalWrite(STATUS1, LOW);    // LED
  delay(10);
}
//-------------------------
void fire_alarm_sound()       // 화재 경고
{
  Serial.print(" fire alarm ");
  Serial.println("  ");
  //digitalWrite(BUZZER, HIGH);
  digitalWrite(STATUS0, HIGH);
  digitalWrite(STATUS1, HIGH);
  delay(200);
  //digitalWrite(BUZZER, LOW);
  digitalWrite(STATUS0, LOW);
  digitalWrite(STATUS1, LOW);
  delay(10);
}
//-------------------------
void power_on_reset()          // 처음 전원을 켜면 일정시간 무동작 : 60 seconds
{
  float alpha = 0.6;
  float beta  = 1.0 - alpha;
  past_data   = initial_voltage;          // 연기 초기치
  smoke_threshold = initial_voltage;      // 연기 초기치

  for (int mmm = 0; mmm <= reset_lapse_counter ; mmm++) // 60초 지연
  {
    data_adc = smoke_sampling();           // 아날로그 값을 받아 Smoke_input에 저장

    smoke_voltage = data_adc * (5.0 / 1024.0);
    filter_out = alpha * smoke_voltage + beta * past_data; // IIR noise filtering
    past_data = filter_out;
    delay(1000);          // 1 second sample delay

  }

}    // power on reset

//-----------------------------
void battery_check()          //
{
  float battery_voltage = 10 * (5.0 / 1024.0) * analogRead(VCC_CHECK); // battery voltage read
  Serial.println(" ");
  Serial.print(battery_voltage);
  Serial.println(" [volt]");
  if ( battery_voltage < 4.5 ) {                // 5.5 V
    Serial.println(" Low battery voltage ");
    Serial.println("  ");
    //low_battery_alarm();    // battery alarm
  }
  //Serial.println(" ");
}    // battery check

//-------------------------
void setup() {
  Serial.begin(9600);

  pinMode(3, INPUT);  // D3  setup switch
  pinMode(4, INPUT);  // D4
  pinMode(5, INPUT);  // D5
  pinMode(6, INPUT);  // D6
  pinMode(7, INPUT);  // D7
  pinMode(8, INPUT);  // D8
  pinMode(9, INPUT);  // D9
  pinMode(10, INPUT);  // D10

  pinMode(IR_TX, OUTPUT);    // D2 :IR LED
  pinMode(15, OUTPUT);       // D15 :shutter

  pinMode(STATUS0, OUTPUT);
  digitalWrite(STATUS0, LOW);
  pinMode(STATUS1, OUTPUT);
  digitalWrite(STATUS1, LOW);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  pinMode(IR_TX, OUTPUT);
  digitalWrite(IR_TX, LOW);
  pinMode(Shutter, OUTPUT);
  digitalWrite(Shutter, LOW);

  // smoke_voltage   = 0.0;    // initialize
  // smoke_threshold = 0.25;    // initialize

  //   TCCR2A |= (1<<WGM21);            // fast pwm, ocr top
  //   TCCR2A |= (1<<WGM20);
  //   TCCR2B |= (1<<WGM22);
  //   TCCR2B |= (1<<CS22);
  //   TCCR2B |= (1<<CS20);
  //   TIMSK2 |= (1<<TOIE2);
  //   OCR2A=124;

  battery_check();
  power_on_reset();

}
//-------------------------


//------------------------------------------------------------------------------------------
//
//
//
//
//------------------------------------------------------------------------------------------
void loop() {         // mode == 0 : 화재 감시 , mode == 1 : 테스트



  mode = 0;


  //------------------------------------------------------------

  if (mode == 0)   //화재 감시모드 : surveillance  : IIR filter
  {

    data_adc = smoke_sampling();           // 아날로그 값을 받아 Smoke_input에 저장

    smoke_voltage = data_adc * (5.0 / 1024.0); // [volt]
    filter_out = alpha * smoke_voltage + beta * past_data; // IIR noise filtering
    past_data = filter_out;                    //

    // average smoke voltage 계산 : IIR SUM  -------------

    Serial.print(smoke_voltage, 4);
    Serial.print("    ");
    //Serial.print(filter_out,4);
    //Serial.print("    ");
    Serial.print(smoke_threshold, 4);
    Serial.print("    ");
    //Serial.print((filter_out - smoke_threshold), 4);
    //Serial.print("    ");
    Serial.println(detect_counter);
    Serial.println("     ");

    //------------------------------------------------------------------------------------
    // detection voltage calculation -----------------------------------------------------
    smoke_detection = 0;
    float abs_diff = abs(filter_out - past_threshold);   // abs

    if  ( filter_out > (past_threshold + noise_threshold) )  // noise input signal 제거
    {
      float alpha_threshold = 0.01;
      smoke_detection = 1;
      if (fire_decision == 1)      // fire detection
      {
        alpha_threshold = 0.0;
      }
      smoke_threshold = alpha_threshold * filter_out + (1 - alpha_threshold) * past_threshold;
    }
    else if ( abs_diff <= noise_threshold )
    {
      float alpha_threshold = 0.3;
      if (fire_decision == 1)
      {
        alpha_threshold = 0.0;
      }
      smoke_threshold = alpha_threshold * filter_out + (1.0 - alpha_threshold) * past_threshold;
    }
    else
    {
      float alpha_threshold = 0.1;
      smoke_threshold = alpha_threshold * filter_out + (1.0 - alpha_threshold) * past_threshold;
    }

    if (smoke_threshold > smoke_threshold_high) // lower voltage input then reference
    {
      smoke_threshold = smoke_threshold_high;
    }
    past_threshold  = smoke_threshold;

    //---------------------------------------------------------------------------------
    // check fire detection -----------------------------------------------------------
    float diff_smoke = filter_out - smoke_threshold;

    if  (diff_smoke > smoke_detection_voltage)
    {
      detect_counter = detect_counter + 1;          // smoke 연속 감지
    }
    else {
      detect_counter = detect_counter - 1;          // smoke 연속 감지 안됨
    }
    //  -------------------------------------------------
    if (detect_counter > fire_detection_lapse)       // 화재 온도 일정 시간 이상 감지
    {
      detect_counter = fire_detection_lapse;
      fire_decision = 1;                             // 화재 발생 판정
    }
    else if (detect_counter < 0)
    {
      detect_counter = 0;
      fire_decision  = 0;
    }
    //----------------------------------------------------

    if ((fire_decision == 1) && (detect_counter > 0))     // 이미 화재 발생 상태이면
    {
      fire_alarm = 1;                                   // 화재 알림 통보
      fire_alarm_sound();
      //digitalWrite(RELAY, HIGH);                        // RELAY ON
    }
    else
    {
      fire_alarm = 0;
      //digitalWrite(RELAY, LOW);
    }

  }   // fire surveillance mode = 0 -------------------------------------------------


  if ( (detect_counter > 0) || (smoke_detection == 1) )
  {
    delay(1000);   // 1 second sample delay
  }
  else
  {
    delay(5000);   // 5 second sample delay
  }

  battery_check();

  //------------------------------------------------------------------------------------------
  //
  //
  //
  //
  //
  //
  // ---- smoke sensor test mode -------------------------------------------------------------------
  if (mode == 1)   //  smoke test mode
  {

    OFF_threshold = smoke_threshold + 0.5;
    ON_threshold  = smoke_threshold + 1.0;
    
    Serial.println("  ");
    Serial.println(" test mode ");
    
    //------------ 1
    shutter_off();
    LED_OFF();
    delay(100);   // settling time
    data_adc = analogRead(port) * (5.0 / 1024.0);            // arduino data read
    
    Serial.print("1  ");
    Serial.println(data_adc);

    if (data_adc > OFF_threshold)
    {
      out_of_order();
    }
    delay(100);   // settling time

    //------------ 2
    shutter_off();
    //LED_ON();
    delay(300);   // settling time
    data_adc = smoke_sampling() * (5.0 / 1024.0);            // arduino data read

    Serial.print("2  ");
    Serial.println(data_adc);

    if (data_adc > OFF_threshold)
    {
      out_of_order();
    }
    delay(100);   // settling time
    //------------ 3 ----ON--------------------------------

    shutter_on();
    LED_OFF();
    delay(500);  // settling time
    data_adc = analogRead(port) * (5.0 / 1024.0);            // arduino data read

    Serial.print("3  ");
    Serial.println(data_adc);

    if (data_adc > OFF_threshold)
    {
      out_of_order();
    }
    delay(100);  // settling time

    //------------ 4
    //shutter_on();

    //LED_ON();
    delay(300);  // settling time
    data_adc = smoke_sampling() * (5.0 / 1024.0);            // arduino data read

    Serial.print("4  ");
    Serial.println(data_adc);

    if (data_adc < ON_threshold)
    {
      out_of_order();
    }
    delay(100);   // settling time

    //------------
    shutter_off();
    LED_OFF();
    delay(500);   // settling time for shutter
    //------------


    digitalWrite(BUZZER, HIGH);
    digitalWrite(STATUS0, HIGH);
    digitalWrite(STATUS1, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    digitalWrite(STATUS0, LOW);
    digitalWrite(STATUS1, LOW);
    delay(1000);

    //digitalWrite(RELAY, HIGH);
    //delay(500);
    //fire_alarm_sound();
    //digitalWrite(RELAY, LOW);

    //delay(500);
  } //-- sensor test mode -------------------------------------------------

  digitalWrite(STATUS0, HIGH);
  digitalWrite(STATUS1, HIGH);
  delay(200);

  digitalWrite(STATUS0, LOW);
  digitalWrite(STATUS1, LOW);
  delay(1);

} //----------------------- loop ---------------------------------------------
