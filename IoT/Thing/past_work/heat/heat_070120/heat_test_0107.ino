//--------------------------------------------------------------//
//   2020 / 1 / 7 
//   heat temperature detector 
//   KEM Co. + DAEJIN University
//   Electronic Engineering Department 
//   정온식 감지기 
//   감지기 센서 2개 
//   감지 & 테스트 모드 
//--------------------------------------------------------------//
#define ABS(x)      (x * ((x > 0) - (x < 0)))

#define Q1          (0.29476)         /*   */
#define Q2          (-4.5725)         /*   */
#define Q3          (33.54)           /*   */
#define Q4          (-50.2612)

#define HEAT_TEST       2    /* D2:  TR 열 발생 핀 2번  */
#define STATUS0         A2   // led 1
//#define VCC_CHECK       A3   // analog voltage in 
#define STATUS1         A4   // led 2
#define BUZZER          A5   // warning sound 
//#define RELAY           A6   // relay

#define RELAY           A3   // relay
#define VCC_CHECK       A6   // analog voltage in 


float pastValue1 = 25.0;  /*  전원 인가 시 감지기 초기 가정 온도 */
float pastValue2 = 25.0;  /*  전원 인가 시 감지기 초기 가정 온도 */
float temp1;              /*  a value which get from ADC  */
float temp2;              /*  a value which get from ADC  */
float out_temp1;
float out_temp2;
float filtered_data1;
float filtered_data2;

float ALPHA = 0.7;             /*  Noise filter 계수 */
float BETA  = 1.0-ALPHA;
//counter------------------------------------------------------
int sample_rate = 1;                        // 1: sample number in one second      
int fire_detection_lapse = 2*sample_rate;   // 2: 화재 지속 판정 유지 time [2 second] 
int reset_lapse_counter  = 5*sample_rate;  //  60: 초기 power ON 시 화재 비감지 시간 [60 second] 
 
// count flag---------------------------------------------------------
int reset_counter =0;     // 전원 ON reset 시 initial reset counter 
int detect_counter=0;     // fire 발생 감지 지속 시간 측정 카운터   
int fire_alarm    =0;     // 화재 통보 
int fire_decision =0;     // 화재 발생 판정  


int mode = 0;                       // surveillance=0 & test=1 
float fire_threshold_temp  = 30;    // fire alarm temperature   [ 60 ~ 80 C]
float fire_rise_warning    = 28;    // temperature rise warning [ 60 ~ 80 C]-10
float fire_threshold_temp_low;
float normal_threshold  = 3.0;      // ~ 2 degree :  for cooling test 
float heating_threshold = 0.5;      // ~ 4 degree :  for heating test
float data_in;


//-------------------------
void power_on_reset()          //  처음 전원을 켜면 일정시간 무동작 
{
  for(int mmm = 1; mmm <= reset_lapse_counter ; mmm++)  
  {
    data_in = analogRead(A0);     //  arduino dummy data read 
    fire_alarm = 0;
    delay(1000);   // one second delay time 
  }  
}

//-----------------------------
void battery_check()   // battery 입력 전압 확인
{ 
    float battery_voltage = 10.0*5.0*analogRead(VCC_CHECK)/1024.0;   // battery voltage read
    Serial.println(" ");
    Serial.print(battery_voltage);
    Serial.println(" [volt]");
    
    if ( battery_voltage < 4.5 ) {                // 5.5 V
        Serial.println(" Low battery voltage "); 
        Serial.println("  ");

        digitalWrite(STATUS0, HIGH);  
        delay(10);
        digitalWrite(STATUS0, LOW);  
     }
 
} // battery check

//-------------------------
void setup() {
  Serial.begin(9600);
  
  pinMode(3, INPUT);  // D3 : setting connector
  pinMode(4, INPUT);  // D4 : setting connector
  pinMode(5, INPUT);  // D5 : setting connector
  pinMode(6, INPUT);  // D6 : setting connector
  pinMode(7, INPUT);  // D7 : setting connector
  pinMode(8, INPUT);  // D8 : setting connector
  pinMode(9, INPUT);  // D9 : setting connector
  pinMode(10, INPUT); // D10 : setting connector
                    
  pinMode(HEAT_TEST, OUTPUT);
  digitalWrite(HEAT_TEST, LOW);   
  pinMode(STATUS0, OUTPUT);
  digitalWrite(STATUS0, LOW);  
  pinMode(STATUS1, OUTPUT);
  digitalWrite(STATUS1, LOW);  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);  
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);  
    
  battery_check();
  power_on_reset();
}

