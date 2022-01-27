#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cout;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

ProcessData::ProcessData(int pid) {
  ram = readRam(pid);
  uId = readUId(pid);
  upTime = readUpTime(pid);
  command = readCommand(pid);
  user = readUserName(uId);
  readJiffies(pid, userJiffies, kernelJiffies);
}

void ProcessData::readJiffies(int pid, long& userJiffies, long& kernelJiffies) {
  string filePath = LinuxParser::kProcDirectory + std::to_string(pid) +
                    LinuxParser::kStatFilename;
  string line;
  string placeholder;
  string userStr;
  string kernelStr;

  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i < 16; i++) {
        if (i == 14) {
          linestream >> userStr;
        } else if (i == 15) {
          linestream >> kernelStr;
          userJiffies = LinuxParser::intFromString(userStr);
          kernelJiffies = LinuxParser::intFromString(kernelStr);
          return;
        } else {
          linestream >> placeholder;
        }
      }
    }
  }
}

string ProcessData::readUserName(string uId) {
  string line;
  string userName;
  string passwd;
  string userId;

  std::ifstream fileStream(LinuxParser::kPasswordPath);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> userName >> passwd >> userId) {
        if (userId == uId) {
          return userName;
        }
      }
    }
  }
  return userName;
}

string ProcessData::readCommand(int pid) {
  string filePath = LinuxParser::kProcDirectory + std::to_string(pid) +
                    LinuxParser::kCmdlineFilename;
  string line;
  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    if (std::getline(fileStream, line)) {
      return line;
    }
  }
  return line;
}

long ProcessData::readUpTime(int pid) {
  string filePath = LinuxParser::kProcDirectory + std::to_string(pid) +
                    LinuxParser::kStatFilename;
  string line;
  string placeholder;
  string upTimeS;

  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 21; i++) {
        linestream >> placeholder;
      }
      linestream >> upTimeS;
      int upTimeI = LinuxParser::intFromString(upTimeS);
      return upTimeI / sysconf(_SC_CLK_TCK);
    }
  }
  return -1;
}

string ProcessData::readUId(int pid) {
  string filePath = LinuxParser::kProcDirectory + std::to_string(pid) +
                    LinuxParser::kStatusFilename;
  return LinuxParser::getValueForKey(filePath, LinuxParser::keyUId,
                                     vector<char>{});
}

string ProcessData::readRam(int pid) {
  string filePath = LinuxParser::kProcDirectory + std::to_string(pid) +
                    LinuxParser::kStatusFilename;
  string ramKbS = LinuxParser::getValueForKey(filePath, LinuxParser::keyRamKb,
                                              vector<char>{});
  int ramKbI = LinuxParser::intFromString(ramKbS);
  float ramMbF = 1.0 * ramKbI / 1000;
  return std::to_string(ramMbF);
}

string ProcessData::getCommand() const { return command; };
string ProcessData::getRam() const { return ram; };
string ProcessData::getUId() const { return uId; };
long ProcessData::getUpTime() const { return upTime; };
string ProcessData::getUser() const { return user; };
long ProcessData::getUserJiffies() const { return userJiffies; };
long ProcessData::getKernelJiffies() const { return kernelJiffies; };

CpuUtil::CpuUtil(vector<string> cpuStrings) {
  user = LinuxParser::intFromString(cpuStrings[0]);
  nice = LinuxParser::intFromString(cpuStrings[1]);
  system = LinuxParser::intFromString(cpuStrings[2]);
  idle = LinuxParser::intFromString(cpuStrings[3]);
  ioWait = LinuxParser::intFromString(cpuStrings[4]);
  irq = LinuxParser::intFromString(cpuStrings[5]);
  softirq = LinuxParser::intFromString(cpuStrings[6]);
}

