#include <Wire.h>
#include <RTClib.h>


#define RTC_POW_PIN A3
#define RTC_GND_PIN A2
#define BUZZER_PIN A1
#define BTN_GND_PIN A0

#define FIRST_B 10  //mode
#define SECOND_B 11 //alarm
#define THIRD_B 12  //timer
#define FOURTH_B 8 //ok

#define CONTROL_PIN_EVEN 13
#define CONTROL_PIN_ODD 9
#define DATA_PIN 7
#define LATCH_PIN 6
#define CLOCK_PIN 5
#define DATA_PIN_1 4
#define LATCH_PIN_1 3
#define CLOCK_PIN_1 2

#define BUTTON_PINS 3  //number for buttons
#define FIRST_BUTTON_PINS 10
#define OUT_PINS 8    //number pins for output
#define FIRST_OUT_PINS 2

byte numberSegment[20] = {
  0b00111111, 0b00001100, 0b01011011, 0b01011101, 0b01101100, 
  0b01110101, 0b01110111, 0b00011100, 0b01111111, 0b01111101, 
  0b01000000, 0b01111110, 0b00100011, 0b00110010, 0b00011100,
  0b00010000, 0b00101110, 0b00111100, 0b01110011, 0b00110011};

int watches_digits[4] = {
  13, 7, 15, 19};
int mode_digits[4] = {
  10, 10, 10, 10}; 
int alarm_digits[4] = {
  11, 12, 11, 13};
int timer_digits[4] = {
  14, 15, 16, 17}; 
int setting_digits[4] = {
  5, 18, 7, 15};
int rtc_digits[4] = {
  13, 7, 15, 19};
  
int alarm_set_on[4] = { 
  0, 0, 0, 0};
int timer_set_on[4] = { 
  0, 0, 0, 0};
int timer_setting_time[4] = {
  0, 0, 0, 0};
int setting_set_on[4] = { 
  0, 0, 0, 0};
  
RTC_DS1307 RTC;

boolean watchesOn = true;
boolean modeWasUp = true;
boolean modeOn = false;  //мод режим
//будильник
boolean alarmWasUp = true; 
boolean alarmOn = false;     //режим будильника
boolean alarmSet = false;    //режим установка будильника
boolean alarmReady = false;  //будильник включен
boolean alarmBlocked = false;//блокировка будильника
//таймер
boolean timerWasUp = true;
boolean timerOn = false;     //режим таймера
boolean timerSet = false;    //режим установки таймера
boolean timerReady = false;  //таймер включен
boolean timerBlocked = false;//таймер блокирован
//настройка
boolean settingWasUp = true;
boolean settingOn = false;     //режим времени
boolean settingSet = false;    //режим установки времени
//rtc
boolean rtcOn = true;         //для модуля часов
int cnt, digit_cnt;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Initialize"));

  for (int i = 0; i < OUT_PINS; i++)
    pinMode(i + FIRST_OUT_PINS, OUTPUT);
  for (int i = 0; i < BUTTON_PINS; i++)
    pinMode(i + FIRST_BUTTON_PINS, INPUT);
  pinMode(FOURTH_B, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_GND_PIN, OUTPUT);
  pinMode(RTC_GND_PIN, OUTPUT);
  pinMode(RTC_POW_PIN, OUTPUT);
  digitalWrite(RTC_POW_PIN, HIGH);
  digitalWrite(RTC_GND_PIN, LOW);
  digitalWrite(BTN_GND_PIN, LOW);

  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning())
  {
    Serial.println(F("RTC is not running!"));
    RTC.adjust(DateTime(__DATE__, __TIME__));
    rtcOn = false; 
  }
  
  cnt = 0;
  digit_cnt = 0;
  Serial.println(digitalRead(FIRST_B));
  Serial.println(digitalRead(SECOND_B));
  Serial.println(digitalRead(THIRD_B));
  Serial.println(digitalRead(FOURTH_B));
}

