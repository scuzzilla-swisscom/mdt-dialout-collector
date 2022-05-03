#include <iostream>
#include "mdt_dialout_core.h"


int main(void)
{
    std::string srv_addr {"0.0.0.0:10000"};
    Srv mdt_dialout_collector;
    mdt_dialout_collector.Bind(srv_addr);
    //std::cout << "MDT Server listening on " << server_addr << std::endl;

    return (0);
}