long CpuUtil::getUser() { return user; }
long CpuUtil::getNice() { return nice; }
long CpuUtil::getSystem() { return system; }
long CpuUtil::getIdle() { return idle; }
long CpuUtil::getIoWait() { return ioWait; }
long CpuUtil::getIrq() { return irq; }
long CpuUtil::getSoftIrq() { return softirq; }

// INPUT:
//  filepath - the file to the file to read
//  searchKey - the key of the line of interest
// replace - vector of characters which are replaced by a space
// OUPUT:
//  the value in the line with the given key
string LinuxParser::getValueForKey(string filePath, string searchKey,
                                   vector<char> replace) {
  string line;
  string key;
  string value;

  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      if (replace.size() > 0) {
        std::replace(line.begin(), line.end(), ' ', '_');
        for (char replChar : replace) {
          std::replace(line.begin(), line.end(), replChar, ' ');
        }
      }
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == searchKey) {
          if (replace.size() > 0) {
            std::replace(value.begin(), value.end(), '_', ' ');
          }
          return value;
        }
      }
    }
  }
  return value;
}

// parses the given string to an int
int LinuxParser::intFromString(string string) {
  int result;
  std::stringstream stream(string);
  stream >> result;
  return result;
}

int LinuxParser::getIntValueForKey(string filePath, string key,
                                   vector<char> replace) {
  string strVal = getValueForKey(filePath, key, replace);
  return intFromString(strVal);
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  return getValueForKey(kOSPath, keyOs, vector<char>{'=', '"'});
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  int memTotal = getIntValueForKey(kProcDirectory + kMeminfoFilename,
                                   keyMemTotal, vector<char>{});
  int memFree = getIntValueForKey(kProcDirectory + kMeminfoFilename, keyMemFree,
                                  vector<char>{});
  int memUsed = memTotal - memFree;
  return 1.0 * memUsed / memTotal;
}

long LinuxParser::UpTime() {
  string line;
  long uptime;
  long idleTime;

  std::ifstream fileStream(kProcDirectory + kUptimeFilename);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::istringstream linestream(line);
      if (linestream >> uptime >> idleTime) {
        return uptime;
      }
    }
  }
  return uptime;
}

long LinuxParser::Jiffies() {
  auto util = CpuUtil(CpuUtilization());
  return util.getUser() + util.getNice() + util.getSystem() + util.getIdle() +
         util.getIoWait();
}

long LinuxParser::ActiveJiffies(int pid) {
  ProcessData data = ProcessData{pid};
  return data.getUserJiffies() + data.getKernelJiffies();
}

long LinuxParser::ActiveJiffies() {
  auto util = CpuUtil(CpuUtilization());
  return util.getUser() + util.getNice() + util.getSystem();
}

long LinuxParser::IdleJiffies() { return CpuUtil{CpuUtilization()}.getIdle(); }

vector<string> LinuxParser::CpuUtilization() {
  string user;
  string nice;
  string system;
  string idle;
  string ioWait;
  string irq;
  string softirq;

  string key;
  string line;

  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          linestream >> user >> nice >> system >> idle >> ioWait >> irq >>
              softirq;
          return vector<string>{user, nice, system, idle, ioWait, irq, softirq};
        }
      }
    }
  }
  return vector<string>{};
}

int LinuxParser::TotalProcesses() {
  return getIntValueForKey(kProcDirectory + kStatFilename, keyTotalProcesses,
                           vector<char>{});
}

int LinuxParser::RunningProcesses() {
  return getIntValueForKey(kProcDirectory + kStatFilename, keyRunningProcesses,
                           vector<char>{});
}

string LinuxParser::Command(int pid) { return ProcessData{pid}.getCommand(); }

string LinuxParser::Ram(int pid) { return ProcessData{pid}.getRam(); }

string LinuxParser::Uid(int pid) { return ProcessData{pid}.getUId(); }

string LinuxParser::User(int pid) { return ProcessData{pid}.getUser(); }

long LinuxParser::UpTime(int pid) { return ProcessData{pid}.getUpTime(); }
