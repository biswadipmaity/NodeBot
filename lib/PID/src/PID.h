#include <list>

#ifndef PIDHeader
#define PIDHeader

#define Kc 30
#define Tc 0.6

class PID{
public:
  double kp,ki,kd,bias,filteredVal;
  int lpLength;

  PID(bool dummy,double KP=0.6*Kc,double KI=2/Tc,double KD=Tc/8,double Bias=0,double InitState=0,int LPLength=3);
  double calculate(double input);
  void resetLP();
  void resetController();
  void reset();
  ~PID();

private:
  double integral,lastVal;
  double * windowArray;
  bool initialized=false;
  bool running=false;
  std::list<double> lpArray;
  long lastTime;

  double lpCalc(double input);
};
#endif
