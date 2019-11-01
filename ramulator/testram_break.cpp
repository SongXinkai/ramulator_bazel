#include "ram.h"

#include <iostream>
#include <string>

using namespace ramulator;

int main(int argc, const char *argv[]){
    if (argc < 3) {
        printf("Usage: %s <configs-file> <memory access interval>\n"
            "Example: %s ramulator-configs.cfg 16\n", argv[0], argv[0]);
        return 0;
    }
  Ram<ramulator::DDR3> ram(argv[1]);
  ram.SetFrequency(1.0);
  int addr_interval = std::stoi(argv[2]);
  for (int i = 0; i < 1000; ++i){
    ram.AccessCommand(long(20734016 + i * addr_interval), ramulator::Request::Type::READ);
    ram.WaitFor(10);
  }
  ram.WaitUntil(80000);
  std::cout << "total cycles: " << ram.GetClockRealtime() << std::endl;
  return 0;
}
