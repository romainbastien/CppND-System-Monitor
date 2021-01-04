#include <string>

#include "format.h"

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) {
  int hour = seconds / 3600;
  seconds %= 3600;
  int minutes = seconds / 60 ;
  seconds %= 60;

  return to_string(hour) + ":" + to_string(minutes) + ":" + to_string(seconds);
}