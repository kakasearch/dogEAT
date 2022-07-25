#define BLINKER_WIFI
#define BLINKER_PRINT Serial
#define BLINKER_WITHOUT_SSL
//#define BLINKER_ESP_SMARTCONFIG
#include <Blinker.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#define maxd 180 //开门时舵机偏转角度
#define mind 5 //关门时舵机偏转角度
#define ServoPin D2
#define addr 2500 //保存间隔的地址
#define max_interval 200 //最大的投喂间隔 s


#define blinksk "123123213" //blinker的sk,需要自己去blinker注册
#define STASSID "kaka" //wifi名称
#define STAPSK  "12341234" //wifi密码

char auth[] = blinksk;
int interval_time = 3;
BlinkerSlider Slider1("ran");
BlinkerButton Button_open("btn-ope");

//////////////执行开关门动作
void ServoControl(int servoAngle)
{
  Serial.println("servo");
  Serial.println(servoAngle);
  double thisAngle =  (servoAngle - 0) * 2000 / (180.0) + 500; map(servoAngle, 0, 180, 500, 2500);//等比例角度值范围转换高电平持续时间范围
  unsigned char i = 50;//50Hz 每秒的周期次数(周期/秒) 即1S 50 个周期 每个周期20ms
  while (i--)
  {
    digitalWrite(ServoPin, HIGH); 
    delayMicroseconds(thisAngle); //高电平时间
    digitalWrite(ServoPin, LOW); 
    delayMicroseconds(20000 - thisAngle);//每个周期20ms减去高电平持续时间
  }
}
void open_(){
  ServoControl(maxd);
  }
void close_(){
  ServoControl(mind);
  }
void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
}


void slider_callback(int32_t value)
{
    BLINKER_LOG("get slider value: ", value);
    interval_time = value;
     EEPROM.write(addr,interval_time);
}
void heartbeat() {
    Slider1.print(interval_time);
   
}

void button_callback(const String & state)
{
    BLINKER_LOG("get button state: ", state);

    if (state == BLINKER_CMD_BUTTON_TAP) {
        BLINKER_LOG("Button tap!");
        open_();
        Blinker.delay(interval_time*100);
        close_();
    }
    else if (state == BLINKER_CMD_BUTTON_PRESSED) {
        BLINKER_LOG("Button pressed!");
        open_(); 
    }
    else if (state == BLINKER_CMD_BUTTON_RELEASED) {
        BLINKER_LOG("Button released!");
        close_();
    }
    else {
        BLINKER_LOG("Get user setting: ", state);
       BLINKER_LOG("未知操作");
    }
}
///////////////////////程序入口
void setup() {
 
  pinMode(ServoPin,OUTPUT);
  Serial.begin(115200);//开启端口，速度为115200
  // 初始化blinker
  BLINKER_DEBUG.stream(Serial);
  
  Blinker.begin(blinksk,STASSID,STAPSK);
 // Blinker.begin(auth);
  Blinker.attachData(dataRead);
  Button_open.attach(button_callback);
  Slider1.attach(slider_callback);
  Blinker.attachHeartbeat(heartbeat);
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
  interval_time = EEPROM.read(addr);
  Serial.print("interval: ");
  Serial.println(interval_time);
  if (interval_time <=0 || interval_time>max_interval){
  interval_time = 3;
  }
  
}

void loop() {
  Blinker.run();
 }