//---------------------------------------------------------------
//
//  mode == 0 : 화제감시 , mode == 1 : 테스트
//---------------------------------------------------------------
void loop() {         // 

    //------------------

      mode = 0;


    //------------------------------------------------------------
    if (mode == 0)   // 화제감시모드 : surveillance                      
    {
      temp1     = (float) analogRead(A0)/100.0;         // adc 입력값 0~1023 의 크기를 1/100 로 줄여서 입력              
      out_temp1 = Q1*(temp1)*(temp1)*(temp1) + Q2*temp1*(temp1) + Q3*temp1 + Q4;    // adc to temperature 
    
      temp2     = (float) analogRead(A1)/100.0;
      out_temp2 = Q1*(temp2)*(temp2)*(temp2) + Q2*(temp2)*(temp2) + Q3*temp2 + Q4;    // adc to temperature 
      
      filtered_data1 = (ALPHA * out_temp1) + (BETA * pastValue1);  /*  노이즈 제거부 */
      pastValue1 = filtered_data1;                                 // 센서 1

      filtered_data2 = (ALPHA * out_temp2) + (BETA * pastValue2);  /*  노이즈 제거부 */
      pastValue2 = filtered_data2;                                 // 센서 2
      
      Serial.print(filtered_data1); // heat sensor 
      Serial.print("  ");
      Serial.print(filtered_data2); // tr sensor 
      Serial.print("  "); 
      Serial.print(detect_counter); // tr sensor 
      Serial.println("  "); 
      
      // check fire -----------------------------------------------------------
      fire_threshold_temp_low = fire_threshold_temp;  
    
      if(detect_counter > 0)                                //화재 감지가 된 경우
      {
        fire_threshold_temp_low = fire_threshold_temp - 1; // 1 degree noise margin 
      }

      if (filtered_data1 > fire_threshold_temp_low)         //센서 1: 화재 감지가 된 경우 
          detect_counter = detect_counter+1;                //화재 온도 연속 감지
      else
          detect_counter = detect_counter-1;                //화재 온도 연속 감지 안됨 

      if (detect_counter > fire_detection_lapse)       //화재 온도 일정 시간 이상 감지 
      {
          detect_counter = fire_detection_lapse;
          fire_decision = 1;                                //화재 발생 판정  
      }
      else if (detect_counter < 0)
      {
          detect_counter = 0;
          fire_threshold_temp_low = fire_threshold_temp;
          fire_decision = 0;   
      }

      //-----------
      if ((fire_decision == 1) && (detect_counter > 0))     // 이미 화재 발생 상태이면 
      { 
          fire_alarm = 1;                                   // 화재 알림 통보 
                        Serial.print(" fire alarm !! ");
                        digitalWrite(BUZZER, HIGH);
                        digitalWrite(RELAY, HIGH);
                        digitalWrite(STATUS0, HIGH);  
                        digitalWrite(STATUS1, HIGH);  
                        delay(100);
                        digitalWrite(BUZZER, LOW); 
                        //digitalWrite(RELAY, LOW); 
                        digitalWrite(STATUS0, LOW);  
                        digitalWrite(STATUS1, LOW);   
                        delay(10);  
      } 
      else 
      {
            fire_alarm = 0;
            digitalWrite(RELAY, LOW); 
      }
          
       
 } // fire surveillance mode -------------------------------------------------


      digitalWrite(STATUS0, HIGH);  
      digitalWrite(STATUS1, HIGH);
      delay(10);
      digitalWrite(STATUS0, LOW); 
      digitalWrite(STATUS1, LOW);
      
      if ((detect_counter > 0) || (filtered_data1 > fire_rise_warning))
      {
          delay(1000);       // 1 second sample delay 
      }
      else 
      {
          delay(5000);   // 5 second sample delay 
      }
       
      battery_check();

//----------------------------------------------------------------
//
//
//
//
//----------------------------------------------------------------
      
    if (mode == 1)   // test mode
      {
        //-----------------normal-------------------------------

            float sensor_temp[3];
            float heater_temp[3];
            
            digitalWrite(HEAT_TEST, LOW);        // heater off
            
            temp1     = (float) analogRead(A0)/100.0;   // adc 입력값 0~1023 의 크기를 1/100 로 줄여서 입력           
            sensor_temp[0] = Q1*(temp1)*(temp1)*(temp1) + Q2*temp1*(temp1) + Q3*temp1 + Q4;    // adc to temperature 
    
            temp2     = (float) analogRead(A1)/100.0;
            heater_temp[0] = Q1*(temp2)*(temp2)*(temp2) + Q2*(temp2)*(temp2) + Q3*temp2 + Q4;    // adc to temperature 

            float diff_value = abs(sensor_temp[0]-heater_temp[0]);
            if ( diff_value > normal_threshold )
              {
                //call out_of_order;  // 고장 통보
                Serial.println(" sensor normal test : damaged ");
                        digitalWrite(BUZZER, HIGH);
                        digitalWrite(STATUS0, HIGH);  
                        digitalWrite(STATUS1, HIGH);  
                        delay(10);
                        digitalWrite(BUZZER, LOW); 
                        digitalWrite(STATUS0, LOW);  
                        digitalWrite(STATUS1, LOW);  
                        delay(10); 
              }

            // heating ----------------------------  
            if ( heater_temp[0] < 45.0 ) {        //   heater 과열 감지 : 45도 
                digitalWrite(HEAT_TEST, HIGH);    //   heater on
            }
            else {
                Serial.println(" sensor over heating  ");
            }
            
            delay(5000);                      //   5 second heating 
            
            temp1     = (float) analogRead(A0)/100.0;            // adc 입력값 0~1023 의 크기를 1/100 로 줄여서 입력 
            sensor_temp[1] = Q1*(temp1)*(temp1)*(temp1) + Q2*temp1*(temp1) + Q3*temp1 + Q4;    // adc to temperature 
    
            temp2     = (float) analogRead(A1)/100.0;
            heater_temp[1] = Q1*(temp2)*(temp2)*(temp2) + Q2*(temp2)*(temp2) + Q3*temp2 + Q4;    // adc to temperature 

            delay(5000);                      //   1 second heating
            
            temp1     = (float) analogRead(A0)/100.0;               // adc 입력값 0~1023 의 크기를 1/100 로 줄여서 입력 
            sensor_temp[2] = Q1*(temp1)*(temp1)*(temp1) + Q2*temp1*(temp1) + Q3*temp1 + Q4;    // adc to temperature 
    
            temp2     = (float) analogRead(A1)/100.0;
            heater_temp[2] = Q1*(temp2)*(temp2)*(temp2) + Q2*(temp2)*(temp2) + Q3*temp2 + Q4;    // adc to temperature 

            digitalWrite(HEAT_TEST, LOW);   // heat off
            digitalWrite(HEAT_TEST, LOW);

            Serial.print(sensor_temp[2]);
            Serial.print("  ");
            Serial.print(heater_temp[0]);
            Serial.print("  ");
            Serial.print(heater_temp[1]);
            Serial.print("  ");
            Serial.print(heater_temp[2]);
            Serial.println("  ");
            
            if ( (heater_temp[1] - heater_temp[0]) < heating_threshold )
            {
                //call out_of_order;  // 고장 통보 
                Serial.println(" sensor heating test 1: damaged  ");
                        digitalWrite(BUZZER, HIGH);
                        digitalWrite(STATUS0, HIGH);  
                        digitalWrite(STATUS1, HIGH);  
                        delay(50);
                        digitalWrite(BUZZER, LOW); 
                        digitalWrite(STATUS0, LOW);  
                        digitalWrite(STATUS1, LOW);  
                        delay(50);   
            }

            if ( (heater_temp[2] - heater_temp[1]) < heating_threshold )
            {
                //call out_of_order;  // 고장 통보 
                Serial.println(" sensor heating test 2: damaged  ");
                        digitalWrite(BUZZER, HIGH);
                        digitalWrite(STATUS0, HIGH);  
                        digitalWrite(STATUS1, HIGH);  
                        delay(50);
                        digitalWrite(BUZZER, LOW); 
                        digitalWrite(STATUS0, LOW);  
                        digitalWrite(STATUS1, LOW);  
                        delay(50);   
            }
            
            if ( (heater_temp[2] - sensor_temp[0]) < heating_threshold )
            {
                //call out_of_order;  // 고장 통보 
                Serial.println(" sensor heating test 3: damaged  ");
                        digitalWrite(BUZZER, HIGH);
                        digitalWrite(STATUS0, HIGH);  
                        digitalWrite(STATUS1, HIGH);  
                        delay(50);
                        digitalWrite(BUZZER, LOW); 
                        digitalWrite(STATUS0, LOW);  
                        digitalWrite(STATUS1, LOW);  
                        delay(50);   
            }

            Serial.println(" cooling 1 minute"); 
            for (int tt=1;tt<=60;tt++)  {   // 60 second  time_delay_for_cooling 
                delay(1000);                // 
                        digitalWrite(STATUS0, HIGH);
                        digitalWrite(STATUS1, HIGH);  
                        delay(10);
                        digitalWrite(STATUS0, LOW);
                        digitalWrite(STATUS1, LOW);  
                        delay(5);  
                        
                        Serial.print(".");      
            }

    } //-- sensor test mode -------------------------------------------------


      
  
}     //----- loop ------------------------------------------------------------
