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

string LinuxParser::readRam(int pid) {
  string filePath = kProcDirectory + std::to_string(pid) + kStatusFilename;
  string ramKbS = getValueForKey(filePath, keyRamKb, vector<char>{});
  int ramKbI = intFromString(ramKbS);
  float ramMbF = 1.0 * ramKbI / 1000;
  char buffer[50];
  sprintf(buffer, "%0.2lf", ramMbF);
  return string(buffer);
}

string LinuxParser::readCommand(int pid) {
  string filePath = kProcDirectory + std::to_string(pid) + kCmdlineFilename;
  string line;
  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    if (std::getline(fileStream, line)) {
      return line;
    }
  }
  return line;
}

long int LinuxParser::readUpTime(int pid) {
  string filePath = kProcDirectory + std::to_string(pid) + kStatFilename;
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
      int upTimeI = intFromString(upTimeS);
      return upTimeI / sysconf(_SC_CLK_TCK);
    }
  }
  return -1;
}

string LinuxParser::readUserId(int pid) {
  string filePath = kProcDirectory + std::to_string(pid) + kStatusFilename;
  return getValueForKey(filePath, keyUId, vector<char>{});
}

string LinuxParser::readUserName(int pid) {
  string line;
  string userName;
  string passwd;
  string userId;
  string uId = readUserId(pid);

  std::ifstream fileStream(kPasswordPath);
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

LinuxParser::ProcessUtilData LinuxParser::readUtilData(int pid) {
  string filePath = kProcDirectory + std::to_string(pid) + kStatFilename;
  string line;
  string placeholder;
  string userStr;
  string kernelStr;
  string userTicksChildren;
  string kernelTicksChildred;
  string startTime;

  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    if (std::getline(fileStream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i < 23; i++) {
        switch (i) {
          case 14:
            linestream >> userStr;
            break;
          case 15:
            linestream >> kernelStr;
            break;
          case 16:
            linestream >> userTicksChildren;
            break;
          case 17:
            linestream >> kernelTicksChildred;
            break;
          case 22:
            linestream >> startTime;
            break;
          default:
            linestream >> placeholder;
            break;
        }
      }
    }
  }
  return LinuxParser::ProcessUtilData{
      intFromString(userStr), intFromString(kernelStr),
      intFromString(userTicksChildren), intFromString(kernelTicksChildred),
      intFromString(startTime)};
}

float LinuxParser::readCpuUtilization(int pid) {
  int upTime = UpTime();
  LinuxParser::ProcessUtilData pData = readUtilData(pid);

  long int hertz = sysconf(_SC_CLK_TCK);
  long int totalTime = pData.userTicks + pData.kernelTicks +
                       pData.userTicksChildren + pData.kernelTicksChildren;
  long seconds = upTime - (pData.startTime / hertz);
  if (seconds == 0) {
    return 0;
  }
  float result = ((1.0 * totalTime / hertz) / seconds);

  if (result < 0.0) {
    throw "negative util";
  }

  if (result > 1.0) {
    throw "Overutilization";
  }
  return result;
}

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

vector<string> LinuxParser::CpuUtilization() {
  string user;
  string nice;
  string system;
  string idle;
  string ioWait;
  string irq;
  string softirq;
  string steal;

  string key;
  string line;

  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          linestream >> user >> nice >> system >> idle >> ioWait >> irq >>
              softirq >> steal;
          return vector<string>{user,   nice, system,  idle,
                                ioWait, irq,  softirq, steal};
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
