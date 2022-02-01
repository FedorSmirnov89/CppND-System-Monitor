#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// The constructor
Process::Process(int pid) {
  this->pid = pid;
  ram = LinuxParser::readRam(pid);
  command = LinuxParser::readCommand(pid);
  upTime = LinuxParser::UpTime() - LinuxParser::readUpTime(pid);
  user = LinuxParser::readUserName(pid);
  cpuUtilization = LinuxParser::readCpuUtilization(pid);
}

// the Getters
int Process::Pid() { return pid; }
float Process::CpuUtilization() { return cpuUtilization; }
string Process::Command() { return command; }
string Process::Ram() { return ram; }
string Process::User() { return user; }
long int Process::UpTime() { return upTime; }

bool Process::operator<(Process const& otherProcess) const {
  return (cpuUtilization >= otherProcess.cpuUtilization);
}