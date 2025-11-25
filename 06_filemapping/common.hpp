#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstddef>
#include <boost/interprocess/sync/interprocess_condition.hpp>

namespace ipc_constants {
    const char* const SHARED_MEM_NAME = "my_system_info_shm";
    const char* const MUTEX_NAME = "my_system_info_mutex";
    const std::size_t SHARED_MEM_SIZE = 65536;
}

struct Client1Payload {
    unsigned long long total_ram_kb;
    bool has_external_disk;
    int screen_width_px;
};

struct Client2Payload {
    int status_bar_height_px;
    int notification_panel_width_px;
    int horizontal_dpi;
};

struct SharedData {
    // 0 = No new data, 1 = Client 1, 2 = Client 2
    int new_data_flag;
    
    Client1Payload client1_payload;
    Client2Payload client2_payload;

    boost::interprocess::interprocess_condition data_cond;

    boost::interprocess::interprocess_mutex mutex;
};

#endif