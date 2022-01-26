// File for the methods testing the behavior of the Linux parser

#include <unistd.h>

#include "linux_parser.h"
#include "testHeader.h"

void testValueForKey() {
  string result = LinuxParser::getValueForKey(test_os_file_path, test_os_key,
                                              vector<char>{'=', '"'});
  assert(result == test_exp_os_value);
}

void testIntFromString() {
  assert(10 == LinuxParser::intFromString("10"));
  assert(41622 == LinuxParser::intFromString("41622"));
  assert(0 == LinuxParser::intFromString("0"));
}

void testIntValueForKey() {
  int result = LinuxParser::getIntValueForKey(
      test_proc_stat, test_stat_processes, vector<char>{});
  assert(result == test_exp_processes);
}

void testParser() {
  testValueForKey();
  testIntFromString();
  testIntValueForKey();

  cout << "parser test\n";

  cout << "kernel output " << LinuxParser::OperatingSystem() << "\n";
  cout << "running processes " << LinuxParser::RunningProcesses() << "\n";

  char tmp[256];
  getcwd(tmp, 256);

  cout << "pwd: " << tmp << "\n";

  ProcessData data1 = ProcessData{1};
  cout << "ram in MB " << data1.getRam() << "\n";
  cout << "UID " << data1.getUId() << "\n";
  cout << "Up time in seconds " << data1.getUpTime() << "\n";
}
