#include "Arduino.h"
#include <PID_v1.h>

//#define DEBUG_WHEELS

//Define Variables we'll be connecting to
double left_ticks, right_ticks;

double Setpoint = 0, Output_L=0, Output_R=0;
double consKp=2, consKi=0.1, consKd=0.5;


//Specify the links and initial tuning parameters
PID myPID_L(&left_ticks, &Output_L, &Setpoint, consKp, consKi, consKd, DIRECT);
PID myPID_R(&right_ticks, &Output_R, &Setpoint, consKp, consKi, consKd, DIRECT);

#define ML_A D5
#define ML_B D6
#define MR_A D3
#define MR_B D4

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
  if (millis() - prev_time_L < TIME_THRESHOLD)
  {
    return;
  }

  prev_time_L = millis();
  wheel_count_L++;
  //Serial.print("L : ");
  //Serial.println(wheel_count_L);
}

void count_encoder_R()
{
  if (millis() - prev_time_R < TIME_THRESHOLD)
  {
    return;
  }

  prev_time_R = millis();
  wheel_count_R++;
  //Serial.print("R: ");
  //Serial.println(wheel_count_R++);
}

unsigned long prev_left_ticks=0;
unsigned long prev_right_ticks=0;

void timer0_ISR (void) {
  left_ticks = wheel_count_L - prev_left_ticks;
  prev_left_ticks = wheel_count_L;
  left_ticks *= 10;

  right_ticks = wheel_count_R - prev_right_ticks;
  prev_right_ticks = wheel_count_R;
  right_ticks *= 10;

  myPID_L.Compute();
  myPID_R.Compute();

  if(wheel_state == forward)
  {
    ML_fwd(Output_L);
    MR_fwd(Output_R);
  }
  else if(wheel_state == reverse)
  {
    ML_rev(Output_L);
    MR_rev(Output_R);
  }

  #ifdef DEBUG_WHEELS
    Serial.print("L : ");
    Serial.print(left_ticks);
    Serial.print("\tOut : ");
    Serial.print(Output_L);
    Serial.print("\tR : ");
    Serial.print(right_ticks);
    Serial.print("\tOut : ");
    Serial.println(Output_L);
  #endif

  timer0_write(ESP.getCycleCount() + 8000000L); // 8MHz == 100 ms
}

void init_timer() {
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 8000000L); // 8MHz == 100 ms
  interrupts();

  myPID_L.SetMode(AUTOMATIC);
  myPID_R.SetMode(AUTOMATIC);
}
