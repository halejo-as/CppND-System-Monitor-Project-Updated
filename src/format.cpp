#include "format.h"

#include <string>

using std::string;
char buffer[10]{0};

// What happend when the time arrive to 99 Heures)
// improve code to add days ?

string Format::ElapsedTime(long seconds) {
  int heure = seconds / 3600;
  int min = (seconds - heure * 3600) / 60;
  int sec = seconds - heure * 3600 - min * 60;
  sprintf(buffer, "%02d:%02d:%02d", heure, min, sec);
  return buffer;
}
