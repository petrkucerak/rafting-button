#ifndef RAFTING_BUTTON_H
#define RAFTING_BUTTON_H

#define BALANCER_SIZE 100

#include <espnow.h>
#include <inttypes.h>

// Device 1 <70:b8:f6:5b:d3:24>
static uint8_t mac_addr_1[] = {0x70, 0xb8, 0xf6, 0x5b, 0xd3, 0x24};

// Device 2 <cc:db:a7:1d:c4:08>
static uint8_t mac_addr_2[] = {0xcc, 0xdb, 0xa7, 0x1d, 0xc4, 0x08};

// Device 3 <c8:f0:9e:7b:10:8c>
static uint8_t mac_addr_3[] = {0xc8, 0xf0, 0x9e, 0x7b, 0x10, 0x8c};

// Device 4 <cc:db:a7:1d:c7:cc>
static uint8_t mac_addr_4[] = {0xcc, 0xdb, 0xa7, 0x1d, 0xc7, 0xcc};

// Device 5 <c8:f0:9e:7b:10:8c>
static uint8_t mac_addr_5[] = {0x94, 0xb5, 0x55, 0xf9, 0xf2, 0xf0};

typedef enum device_title {
   MASTER, // lidr
   SLAVE   // nasledovnik
} device_title_t;

typedef enum device_status {
   ACTIVE,  // device is active in DS
   INACTIVE // device was active in DS
} device_status_t;

typedef enum espnow_event_id {
   ESPNOW_SEND_CB,
   ESPNOW_RECV_CB,
} espnow_event_id_t;

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

typedef enum message_type {
   RTT_CAL_MASTER, // time used to calculate the RTT (master -> slave)
   RTT_CAL_SLAVE,  // time used to calculate the RTT (slave -> master)
   RTT,            // calculated value of RTT
   TIME,           // time to synchronize the time
   HELLO_DS,
   NEIGHBOURS
} message_type_t;

typedef struct message {
   message_type_t type;
   uint64_t content;
   uint8_t payload[0];
} __attribute__((packed)) message_data_t;

typedef struct espnow_send_param {
   message_type_t type;
   uint64_t content;
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
} node_info_t;

typedef struct print_data {
   uint32_t rtt;
   int32_t deviation;
   uint64_t time;
} print_data_t;

typedef struct neighbour {
   esp_now_peer_info_t peer_info;
   device_title_t title;
   device_status_t status;
} neighbour_t;

#endif // RAFTING_BUTTON_H