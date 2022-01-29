#include "processor.h"

#include "linux_parser.h"

Processor::Processor() {
  auto cpuStrings = LinuxParser::CpuUtilization();
  user = LinuxParser::intFromString(cpuStrings[0]);
  nice = LinuxParser::intFromString(cpuStrings[1]);
  system = LinuxParser::intFromString(cpuStrings[2]);
  idle = LinuxParser::intFromString(cpuStrings[3]);
  ioWait = LinuxParser::intFromString(cpuStrings[4]);
  irq = LinuxParser::intFromString(cpuStrings[5]);
  softirq = LinuxParser::intFromString(cpuStrings[6]);
  steal = LinuxParser::intFromString(cpuStrings[7]);
}

float Processor::Utilization() {
  // this method is doing the calculation as proposed in the StackOverflow
  // answer (without accounting for the previous point in time)
  // "https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux"
  long int idleSum = idle + ioWait;
  long int nonIdleSum = user + nice + system + irq + softirq + steal;
  long int totalSum = idleSum + nonIdleSum;
  return 1.0 * (totalSum - idleSum) / totalSum;
}