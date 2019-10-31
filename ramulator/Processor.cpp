#include "Processor.h"
#include <cassert>
#include <string>
#include <iostream>

using namespace std;
using namespace ramulator;

Processor::Processor(const Config& configs, 
                       function<bool(Request)> send_memory, 
                       MemoryBase& memory):memory(memory) 
{
    //cpu behavior can be defined by configs
    //configs are not used here
    send = send_memory;
    callback = std::bind(&Processor::receive, this, placeholders::_1);//callback is triggered when a request is issued in memory.
    req_addr_interval = stoi(configs["req_addr_interval"]);
}

void Processor::receive(Request& req) {
    //this function indicates the processor's following behavior when a memory request is issued and received.
    waiting = false;
    recieve_num ++;
    processing += 0;                   //randomize or indicate pricessing cycles
    //cout << "id: " << recieve_num << "request received at clk " << clk << ", waiting for " << clk-send_clk << endl;
    if (last_access_time != clk-send_clk && last_access_time > 0){
        cout << "access id: " << recieve_num << ", cycles: " << last_access_time << " (" << same_access_time_count << ")" << endl;
        same_access_time_count = 1;
    }else{
        same_access_time_count ++;
    }
    last_access_time = clk-send_clk;
}

void Processor::tick(){
    clk++;

    if(processing > 0){
        processing--;
    }
    else if(!waiting){
        req_addr = req_addr == -1 ? 20734016 : req_addr + req_addr_interval; //randomize or indicating request addr
        req_type = Request::Type::READ; //randomize or indicate request type
        req_addr = memory.page_allocator(req_addr, 0);
        Request req(req_addr, req_type, callback, 0);
        waiting = true;
        //cout<<"request send at clk "<<clk<<endl;
        send_clk = clk;
        send(req);//send request to memory. request's callback will be called when issued.
    }
    else{}
}
