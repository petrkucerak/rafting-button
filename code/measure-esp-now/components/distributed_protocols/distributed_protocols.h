// The file describes the distributed system (DS), includes function, and define
// custom structures for the distributed system.

// DISTRIBUTED SYSTEM DESCRIPTION
// The system provides 4 custom major features:
//    1. time synchronization
//    2. leader selection
//    3. the list of neighbors (in ESP-NOW terminology the peer liest)
//    distribution
//    4. distribution of logs with DS events
//

// 1. TIME SYNCHRONZIATION
//    Master                                 Slave
//                      time
//        --------------------------------->
//                      time
//        <---------------------------------
//    RTT            time + RTT              set time
//        --------------------------------->
//                      ACK
//        <---------------------------------

// RTT ... round-trip time

#ifndef DISTRIBUTED_PROTOCOLS_H
#define DISTRIBUTED_PROTOCOLS_H

#include <esp_types.h>

typedef enum ds_message_type { time, time_RTT, ACK } ds_message_type_t;

typedef struct ds_message {
   ds_message_type_t type;
   uint8_t data[200];
} ds_message_t;

#endif // DISTRIBUTED_PROTOCOLS_H