#include "Processor.h"
#include "Config.h"
#include "Controller.h"
#include "SpeedyController.h"
#include "Memory.h"
#include "DRAM.h"
#include "Statistics.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include <functional>
#include <map>

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

using namespace std;
using namespace ramulator;

template <typename T>
void run_cpu(const Config& configs, Memory<T, Controller>& memory)
{
    int cpu_tick = configs.get_cpu_tick();
    int mem_tick = configs.get_mem_tick();
    auto send = bind(&Memory<T, Controller>::send, &memory, placeholders::_1);
    Processor proc(configs, send, memory);

    printf("Starting the simulation...\n");

    int tick_mult = cpu_tick * mem_tick;
    for (long i = 0; i < 10000000000 ; i++) {
        if (((i % tick_mult) % mem_tick) == 0) { // When the CPU is ticked cpu_tick times,
                                                 // the memory controller should be ticked mem_tick times
            proc.tick();
        }
        if (((i % tick_mult) % cpu_tick) == 0) { // TODO_hasan: Better if the processor ticks the memory controller
            memory.tick();
        }
        if (proc.recieve_num >= 1000){
            cout << "total cycles: " << proc.clk << endl;
            break;
        }
    }
    memory.finish();
}

template<typename T>
void start_run(const Config& configs, T* spec) {
  // initiate controller and memory
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
  Memory<T, Controller> memory(configs, ctrls);

  //assert(files.size() != 0);
  //if (configs["trace_type"] == "CPU") {
    run_cpu(configs, memory);
  //} else if (configs["trace_type"] == "DRAM") {
  //  run_dramtrace(configs, memory, files[0]);
  //}
}

int main(int argc, const char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s <configs-file> <memory access interval>\n"
            "Example: %s ramulator-configs.cfg 16\n", argv[0], argv[0]);
        return 0;
    }
    Config configs(argv[1]);
    configs.add("req_addr_interval", argv[2]);
    //DRAM standard 
    const std::string& standard = configs["standard"];
    assert(standard != "" || "DRAM standard should be specified.");
    //not usefull
    configs.add("trace_type", "CPU");
    //mapping strategy
    configs.add("mapping", "defaultmapping");
    //not usefull
    configs.set_core_num(1);

    //run simulation according to the memory standard.
    if (standard == "DDR3") {
      DDR3* ddr3 = new DDR3(configs["org"], configs["speed"]);
      start_run(configs, ddr3);
    } else if (standard == "DDR4") {
      DDR4* ddr4 = new DDR4(configs["org"], configs["speed"]);
      start_run(configs, ddr4);
    } else if (standard == "SALP-MASA") {
      SALP* salp8 = new SALP(configs["org"], configs["speed"], "SALP-MASA", configs.get_subarrays());
      start_run(configs, salp8);
    } else if (standard == "LPDDR3") {
      LPDDR3* lpddr3 = new LPDDR3(configs["org"], configs["speed"]);
      start_run(configs, lpddr3);
    } else if (standard == "LPDDR4") {
      // total cap: 2GB, 1/2 of others
      LPDDR4* lpddr4 = new LPDDR4(configs["org"], configs["speed"]);
      start_run(configs, lpddr4);
    } else if (standard == "GDDR5") {
      GDDR5* gddr5 = new GDDR5(configs["org"], configs["speed"]);
      start_run(configs, gddr5);
    } else if (standard == "HBM") {
      HBM* hbm = new HBM(configs["org"], configs["speed"]);
      start_run(configs, hbm);
    } else if (standard == "WideIO") {
      // total cap: 1GB, 1/4 of others
      WideIO* wio = new WideIO(configs["org"], configs["speed"]);
      start_run(configs, wio);
    } else if (standard == "WideIO2") {
      // total cap: 2GB, 1/2 of others
      WideIO2* wio2 = new WideIO2(configs["org"], configs["speed"], configs.get_channels());
      wio2->channel_width *= 2;
      start_run(configs, wio2);
    } else if (standard == "STTMRAM") {
      STTMRAM* sttmram = new STTMRAM(configs["org"], configs["speed"]);
      start_run(configs, sttmram);
    } else if (standard == "PCM") {
      PCM* pcm = new PCM(configs["org"], configs["speed"]);
      start_run(configs, pcm);
    }
    // Various refresh mechanisms
      else if (standard == "DSARP") {
      DSARP* dsddr3_dsarp = new DSARP(configs["org"], configs["speed"], DSARP::Type::DSARP, configs.get_subarrays());
      start_run(configs, dsddr3_dsarp);
    } else if (standard == "ALDRAM") {
      ALDRAM* aldram = new ALDRAM(configs["org"], configs["speed"]);
      start_run(configs, aldram);
    } else if (standard == "TLDRAM") {
      TLDRAM* tldram = new TLDRAM(configs["org"], configs["speed"], configs.get_subarrays());
      start_run(configs, tldram);
    }

    printf("Simulation done.\n");

    return 0;
}
