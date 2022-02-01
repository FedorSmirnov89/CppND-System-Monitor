#include "ncurses_display.h"
#include "system.h"

int main() {
  System system = System();
  NCursesDisplay::Display(system);
}