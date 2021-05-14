

%-------- 초기치 설정 --------------------------------------
filtered_past1 = 20;        % 전원 On시 초기 가정 온도 sensor #1
filtered_past2 = 20;        % 전원 On시 초기 가정 온도 sensor #2
alpha  = 0.9;               % noise filter 계수 0.5~0.9
beta   = 1- alpha;
% counter -------------------------------------------------
sample_rate          = 1;               % sample number in one second
fire_threshold_temp  = 60;              % fire alarm temperature [C]

fire_detection_lapse = 2*sample_rate;   % 화재 지속 판정 유지 time [second] 
reset_lapse_counter  = 60*sample_rate;  %  초기 power ON 시 화재 비감지 시간 [second]  

% flag ----------------------------------------------------
reset_counter       = 0;               % 전원 ON reset 시 initial start counter 
detect_counter      = 0;               % fire 온도 감지 지속 시간 측정 카운터   
fire_decision       = 0;               % 화재 판정 확인   
fire_alarm          = 0; 

% adc-temp  fitting function coefficient 
    Q1 =  0.29476;
    Q2 = -4.5725;
    Q3 =  33.54;
    Q4 = -50.2612;    
%-----  power on reset ----------------------------------

for mmm = 1:reset_lapse_counter             % 처음 전원을 켜면 일정시간 무동작 
    data_in = analogRead(port);             % arduino dummy data read 
    fire_alarm = 0;
end

%-------- start fire detection ---------------------------------

do {
    
    data_adc = analogRead(port);            % arduino data read 
    
    data_in = data_adc/100;

    out_temp = Q1*(data_in)^3 + Q2*data_in^2 + Q3*data_in + Q4;    % adc to temperature : third

    %-------- 센서 1: 온도값에서 노이즈 제거 ------
    filtered_data = alpha*out_temp + beta*filtered_past;   %  센서 #1
    filtered_past = filtered_data; 

    %------ find heat signal --------------------------------------

    fire_threshold_temp_low = fire_threshold_temp;  
    if detect_counter > 0                             % 화재 감지가 된 경우 
        fire_threshold_temp_low = fire_threshold_temp - 1;      % 1 degree noise margin 
    end
    
    if filtered_data > fire_threshold_temp_low        % 화재 감지가 된 경우 
        detect_counter = detect_counter+1;            % 화재 온도 연속 감지
    else
        detect_counter = detect_counter-1;            % 화재 온도 연속 감지 안됨 
    end
    
    if detect_counter < 0
        detect_counter = 0;
        fire_threshold_temp_low = fire_threshold_temp; 
    elseif detect_counter > fire_detection_lapse       % 화재 온도 일정 시간 이상 감지 
        detect_counter = fire_detection_lapse;
        fire_decision = 1;                             % 화재 발생 판정  
    end
    
    %----------------------------------------------------------------
    %if detect_counter > 0   % 화재 비정상 감지 
    %    call controller;    % 비정상 통신 통보 
    %end

    fire_alarm = 0;
    if fire_decision == 1 & detect_counter > 0         % 이미 화재 발생 상태이면  
        fire_alarm = 1;                                % 화재 알림 통보  
    end 
    %-------------------------------------------------------
    
} while(화재감시) 

%-----------------------------------------------------------------------
%-------- testing -------------------------------------------------------

normal_threshold = 20;     % 2 degree : adc data
heating_threshold =40;     % 4 degree : adc data
do {
    
    %-----------------normal-------------------------------
    for mm=1:10
        data_adc1 = analogRead(port1);            % arduino data read  
        data_adc2 = analogRead(port2);            % arduino data read  

        if abs(data_adc1-data_adc2) > normal_threshold
            call out_of_order;  % 고장 통보
        end
    end
    
    %----------------heating--------------------------------
    for mm=1:2
        heater_on(3second);   % heater ON OFF -> more than 5 degree rising
    
        data_adc1 = analogRead(port1);            % arduino data read  
        data_adc2 = analogRead(port2);            % arduino data read 
    
        if abs(data_adc1-data_adc2) < heating_threshold
            call out_of_order;  % 고장 통보
        end   
    
        time_delay_for_cooling(1minute);          % cooling
    end
    
} while(센서테스트) 





