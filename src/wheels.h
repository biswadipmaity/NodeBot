#include "Arduino.h"
#include <PID_v1.h>

//Define Variables we'll be connecting to
double left_ticks, right_ticks;

double Setpoint = 3, Output_L=120, Output_R=120;
double consKp=4, consKi=0.2, consKd=1;

//Specify the links and initial tuning parameters
PID myPID_L(&left_ticks, &Output_L, &Setpoint, consKp, consKi, consKd, DIRECT);
PID myPID_R(&right_ticks, &Output_R, &Setpoint, consKp, consKi, consKd, DIRECT);

#define ML_A D5
#define ML_B D6
#define MR_A D3
#define MR_B D4

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
  //Serial.print("R : ");
  //Serial.println(wheel_count_R);
}

unsigned long prev_left_ticks=0;
unsigned long prev_right_ticks=0;

void timer0_ISR (void) {

  left_ticks = wheel_count_L - prev_left_ticks;
  prev_left_ticks = wheel_count_L;

  right_ticks = wheel_count_R - prev_right_ticks;
  prev_right_ticks = wheel_count_R;

  myPID_L.Compute();
  myPID_R.Compute();

  Serial.print("L : ");
  Serial.print(left_ticks);
  Serial.print("\tOut : ");
  Serial.print(Output_L);
  Serial.print("\tR : ");
  Serial.print(right_ticks);
  Serial.print("\tOut : ");
  Serial.println(Output_L);

  timer0_write(ESP.getCycleCount() + 8000000L); // 80MHz == 1sec
}

void init_timer() {
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 8000000L); // 80MHz == 1sec
  interrupts();

  myPID_L.SetMode(AUTOMATIC);
  myPID_R.SetMode(AUTOMATIC);
}
