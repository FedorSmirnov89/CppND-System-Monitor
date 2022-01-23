#include "format.h"

#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: string in "HH:MM:SS" format
string Format::ElapsedTime(long seconds) {
  // calculate the values
  long hours = seconds / 3600;
  long minutes = seconds % 3600 / 60;
  long secondsDisp = seconds % 60;
  // format the output
  char buffer[50];
  sprintf(buffer, "%02ld:%02ld:%02ld", hours, minutes, secondsDisp);
  string result = string(buffer);
  return result;
}