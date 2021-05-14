
%-------- 초기치 설정 --------------------------------------

sample_rate          = 1;               % sample number in one second

fire_detection_lapse = 2*sample_rate;   % 화재 지속 판정 유보 time [second] 
reset_lapse_counter  = 60*sample_rate;  %  초기 power ON 시 화재 비감지 시간 [second]  

% flag ----------------------------------------------------
reset_counter       = 0;               % 전원 ON reset 시 initial start counter 
detect_counter      = 0;               % fire 온도 감지 지속 시간 측정 카운터   
fire_decision       = 0;               % 화재 확인  
fire_alarm          = 0; 

%-----  power on reset ----------------------------------

for mmm = 1:reset_lapse_counter             % 처음 전원을 켜면 일정시간 무동작 

    for pp=1:numb_avr-1;  % 이전 저장 데이터 갱신 : 이전 값 평균 계산 
       backup_data(numb_avr-pp+1) = backup_data(numb_avr-pp);
    end
    
    data_in = analogRead(port);             % arduino data read
    backup_data(1) = data_in;
end

% backup_data(1) = latest data

%-------- 초기치 설정 --------------------------------------
smoke_detection_voltage = 0.025;    % smoke voltage difference detection
numb_avr = 30;                       % backup data number: calculate average 
noise_threshold = 0.02               % for average voltage reference  

alpha = 0.5;
beta = 1-alpha;
past_threshold  = backup_data(1);   % 초기치 평균값
smoke_threshold = backup_data(1);  % 초기치 평균값 
past_data = backup_data(2) ;


%-------- start fire decision ---------------------------------
%-------- start fire decision ---------------------------------
%-------- start fire decision ---------------------------------

do {
    
    data_adc =  analogRead(port);            % arduino data read 
    
    %------------------------------------------------------
    filter_out = alpha*data_adc + beta*past_data;  % IIR filtering 
    
    backup_data(1) = filter_out;
    past_data = filter_out;
    %
    % average smoke voltage 계산 : IIR SUM  -------------
    if abs( filter_out-past_threshold ) > noise_threshold    % noise input signal 제거  
        smoke_threshold = past_threshold;
        backup_data(1) = past_threshold;
    else
        smoke_threshold = past_threshold + (backup_data(1)-backup_data(numb_avr))/numb_avr;  % average smoke voltage 
    end
    
    for pp=1:numb_avr-1;  % 이전 저장 데이터 갱신 
       backup_data(numb_avr-pp+1) = backup_data(numb_avr-pp);
    end   
    %------------------------------------------------------
    
    past_threshold = smoke_threshold;
    
    y_out(nn) = smoke_threshold;       % for plot 
    filter_out_plot(nn) =filter_out;   % for plot 
        
    %------ find smoke signal --------------------------------------

    if ( filter_out-smoke_threshold ) > smoke_detection_voltage
        detect_counter = detect_counter+1;            % smoke 연속 감지
    else
        detect_counter = detect_counter-1;            % smoke 연속 감지 안됨 
    end
    
    if detect_counter < 0
        detect_counter = 0;
    elseif detect_counter > fire_detection_lapse       % 화재 온도 일정 시간 이상 감지 
        detect_counter = fire_detection_lapse;
        fire_decision = 1;                             % 화재 발생 판정
    end
    
    %----------------------------------------------------------------
    %if detect_counter > 0           % 화재 비정상 감지 
    %    call controller;            % 통신 통보 
    %end
    
    fire_alarm = 0;
    if fire_decision == 1 
        fire_alarm = 1;              % 화재 알림 통보  
    end 
    %-------------------------------------------------------
    
} while(화재 감시) 
    
    
    
    
    
%----------sensor test -----------------------------------------
OFF_threshold = 0.2;  % 0.2 voltage
ON_threshold  = 1.0;  % 1.0 voltage

do {
    
    %-----------------normal-------------------------------

        
        shutter_off(); %
        LED_OFF();
        data_adc1 = analogRead(port1);            % arduino data read  

        if data_adc1 > OFF_threshold
            call out_of_order;  % 고장 통보
        end
        
        shutter_off(); %
        LED_ON();
        data_adc1 = analogRead(port1);            % arduino data read  

        if data_adc1 > OFF_threshold
            call out_of_order;  % 고장 통보
        end        
        

    
    %----------------ON--------------------------------

        
        shutter_on();   % 
        LED_OFF();
        data_adc1 = analogRead(port1);            % arduino data read  
    
        if data_adc1 > ON_threshold
            call out_of_order;  % 고장 통보
        end   
        
        shutter_on();   % 
        LED_ON();
        data_adc1 = analogRead(port1);            % arduino data read
        
        if data_adc1 < ON_threshold
            call out_of_order;  % 고장 통보
        end   
    


    
    shutter_off(); 
    LED_OFF();
    
} while(센서 테스트) 