void loop() {
  digitalWrite(CONTROL_PIN_EVEN, LOW);
  digitalWrite(CONTROL_PIN_ODD, LOW);
   
  DateTime datetime = RTC.now();
  watches_digits[0] = datetime.hour() / 10;
  watches_digits[1] = datetime.hour() % 10;
  watches_digits[2] = datetime.minute() / 10; 
  watches_digits[3] = datetime.minute() % 10;
  
  boolean modeIsUp = digitalRead(FIRST_B);
  boolean alarmIsUp = digitalRead(SECOND_B);
  boolean timerIsUp = digitalRead(THIRD_B);
  boolean settingIsUp = digitalRead(FOURTH_B);

  if (!rtcOn) {
    watchesOn = false;
  }
  if (alarmReady) {
    alarmBeeps(watches_digits, alarm_set_on);
  }
  if (timerReady) {
    timerBeeps(watches_digits, timer_setting_time, timer_set_on);
  }
  
  if (modeWasUp && !modeIsUp) { //блок отвечающий за обработку событий кнопки меню
    delay(10);
    modeIsUp = digitalRead(FIRST_B);
    Serial.println(digitalRead(FIRST_B));
    if (!modeIsUp) {
      watchesOn = !watchesOn;
      modeOn = !modeOn;
      if (alarmOn || timerOn || settingOn || alarmSet || timerSet || settingSet || alarmReady || timerReady) {
        alarmOn = false;
        timerOn = false;
        settingOn = false;
        alarmSet = false;
        timerSet = false;
        settingSet = false;
        modeOn = !modeOn;
        cnt = 0;
        digit_cnt = 0;
        if (alarmReady) {
          alarmBlocked = true;   //блокируем показ будильника
          modeOn = true;
        }
        if (timerReady) {
          timerBlocked = true;   //блокируем показ таймера
          modeOn = true;
        }
      }
    }
  }
  
  if (alarmWasUp && !alarmIsUp) {
    delay(10);
    alarmIsUp = digitalRead(SECOND_B);
    Serial.println(F("alarm"));
    Serial.println(digitalRead(SECOND_B));
    if (!alarmIsUp) {
      if (modeOn) {
        modeOn = false;
        alarmOn = true;
      }
      else if (alarmOn && !alarmReady) { //будильник -> установка будильника
        fillWithZeros(alarm_set_on);
        alarmOn = false;
        alarmSet = true;
      }
      else if (alarmReady && !watchesOn && !timerSet) { //если будильник уже был установлен - показать
        alarmOn = false;
        alarmSet = false;
        alarmBlocked = false;
      }
      else if (timerOn || settingOn) {
        timerOn = false;
        settingOn = false;
        modeOn = !modeOn;        
      }
      else if (alarmSet && (cnt > 0)) {
        cnt--;
        alarm_set_on[digit_cnt] = cnt;
      }
      else if (timerSet && (cnt > 0)) {
        cnt--;
        timer_set_on[digit_cnt] = cnt;
      }
      else if (settingSet && (cnt > 0)) {
        cnt--;
        setting_set_on[digit_cnt] = cnt;
      }
    }
  }
  
  if (timerWasUp && !timerIsUp) {
    delay(10);
    timerIsUp = digitalRead(THIRD_B);
    Serial.println(F("timer"));
    Serial.println(digitalRead(THIRD_B));
    if (!timerIsUp) {
      if (modeOn) { //мод -> таймер
        modeOn = false;
        timerOn = true;
      }
      else if (timerOn && !timerReady) { //таймер -> установка таймера
        fillWithZeros(timer_set_on);
        timerOn = false;
        timerSet = true;
      }
      else if (timerReady && !watchesOn && !alarmSet) { //если таймер уже был установлен - показать
        timerOn = false;
        timerSet = false;
        timerBlocked = false;  
      }
      else if (alarmOn || settingOn) {
        alarmOn = false;
        settingOn = false;
        modeOn = !modeOn;
      }
      else if (alarmSet) { //настройка будильника, увеличение
        if ((digit_cnt == 0 && cnt < 2) || (digit_cnt == 1 && alarm_set_on[0] < 2 && cnt < 9) || (digit_cnt == 1 && alarm_set_on[0] == 2 && cnt < 3) || (digit_cnt == 2 && cnt < 5) || (digit_cnt == 3 && cnt < 9)) {
          cnt++;
          alarm_set_on[digit_cnt] = cnt;  
        }
      }
      else if (timerSet) { //настройка таймера, увеличение
        if ((digit_cnt == 0 && cnt < 2) || (digit_cnt == 1 && timer_set_on[0] < 2 && cnt < 9) || (digit_cnt == 1 && timer_set_on[0] == 2 && cnt < 3) || (digit_cnt == 2 && cnt < 5) || (digit_cnt == 3 && cnt < 9)) {
          cnt++;
          timer_set_on[digit_cnt] = cnt;  
        }
      }
      else if (settingSet) { //настройка времени, увеличение
        if ((digit_cnt == 0 && cnt < 2) || (digit_cnt == 1 && setting_set_on[0] < 2 && cnt < 9) || (digit_cnt == 1 && setting_set_on[0] == 2 && cnt < 3) || (digit_cnt == 2 && cnt < 5) || (digit_cnt == 3 && cnt < 9)) {
          cnt++;
          setting_set_on[digit_cnt] = cnt;  
        }
      }
    }
  }
  
  if (settingWasUp && !settingIsUp) {
    delay(10);
    settingIsUp = digitalRead(FOURTH_B);
    Serial.println(F("sett"));
    Serial.println(digitalRead(FOURTH_B));
    if (!settingIsUp) {
      if (modeOn) {
          modeOn = false;
          settingOn = true;
        }
        else if (settingOn) {
          settingOn = false;
          settingSet = true;
        }
        else if (alarmOn || timerOn) {
          alarmOn = false;
          timerOn = false;
          modeOn = !modeOn;
        }
        else if (alarmSet) {
          if (digit_cnt < 3) { //проходим по числам
            digit_cnt++;
            cnt = 0;  
          }
          else {
            alarmReady = true;
            alarmSet = false;
            modeOn = !modeOn;
            alarmBlocked = !alarmBlocked;
            cnt = 0;
            digit_cnt = 0;
          }
        }
        else if (alarmReady && !watchesOn && !alarmBlocked) { //сбрасываем уже установленный будильник
          alarmReady = !alarmReady;
          alarmSet = false;
          modeOn = !modeOn;
          alarmBlocked = !alarmBlocked;
          cnt = 0;
          digit_cnt = 0;
          fillWithZeros(alarm_set_on);
        }
        else if (timerSet) {
          if (digit_cnt < 3) { //проходим по числам
            digit_cnt++;
            cnt = 0;  
          }
          else {
            timerReady = true;
            timerSet = false;
            modeOn = !modeOn;
            timerBlocked = !timerBlocked;
            for ( int i = 0; i < 4 ; i++) 
              timer_setting_time[i] = watches_digits[i];
            cnt = 0;
            digit_cnt = 0;
          }
        }
        else if (timerReady && !watchesOn && !timerBlocked) { //сбрасываем уже установленный таймер
          timerReady = !timerReady;
          timerSet = false;
          modeOn = !modeOn;
          timerBlocked = !timerBlocked;
          cnt = 0;
          digit_cnt = 0;
          fillWithZeros(timer_set_on);
        }
        else if (settingSet) { //для настройки времени
          if (digit_cnt < 3) { //проходим по числам
            digit_cnt++;
            cnt = 0;  
          }
          else {
            RTC.adjust(DateTime(2016, 01, 05, (setting_set_on[0] * 10 + setting_set_on[1]), (setting_set_on[2] * 10 + setting_set_on[3]), 0));
            settingSet = false;
            modeOn = !modeOn;
            cnt = 0;
            digit_cnt = 0;
            fillWithZeros(setting_set_on);
          }
        }
    }
  }
   
  if (watchesOn) {
    informationToIndicator(watches_digits, 0);
  }
  else if (!rtcOn) {
    informationToIndicator(rtc_digits, 0);
  }
  else if (modeOn) {
    informationToIndicator(mode_digits, 0);
  }
  else if (alarmOn) {
    informationToIndicator(alarm_digits, 50);   
  }
  else if (alarmSet) {
    informationToIndicator(alarm_set_on, 100);   
  }
  else if (alarmReady && !alarmBlocked) {
    informationToIndicator(alarm_set_on, 0);   
  }
  else if (timerOn) {
    informationToIndicator(timer_digits, 50);   
  }
  else if (timerSet) {
    informationToIndicator(timer_set_on, 100);   
  }
  else if (timerReady && !timerBlocked) {
    informationToIndicator(timer_set_on, 0);   
  }
  else if (settingOn) {
    informationToIndicator(setting_digits, 100);   
  }
  else if (settingSet) {
    informationToIndicator(setting_set_on, 100);   
  }
  
  modeWasUp = modeIsUp;
  alarmWasUp = alarmIsUp;
  timerWasUp = timerIsUp;
  settingWasUp = settingIsUp;
}

