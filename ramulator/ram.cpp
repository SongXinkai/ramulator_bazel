#include "ram.h"
#include <assert.h>
#include <functional>

template<typename T>
Ram<T>::Ram(const std::string& config_file){
  clk_ = 0;
  processor_frequency_ratio_ = 1;
  Config configs(config_file);
  const std::string& standard = configs["standard"];
  assert(standard != "" || "DRAM standard should be specified.");
  //not usefull
  configs.add("trace_type", "CPU");
  //mapping strategy
  configs.add("mapping", "defaultmapping");
  //not usefull
  configs.set_core_num(1);

  T* ddr = new T(configs["org"], configs["speed"]);
   InitMemory(configs, ddr); // not implement: SALP-MASA, WideIO2, DSARP, TLDRAM
  //run simulation according to the memory standard.
  /*
  if (standard == "DDR3") {
    DDR3* ddr3 = new DDR3(configs["org"], configs["speed"]);
    InitMemory(configs, ddr3);
  } else if (standard == "DDR4") {
    DDR4* ddr4 = new DDR4(configs["org"], configs["speed"]);
    InitMemory(configs, ddr4);
  } else if (standard == "SALP-MASA") {
    SALP* salp8 = new SALP(configs["org"], configs["speed"], "SALP-MASA", configs.get_subarrays());
    InitMemory(configs, salp8);
  } else if (standard == "LPDDR3") {
    LPDDR3* lpddr3 = new LPDDR3(configs["org"], configs["speed"]);
    InitMemory(configs, lpddr3);
  } else if (standard == "LPDDR4") {
    // total cap: 2GB, 1/2 of others
    LPDDR4* lpddr4 = new LPDDR4(configs["org"], configs["speed"]);
    InitMemory(configs, lpddr4);
  } else if (standard == "GDDR5") {
    GDDR5* gddr5 = new GDDR5(configs["org"], configs["speed"]);
    InitMemory(configs, gddr5);
  } else if (standard == "HBM") {
    HBM* hbm = new HBM(configs["org"], configs["speed"]);
    InitMemory(configs, hbm);
  } else if (standard == "WideIO") {
    // total cap: 1GB, 1/4 of others
    WideIO* wio = new WideIO(configs["org"], configs["speed"]);
    InitMemory(configs, wio);
  } else if (standard == "WideIO2") {
    // total cap: 2GB, 1/2 of others
    WideIO2* wio2 = new WideIO2(configs["org"], configs["speed"], configs.get_channels());
    wio2->channel_width *= 2;
    InitMemory(configs, wio2);
  } else if (standard == "STTMRAM") {
    STTMRAM* sttmram = new STTMRAM(configs["org"], configs["speed"]);
    InitMemory(configs, sttmram);
  } else if (standard == "PCM") {
    PCM* pcm = new PCM(configs["org"], configs["speed"]);
    InitMemory(configs, pcm);
  }
  // Various refresh mechanisms
    else if (standard == "DSARP") {
    DSARP* dsddr3_dsarp = new DSARP(configs["org"], configs["speed"], DSARP::Type::DSARP, configs.get_subarrays());
    InitMemory(configs, dsddr3_dsarp);
  } else if (standard == "ALDRAM") {
    ALDRAM* aldram = new ALDRAM(configs["org"], configs["speed"]);
    InitMemory(configs, aldram);
  } else if (standard == "TLDRAM") {
    TLDRAM* tldram = new TLDRAM(configs["org"], configs["speed"], configs.get_subarrays());
    InitMemory(configs, tldram);
  }
  */
}

template<typename T>
Ram<T>::~Ram(){
}

template<typename T>
void Ram<T>::InitMemory(const Config& configs, T* spec){
  int C = configs.get_channels(), R = configs.get_ranks();
  // Check and Set channel, rank number
  spec->set_channel_number(C);
  spec->set_rank_number(R);
  std::vector<Controller<T>*> ctrls;
  for (int c = 0 ; c < C ; c++) {
    DRAM<T>* channel = new DRAM<T>(spec, T::Level::Channel);
    channel->id = c;
    channel->regStats("");//not usefull
    Controller<T>* ctrl = new Controller<T>(configs, channel);
    ctrls.push_back(ctrl);
  }
  memory_ = std::make_shared<Memory<T, Controller> >(configs, ctrls);
}

template<typename T>
long Ram<T>::AccessMemory(const long req_addr, const Request::Type req_type){
  long tmp_clk = 0;
  bool data_return = false;
  long req_addr_param = req_addr;
  auto req_type_param = req_type;
  auto callback = std::function<void(Request&)>([&data_return](Request& rq){data_return = true;});
  Request req(req_addr_param, req_type_param, callback, 0);
  memory_->send(req);
  while (!data_return){
    memory_->tick();
    clk_ ++;
    tmp_clk ++;
  }
  return ConvertCyclesRam2Processor(tmp_clk);
}

template<typename T>
long Ram<T>::AccessMemory(const long send_clk_processor, const long req_addr, const Request::Type req_type){
  long send_clk = ConvertCyclesRam2Processor(send_clk_processor);
  long tmp_clk = 0;
  while (clk_ < send_clk){
    memory_->tick();
    clk_ ++;
    tmp_clk ++;
  }
  bool data_return = false;
  long req_addr_param = req_addr;
  auto req_type_param = req_type;
  auto callback = std::function<void(Request&)>([&data_return](Request& rq){data_return = true;});
  Request req(req_addr_param, req_type_param, callback, 0);
  memory_->send(req);
  while (!data_return){
    memory_->tick();
    clk_ ++;
    tmp_clk ++;
  }
  return ConvertCyclesRam2Processor(tmp_clk);
}

template<typename T>
long Ram<T>::ConvertCyclesRam2Processor(const long cycles){
  return long(double(cycles) * processor_frequency_ratio_);
}

template<typename T>
long Ram<T>::ConvertCyclesProcessor2Ram(const long cycles){
  return long(double(cycles) / processor_frequency_ratio_);
}

template<typename T>
void Ram<T>::SetProcessorFrequencyRatio(const double r){
  void SetProcessorFrequencyRatio(const double r);
  this->processor_frequency_ratio_ = r;
}

template<typename T>
long Ram<T>::GetClockProcessor(){
  return this->clk_ * processor_frequency_ratio_;
}

template<typename T>
long Ram<T>::GetClockRam(){
  return this->clk_;
}

// yet not implement: SALP-MASA, WideIO2, DSARP, TLDRAM
template class Ram<ramulator::DDR3>;
template class Ram<ramulator::DDR4>;
template class Ram<ramulator::GDDR5>;
template class Ram<ramulator::LPDDR3>;
template class Ram<ramulator::LPDDR4>;
//template class Ram<ramulator::DSARP>;
template class Ram<ramulator::WideIO>;
//template class Ram<ramulator::WideIO2>;
template class Ram<ramulator::HBM>;
//template class Ram<ramulator::SALP>;
template class Ram<ramulator::ALDRAM>;
//template class Ram<ramulator::TLDRAM>;
template class Ram<ramulator::STTMRAM>;
template class Ram<ramulator::PCM>;
