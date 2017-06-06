#include "Arduino.h"
// #include <PID_v1.h>
#include "PID.h"
#define DEBUG_WHEELS

PID pid(true);

#define ML_A D6
#define ML_B D5
#define MR_A D4
#define MR_B D3

enum{
  forward,
  reverse,
  stop
} wheel_state;

void init_motors()
{
  pinMode(ML_A, OUTPUT);
  pinMode(ML_B, OUTPUT);
  pinMode(MR_A, OUTPUT);
  pinMode(MR_B, OUTPUT);
  analogWriteRange(255);
  analogWriteFreq(100);
}

void ML_fwd(int x)
{
    analogWrite(ML_A, x);
    analogWrite(ML_B, 0);
}

void ML_rev(int x)
{
  analogWrite(ML_A, 0);
  analogWrite(ML_B, x);
}

void MR_fwd(int x)
{
  analogWrite(MR_A, x);
  analogWrite(MR_B, 0);
}

void MR_rev(int x)
{
  analogWrite(MR_A, 0);
  analogWrite(MR_B, x);
}

volatile unsigned long wheel_count_L = 0;
volatile unsigned long wheel_count_R = 0;

#define TIME_THRESHOLD 5
unsigned long prev_time_L = 0;
unsigned long prev_time_R = 0;

void count_encoder_L()
{
  // noInterrupts();
  //
  // unsigned long currentTime = millis();
  // if ( currentTime - prev_time_L < TIME_THRESHOLD)
  // {
  //   // return;
  // }
  // else
  // {
  //     prev_time_L = currentTime;
  //     wheel_count_L++;
  // }
  //
  // interrupts();
}

void count_encoder_R()
{
  // noInterrupts();
  //
  // unsigned long currentTime = millis();
  // if ( currentTime - prev_time_R < TIME_THRESHOLD)
  // {
  //   // return;
  // }
  // else
  // {
  //     prev_time_R = currentTime;
  //     wheel_count_R++;
  // }
  //
  // interrupts();
}

double leftControl=0, rightControl=0, speed=150;

void timer0_ISR (void) {
  //noInterrupts();
  double deviation=(long)wheel_count_R - (long)wheel_count_L;

  double control=pid.calculate(deviation);
  double bias=0;

  leftControl=speed-control-bias;
  rightControl=speed+control+bias;
  leftControl=leftControl>0?leftControl:0;
  leftControl=leftControl<255?leftControl:255;
  rightControl=rightControl>0?rightControl:0;
  rightControl=rightControl<255?rightControl:255;


  ML_fwd((int) leftControl);
  MR_fwd((int) rightControl);

  #ifdef DEBUG_WHEELS
    Serial.print("L : ");
    Serial.print(wheel_count_L);
    Serial.print("\tR : ");
    Serial.print(wheel_count_R);
    Serial.print("\tDev : ");
    Serial.print(deviation);
    Serial.print("\tController : ");
    Serial.print(control);
  #endif

  timer0_write(ESP.getCycleCount() + 4000000L); // 8MHz == 100 ms
  //interrupts();
}

void init_timer() {
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 4000000L); // 8MHz == 100 ms
  interrupts();
}