//функция для отрисовки массивов на индикаторе
void informationToIndicator(int digits[4], int flashing)
{
  digitalWrite(LATCH_PIN_1, LOW);
  shiftOut(DATA_PIN_1, CLOCK_PIN_1, LSBFIRST, numberSegment[digits[0]]);
  digitalWrite(LATCH_PIN_1, HIGH);
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, numberSegment[digits[2]]);
  digitalWrite(LATCH_PIN, HIGH);
  digitalWrite(CONTROL_PIN_ODD, HIGH);
  delay(2);
  digitalWrite(CONTROL_PIN_ODD, LOW);

  digitalWrite(LATCH_PIN_1, LOW);
  shiftOut(DATA_PIN_1, CLOCK_PIN_1, LSBFIRST, numberSegment[digits[1]]);
  digitalWrite(LATCH_PIN_1, HIGH);
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, numberSegment[digits[3]]);
  digitalWrite(LATCH_PIN, HIGH);
  digitalWrite(CONTROL_PIN_EVEN, HIGH);
  delay(2);
  digitalWrite(CONTROL_PIN_EVEN, LOW);

  delay(flashing); //delay for others mode
}

//функция для обнуления массивов
void fillWithZeros(int array[4])
{
  for (int i = 0; i < 4; i++)
    array[i] = 0;
}

