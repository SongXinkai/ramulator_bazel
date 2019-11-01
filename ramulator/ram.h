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
  long AccessMemory(const long req_addr, const Request::Type req_type);
  long AccessMemory(const long send_clk_processor, const long req_addr, const Request::Type req_type);
  long ConvertCyclesRam2Processor(const long cycles);
  long ConvertCyclesProcessor2Ram(const long cycles);
  void SetProcessorFrequencyRatio(const double r);
  long GetClockProcessor();
  long GetClockRam();
protected:
  void InitMemory(const Config& configs, T* spec);
  double processor_frequency_ratio_;
  long clk_;
  std::shared_ptr<Memory<T, Controller> > memory_;
};
