#ifndef __PROCESSOR_H
#define __PROCESSOR_H

#include "Cache.h"
#include "Config.h"
#include "Memory.h"
#include "Request.h"
#include "Statistics.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctype.h>
#include <functional>

namespace ramulator
{
class Processor{
public:
    //processor clock, for example, usually faster than memory clock
    long clk = 0;
    //this function send request to memory
    function<bool(Request)> send;
    //this function is called when a request is issued in memory
    function<void(Request&)> callback;

    Processor(const Config& configs, function<bool(Request)> send, MemoryBase& memory);
    
    //simulate 1 clock cycle of processor
    void tick();
    //processor behaviors when a request is issued in memory
    void receive(Request& req);
    int recieve_num = 0;

private:
    //processor status
    bool waiting = false;
    int processing = 0;
    int send_clk = 0;
    int last_access_time = -1;
    int same_access_time_count = 0;
    //info of the request sending to memory
    long req_addr = -1;
    long req_addr_interval = 0;
    Request::Type req_type;
    //simulated memory 
    MemoryBase& memory;
};
}

#endif /* __PROCESSOR_H */
