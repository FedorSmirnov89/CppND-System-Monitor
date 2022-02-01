#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::cout;
using std::set;
using std::size_t;
using std::sort;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

System::System() {
  cpu_ = Processor();
  processes_ = {};
}

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  processes_.clear();

  for (int pid : LinuxParser::Pids()) {
    Process proc = Process(pid);
    if (proc.CpuUtilization() > 0) {
      processes_.push_back(Process(pid));
    }
  }
  sort(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }
long int System::UpTime() { return LinuxParser::UpTime(); }
