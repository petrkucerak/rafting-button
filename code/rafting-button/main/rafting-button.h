#ifndef RAFTING_BUTTON_H
#define RAFTING_BUTTON_H

#define BALANCER_SIZE 100
#define NEIGHBOURS_COUNT 9

#include <espnow.h>
#include <inttypes.h>

typedef enum device_title {
   SLAVE, // nasledovnik
   CANDIDATE, // kandidat na lidra
   MASTER,    // lidr
} device_title_t;

typedef enum device_status {
   NOT_INITIALIZED, // device wasn't initialized
   INACTIVE,        // device was active in DS
   ACTIVE,          // device is active in DS
} device_status_t;

typedef enum espnow_event_id {
   ESPNOW_SEND_CB,
   ESPNOW_RECV_CB,
} espnow_event_id_t;

typedef enum message_type {
   RTT_CAL_MASTER, // time used to calculate the RTT (master -> slave)
   RTT_CAL_SLAVE,  // time used to calculate the RTT (slave -> master)
   RTT,            // calculated value of RTT
   TIME,           // time to synchronize the time
   HELLO_DS,
   NEIGHBOURS,
   LOG,
   REQUEST_VOTE,
   GIVE_VOTE,
} message_type_t;

typedef struct espnow_event_send_cb {
   uint8_t mac_addr[ESP_NOW_ETH_ALEN];
   esp_now_send_status_t status;
} espnow_event_send_cb_t;

typedef struct espnow_event_recv_cb {
   uint8_t mac_addr[ESP_NOW_ETH_ALEN];
   uint8_t *data;
   int data_len;
} espnow_event_recv_cb_t;

typedef union espnow_event_info {
   espnow_event_send_cb_t send_cb;
   espnow_event_recv_cb_t recv_cb;
} espnow_event_info_t;

typedef struct espnow_event {
   espnow_event_id_t id;
   espnow_event_info_t info;
   uint64_t timestamp;
} espnow_event_t;

typedef struct neighbour {
   device_title_t title;
   device_status_t status;
   uint8_t mac_addr[ESP_NOW_ETH_ALEN];
} neighbour_t;

typedef struct message_data {
   message_type_t type;
   uint32_t epoch_id;
   uint64_t content;
   neighbour_t neighbour[NEIGHBOURS_COUNT];
   uint8_t payload[0];
} __attribute__((packed)) message_data_t;

typedef struct espnow_send_param {
   message_type_t type;
   uint64_t content;
   uint32_t epoch_id;
   neighbour_t neighbour[NEIGHBOURS_COUNT];
   int data_len;
   uint8_t *buf;
   uint8_t dest_mac[ESP_NOW_ETH_ALEN];
} espnow_send_param_t;

typedef struct node_info {
   uint64_t time_corection;
   uint32_t rtt_balancer[BALANCER_SIZE];
   uint16_t rtt_balancer_index;
   bool is_firts_setup_rtt;
   bool is_time_synced;
   int32_t deviation_avg;
   neighbour_t neighbour[NEIGHBOURS_COUNT];
   uint8_t neighbour_error_count[NEIGHBOURS_COUNT];
   uint32_t epoch_id;
   device_title_t title;
   uint64_t timeout_sync; // represents timestamp
   uint64_t timeout_vote; // represents by timestamp
   uint8_t count_of_vote;
} node_info_t;

typedef struct print_data {
   uint32_t rtt;
   int32_t deviation;
   uint64_t time;
} print_data_t;

#endif // RAFTING_BUTTON_H