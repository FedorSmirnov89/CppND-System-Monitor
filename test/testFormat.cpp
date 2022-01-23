// file containing the tests for the formatting

#include <format.h>
#include "testHeader.h"

void testFormat(long input, string expected) {
  string actual = Format::ElapsedTime(input);
  assert(actual == expected);
}

void testFormatSuite() {
  testFormat(54, "00:00:54");
  testFormat(62, "00:01:02");
  testFormat(3660, "01:01:00");
}