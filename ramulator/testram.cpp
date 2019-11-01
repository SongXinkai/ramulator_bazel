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
  ram.SetProcessorFrequencyRatio(4.0);
  int addr_interval = std::stoi(argv[2]);
  long last_access_time = -1;
  int same_access_time_count = 0;
  for (int i = 0; i < 1000; ++i){
    long access_time = ram.AccessMemory(long(20734016 + i * addr_interval), ramulator::Request::Type::READ);
    if (last_access_time != access_time && last_access_time > 0){
        cout << "access id: " << i << ", cycles: " << last_access_time << " (" << same_access_time_count << ")" << endl;
        same_access_time_count = 1;
    }else{
        same_access_time_count ++;
    }
    last_access_time = access_time;
  }
  std::cout << "total cycles: " << ram.GetClockProcessor() << std::endl;
  return 0;
}
