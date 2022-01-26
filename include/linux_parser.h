#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>
#include <vector>


using std::string;
using std::vector;

namespace LinuxParser {

// Util functions
string getValueForKey(string filePath, string searchKey, vector<char> replace);
int getIntValueForKey(string filePath, string searchKey, vector<char> replace);
int intFromString(string string);

// Line keys
const string keyOs = "PRETTY_NAME";
const string keyTotalProcesses = "processes";
const string keyRunningProcesses = "procs_running";
const string keyMemTotal = "MemTotal:";
const string keyMemFree = "MemFree:";
const string keyRamKb = "VmSize:";
const string keyUId = "Uid:";

// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);

};  // namespace LinuxParser

class ProcessData {
 public:
  ProcessData(int pid);

  string getRam() const;
  string getUId() const;
  long getUpTime() const;
  string getCommand() const;
  string getUser() const;

 private:
  string readRam(int pid);
  string readUId(int pid);
  long readUpTime(int pid);

  string ram;
  string uId;
  long upTime;
  string command;
  string user;
};

class CpuUtil {
 public:
  CpuUtil(vector<string> cpuStrings);

  long getUser();
  long getNice();
  long getSystem();
  long getIdle();
  long getIoWait();
  long getIrq();
  long getSoftIrq();

 private:
  long user;
  long nice;
  long system;
  long idle;
  long ioWait;
  long irq;
  long softirq;
};

#endif