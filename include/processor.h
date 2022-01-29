#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();
  float Utilization();

 private:
  long user;
  long nice;
  long system;
  long idle;
  long ioWait;
  long irq;
  long softirq;
  long steal;
};

#endif