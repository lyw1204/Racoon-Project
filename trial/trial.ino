#include <common.h>
#include <FirebaseFS.h>
#include <Firebase_ESP_Client.h>
#include <Utils.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>         //看一下有哪些Library需要加
//WIFI and DB section
#define FIREBASE_HOST "https://raccoonlock-default-rtdb.firebaseio.com/" //Without http:// or https:// schemes
#define API_KEY "AIzaSyD3QWwmDnP0kIMlcZbPzi0YY0ERE4Ch9kk"

#define WIFI_SSID "BELL738"  //改一下WI-FI信息
#define WIFI_PASSWORD "D63E55A6"
#define USER_EMAIL "racoonlock1@gmail.com"
#define USER_PASSWORD "1234567890"

char ssid [32] = WIFI_SSID;
char password [63] = WIFI_PASSWORD;

int val=0; //声明变量，用来存储按钮返回值HIGH或者LOW；
int val_old=0;//声明变量，存储上一次按钮返回值；比对val和val_old的值，来判断按钮是按下还是抬起
       //val==LOW && val_old==HIGHT 抬起状态；val==HIGH && val_old==LOW 按下状态；
int state=0;//0 off,1 on 声明变量，起到中转的作用；
WiFiUDP ntpUDP;//初始化ntp插件
NTPClient timeClient(ntpUDP);//读取时间
String t;//存时间

void setup() {     //只对我自己有用
 pinMode(D0,INPUT); 
 wifiConnect();    //这个记得加到setup里
}

void loop() {
  // put your main code here, to run repeatedly:
  wifiConnect();

    val=digitalRead(D0);//读取数字针脚的值；
    if((val==HIGH) && (val_old==LOW))//比较前一次和现在的按钮返回的值，
                                     //只有现在按下按钮这个状态下，才改变state的值
  {
    state=1-state;
  }
  if((val==LOW && val_old==HIGH)) //去抖的作用；
    {
      delay(10);
    }
  val_old=val;//把现在按钮返回的值赋予val_old；
  if(state==1)//控制数据传输；
    {
      firebaseHello(HIGH); //检测到浣熊时
    }else{
      firebaseHello(LOW);  //检测到人时
   }
}

void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID); Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++teller); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  timeClient.begin();       // 用于计时
}

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void firebaseHello(bool s){

  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  if(s==HIGH){           //检测到浣熊时
    timeClient.update();
    t=timeClient.getFormattedTime();  // 记录时间
  
    if(Firebase.RTDB.setString(&fbdo, "/Device_ID/Event/E_Time", t)){  //发送到数据库
      //Success
      Serial.println("Set int data success");
    }else{                                                      //Failed?, get the error reason from fbdo
      Serial.print("Error in setInt, ");
      Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.setString(&fbdo, "/Device_ID/Event/type", "Raccoon detected")){
        //Success
        Serial.println("Set int data success");
    }else{//Failed?, get the error reason from fbdo
      Serial.print("Error in setInt, ");
      Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.setBool(&fbdo, "/Device_ID/Event/bool", s)){
      //Success
      Serial.println("Set int data success");
    }else{//Failed?, get the error reason from fbdo
      Serial.print("Error in setInt, ");
      Serial.println(fbdo.errorReason());
    }
 }else if(s==LOW){      //检测到人时
    timeClient.update();
    t=timeClient.getFormattedTime();
  
    if(Firebase.RTDB.setString(&fbdo, "/Device_ID/Event/E_Time", t)){
      //Success
      Serial.println("Set int data success");
    }else{//Failed?, get the error reason from fbdo
      Serial.print("Error in setInt, ");
      Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.setString(&fbdo, "/Device_ID/Event/type", "Human")){
      //Success
      Serial.println("Set int data success");
    }else{//Failed?, get the error reason from fbdo
      Serial.print("Error in setInt, ");
      Serial.println(fbdo.errorReason());
  }
  if(Firebase.RTDB.setBool(&fbdo, "/Device_ID/Event/bool", s)){
      //Success
      Serial.println("Set int data success");
    }else{//Failed?, get the error reason from fbdo
      Serial.print("Error in setInt, ");
      Serial.println(fbdo.errorReason());
  }
 }
}
