#include "Config.h"
#include "Controller.h"
#include "SpeedyController.h"
#include "Memory.h"
#include "DRAM.h"
#include "Request.h"

/* Standards */
#include "Gem5Wrapper.h"
#include "DDR3.h"
#include "DDR4.h"
#include "DSARP.h"
#include "GDDR5.h"
#include "LPDDR3.h"
#include "LPDDR4.h"
#include "WideIO.h"
#include "WideIO2.h"
#include "HBM.h"
#include "SALP.h"
#include "ALDRAM.h"
#include "TLDRAM.h"
#include "STTMRAM.h"
#include "PCM.h"

#include <string>
#include <memory> // std::shared_ptr

using namespace ramulator;

template<typename T>
class Ram{
public:
  Ram(const std::string& config_file);
  ~Ram();
  // Loop: memory_->tick()
  void WaitUntil(const double time);
  void WaitFor(const double time);
  // memory_->send() and memory_->tick()
  void AccessCommand(const long req_addr, const Request::Type req_type);
  // memory_->send() and Loop: memory_->tick(), return: realtime from send to return
  double AccessAndWaitUntilReturn(const long req_addr, const Request::Type req_type);
  double ConvertRamCycle2Realtime(const long ram_cycle);
  long ConvertRealtime2RamCycle(const double realtime);
  void SetFrequency(const double r);
  double GetClockRealtime(); // unit: ns
  long GetClockRam(); // unit: ram cycle
protected:
  void InitMemory(const Config& configs, T* spec);
  double frequency_; // unit: Ghz
  long clk_;
  std::shared_ptr<Memory<T, Controller> > memory_;
};
