#include <Arduino.h>

#define SPAM

constexpr const int control_pin=6;
constexpr const int thermometer_pin=A2;

constexpr const int temperature=394;// adjusted to get 37 from the other thermometer
constexpr const int samples=2048; // the resulting rate will be 5 times a second

void setup() {
  // put your setup code here, to run once:
  analogReference(INTERNAL);//2.56v on this arduino
  pinMode(thermometer_pin, INPUT);
  pinMode(control_pin, OUTPUT);
  pinMode(13, OUTPUT);
  
  #ifdef SPAM
  Serial.begin(115200);
  #endif
}

bool blinker=false;

long integral=0;
// integral can cause overshoot by at most 1 degree for 30 seconds = 10 tehtns of degree * 5 samples per second * 30
long integral_low=-1500;
long integral_high=1500;

void loop() {  
  //int tr=analogRead(thermometer_pin);
  // voltage=tr*5/1024
  // temperature from voltage:
  // (v-0.750)*0.01 + 25;
  // temperature from value:
  // (v*5/1024-0.750)*100 + 25;
  // (tr*500/1024 - 75)+25
  // tr*500/1024 - 50
  //Serial.println(tr);
  // temperature in tenths of a degree, celsius
  analogReference(INTERNAL);//2.56v on this arduino
  long sum=0;
  for(int i=0; i<samples; ++i){
    sum+=analogRead(thermometer_pin);
  }
  long avg_times_256=sum/(samples/256);
  int t=((long)avg_times_256*10l/1024l)-500l;
  //int t=((long)tr*2560l/1024l)-500l;
  integral+=temperature-t;

  integral=min(max(integral, integral_low), integral_high);

/*
  if(t<temperature){
    digitalWrite(control_pin, 1);
    digitalWrite(13, blinker);
  }else{
    digitalWrite(control_pin, 0);
    analogWrite(13, blinker*64);
  }*/
  // 0 to 255 control values
  // 1 degree casuse full swing
  // maxed out integral causes full swing
    
  // prevent wind-up on startup
  if((temperature-t)*25>255){
    integral=0;
  }

  long control_value=(temperature-t)*25 + integral/5;
  int cv=min(max(control_value, 0),255);
  analogWrite(control_pin, cv);
  //analogWrite(13, cv);

  #ifdef SPAM
  Serial.print("t=");
  Serial.print(t);
  Serial.print(", i=");  
  Serial.print(integral);
  Serial.print(", control_value=");
  Serial.println(control_value);
  #endif

  blinker=!blinker;
  digitalWrite(13, blinker);
}