void alarmBeeps(int current_time[4], int alarm[4])
{
  if ((current_time[0] == alarm[0]) && (current_time[1] == alarm[1]) && (current_time[2] == alarm[2]) && (current_time[3] == alarm[3])) {
    beepBeep(6, 1760, 500);
    alarmReady = false;
    fillWithZeros(alarm_set_on);
    modeOn = false;
    watchesOn = true;
  }
}

void timerBeeps(int current_time[4], int installation_time[4], int timer[4])
{
  int a, b, c, d, estimated_minuts_div, estimated_minuts_mod, estimated_hours_div;

  estimated_minuts_div = ( ( ( timer[2] * 10 ) + timer[3] ) + ( ( installation_time[2] * 10 ) + installation_time[3] ) ) % 60;
  estimated_minuts_mod = ( ( ( timer[2] * 10 ) + timer[3] ) + ( ( installation_time[2] * 10 ) + installation_time[3] ) ) / 60;
  estimated_hours_div = ( ( ( timer[0] * 10 ) + timer[1] ) + ( ( installation_time[0] * 10 ) + installation_time[1] ) + estimated_minuts_mod) % 24;
 
  a = estimated_hours_div / 10;
  b = estimated_hours_div % 10;
  c = estimated_minuts_div / 10;
  d = estimated_minuts_div % 10;
  
  if ((current_time[0] == a) && (current_time[1] == b) && (current_time[2] == c) && (current_time[3] == d)) {
    beepBeep(4, 880, 500);
    timerReady = false;
    fillWithZeros(timer_set_on);
    fillWithZeros(timer_setting_time);
    modeOn = false;
    watchesOn = true;
  }
}

void beepBeep(int loops, int frequency, int beep_time)
{
  for (int i = 0; i < loops; i++) {
    tone(BUZZER_PIN, frequency, beep_time);
    delay(beep_time);
  }
}
