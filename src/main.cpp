#include <Arduino.h>

#include "BleKeyboard.h"
BleKeyboard bleKeyboard;

#include "HX711.h"
// HX711 を制御するためのインスタンス
HX711 scale0;
HX711 scale1;
int stand_sc0,stand_sc1;
int val_scale0,val_scale1,def_scale0,def_scale1,old0,old1;
bool junmflug;
int jump_count;
#define th 200000


#define button 33
#define uisw1 35
#define uisw2 34
bool button_status=1,pui=0;
int button_count;

#define red0    15
#define green0  2
#define blue0   4
#define red1    26
#define green1  25
#define blue1   32
//led[red,green,blue,light]
double led0[4];//画面
double led1[4];//脚
const double RED[4]  ={255,  0,  0,0};
const double GREEN[4]={  0,255,  0,0};
const double BLUE[4] ={  0,  0,255,0};
const double WHITE[4]={255,255,255,0};
bool ledflug;
int ledcount;


void led_out(int led_num,double color[4]);
void scale_read(bool scale_num);

void setup() {
  pinMode(button,INPUT_PULLDOWN);
  pinMode(uisw1,INPUT);
  pinMode(uisw2,INPUT);
  ledcSetup(0,12800,8);
  ledcAttachPin(red0,0);
  ledcSetup(1,12800,8);
  ledcAttachPin(green0,1);
  ledcSetup(2,12800,8);
  ledcAttachPin(blue0,2);
  ledcSetup(3,12800,8);
  ledcAttachPin(red1,3);
  ledcSetup(4,12800,8);
  ledcAttachPin(green1,4);
  ledcSetup(5,12800,8);
  ledcAttachPin(blue1,5);

  Serial.begin(115200);
  bleKeyboard.begin();
  Serial.print("Bluetooth");
  while (bleKeyboard.isConnected()==false){
    led1[0]=0;
    led1[1]=0;
    led1[2]=255;
    led1[3]=50;
    led_out(0,led1);
    led_out(1,led1);
    Serial.print(".");
    delay(1000);
  }
  Serial.println("connect");

  
  scale0.begin(12,13); 
  scale1.begin(27,14);
  if(1){
    Serial.print("check scale0");
    led1[0]=255;
    led1[1]=0;
    led1[2]=0;
    led1[3]=50;
    led_out(0,led1);
    led_out(1,led1);
    while(scale0.is_ready()==false){
      Serial.print(".");
      if (scale0.is_ready()) {
        stand_sc0 = scale0.get_units(100);
      }
    }
    Serial.print("check scale1");
    led1[0]=0;
    led1[1]=255;
    led1[2]=0;
    led1[3]=50;
    led_out(0,led1);
    led_out(1,led1);
    while(scale1.is_ready()){
      Serial.print(".");
      if (scale1.is_ready()) {
        stand_sc1 = scale1.get_units(100);
      }
    }
  }
  
  Serial.println("");
  led1[0]=0;
  led1[1]=0;
  led1[2]=0;
  led1[3]=0;
  led_out(0,led1);
  led_out(1,led1);
}


