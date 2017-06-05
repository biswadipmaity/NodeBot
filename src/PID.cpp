#include "PID.h"

using namespace std;

PID::PID(long millis,double KP, double KI,double KD,double InitState,int LPLength){
  kp=KP;
  ki=KI;
  kd=KD;
  integral=InitState;
  lpLength=LPLength;
  lastVal=0;
  lastTime=millis;
  windowArray=new double[lpLength];
  for(int pos=0;pos<lpLength;pos++) {
    windowArray[pos]=1/lpLength;
  }
  list<double> lpArray(0,lpLength);
  initialized=true;
  running=false;
}

PID::~PID(){
  delete [] windowArray;
  lpArray.clear();
}

double PID::lpCalc(double input){
  lpArray.pop_back();
  lpArray.push_front(input);
  double output=0;
  int pos=0;
  for(list<double>::iterator it=lpArray.begin();it!=lpArray.end();it++,pos++){
    output+=windowArray[pos]*(*it);
  }
  return output;
}

double PID::calculate(double input,long millis){
  double value=lpCalc(input);
  integral+=input;
  double diff=value-lastVal;
  double timeDiff=millis-lastTime;
  double avg=(input+lastVal)/2.0;
  lastVal=value;
  lastTime=millis;
  if(!running){
    running=true;
    return 0;
  }
  return -(kp*(input+kd*(diff/timeDiff)+ki*avg*timeDiff));
}

void PID::resetLP(){
  list<double> lpArray(0,lpLength);
}

void PID::resetController(){
  bool initialized=false;
  bool running=false;
  lastTime=0;
  lastVal=0;
  integral=0;
}

void PID::reset(){
  resetLP();
  resetController();
}
