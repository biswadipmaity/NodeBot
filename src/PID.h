#include <list>

#ifndef PIDHeader
#define PIDHeader 0

class PID{
public:
  double kp,ki,kd,filteredVal;
  int lpLength;
  PID(long millis,double KP,double KI=0,double KD=0,double InitState=0,int LPLength=5);
  double calculate(double input,long millis);
  void resetLP();
  void resetController();
  void reset();
  ~PID();

private:
  double lpCalc(double input);
  double integral,lastVal;
  double * windowArray;
  bool initialized=false;
  bool running;
  std::list<double> lpArray;
  long lastTime;
};
#endif