void loop() {
  
  ledflug=0;
  Serial.print(digitalRead(button));
  Serial.print("  ");
  Serial.print(button_count);
  Serial.print("  ");
  if (button_count<3 && digitalRead(button) == 0){
  //if ( digitalRead(button) == 0){
    button_count=button_count+1;
    led1[0]=0;
    led1[1]=0;
    led1[2]=0;
    led1[3]=0;
    led_out(0,led1);
    led_out(1,led1);
    if(pui==0){
      Serial.print("scale setup");
      while(scale0.is_ready()){
        Serial.print(".");
        if (scale0.is_ready()) {
          stand_sc0 = scale0.get_units(20);
        }
      }
      while(scale1.is_ready()){
        Serial.print(".");
        if (scale1.is_ready()) {
          stand_sc1 = scale1.get_units(20);
        }
      }
      pui=1;
    }else if(pui==1){
      if(bleKeyboard.isConnected()){
        ledflug=1;
        led1[0]=0;
        led1[1]=255;
        led1[2]=0;
        led1[3]=100;
        led_out(0,led1);
        led_out(1,led1);
      
        bleKeyboard.print("w");//攻撃
        bleKeyboard.releaseAll();
        delay(10);
      }
      Serial.println("pui");
    }
    
  }else if(digitalRead(button)==1||button_count>7){
    button_count=0;
  }
  button_status = digitalRead(button);
  if(analogRead(uisw2)<=2000){
    ledflug=1;
    led1[0]=255;
    led1[1]=255;
    led1[2]=255;
    led1[3]=100;
    led_out(0,led1);
    led_out(1,led1);
    pui=0;
    Serial.println("key off");
  }else if(analogRead(uisw1)<=3000){
    pui=1;
    Serial.println("key on");
  }
  if(analogRead(uisw1)<=3000){
    ledflug=1;
    Serial.print("scale setup");
    while(scale0.is_ready()){
      Serial.print(".");
      if (scale0.is_ready()) {
        stand_sc0 = scale0.get_units(100);
      }
    }
    while(scale1.is_ready()){
      Serial.print(".");
      if (scale1.is_ready()) {
        stand_sc1 = scale1.get_units(100);
      }
    }
  }

  old0=val_scale0;
  old1=val_scale1;
  scale_read(0);
  val_scale0=abs(val_scale0);
  def_scale0=val_scale0-old0;
  Serial.print(val_scale0);
  Serial.print("    ");
  scale_read(1);
  val_scale1=abs(val_scale1);
  def_scale1=val_scale1;
  Serial.print(def_scale0);
  Serial.print("    ");
  Serial.println(jump_count);
  if(pui==1){
    if(val_scale0<=th&&def_scale0<-5000){
      
      
      jump_count++;
      if(bleKeyboard.isConnected()&&jump_count==1){
        
        ledflug=1;
        led1[0]=0;
        led1[1]=0;
        led1[2]=255;
        led1[3]=100;
        led_out(0,led1);
        led_out(1,led1);
        bleKeyboard.print("j");//ジャンプ
        bleKeyboard.releaseAll();
        delay(10);
      }
    //}else if(val_scale0>=th&&val_scale1>=th){
      
    //}else if(val_scale0>=val_scale1||0){
    }else if(0){
      if(bleKeyboard.isConnected()){
        ledflug=1;
        bleKeyboard.print("d");//右
        bleKeyboard.releaseAll();
        delay(10);
      }
    //}else if(val_scale0<val_scale1||0){
    }else if(0){
      if(bleKeyboard.isConnected()){
        ledflug=1;
        bleKeyboard.print("a");//左
        bleKeyboard.releaseAll();
        delay(10);
      }
    }else{
      if(jump_count>3||val_scale0>=th){
        jump_count=0;
      }      
    }
  }

  if(ledflug==1&&ledcount<5){
    ledcount++;
  }else{
    ledcount=0;
    led1[0]=255;
    led1[1]=0;
    led1[2]=0;
    led1[3]=100;
    led_out(0,led1);
    led_out(1,led1);
  }

  //Serial.print(digitalRead(button));
  //Serial.print("  ");
  
  //Serial.println(button_status);
  
}

void led_out(int led_num,double color[4]){
  if(color[3]>=100){
    color[3]=100;
  }else if(color[3]<=0){
    color[3]=0;
  }
  color[3]=color[3]/100;
  if(led_num==0){
    ledcWrite(0,color[0]*color[3]);
    ledcWrite(1,color[1]*color[3]);
    ledcWrite(2,color[2]*color[3]);
  }else if(led_num==1){
    ledcWrite(3,color[0]*color[3]);
    ledcWrite(4,color[1]*color[3]);
    ledcWrite(5,color[2]*color[3]);
  }
}


void scale_read(bool scale_num){
  if(scale_num==0){
    // HX711とESP32の間で通信準備が完了したら実行
    if (scale0.is_ready()) {
      // HX711から受け取る値の10回平均を返す
      int VAL_scale0 = scale0.get_units(3);
      val_scale0=VAL_scale0-stand_sc0;
      //Serial.print(val_scale0);
    } else {
      Serial.println("Scal0 is not ready");
    }
  }else if(scale_num==1){
    if (scale1.is_ready()) {
      // HX711から受け取る値の10回平均を返す
      int VAL_scale1 = scale1.get_units(3);
      val_scale1=VAL_scale1-stand_sc1;
      //Serial.println(val_scale1);
    } else {
      Serial.print("Scal1 is not ready");
    }
  }
}