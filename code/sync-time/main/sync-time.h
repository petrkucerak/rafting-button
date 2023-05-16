#ifndef SYNC_TIME_H
#define SYNC_TIME_H

#include <espnow.h>
#include <inttypes.h>

// Device 1 <70:b8:f6:5b:d3:24>
static uint8_t mac_addr_1[] = {0x70, 0xb8, 0xf6, 0x5b, 0xd3, 0x24};

// Device 1 <cc:db:a7:1d:c4:08>
static uint8_t mac_addr_2[] = {0xcc, 0xdb, 0xa7, 0x1d, 0xc4, 0x08};

// Device 1 <c8:f0:9e:7b:10:8c>
static uint8_t mac_addr_3[] = {0xc8, 0xf0, 0x9e, 0x7b, 0x10, 0x8c};

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
} espnow_event_t;

typedef enum message_type {
   RTT_CAL_MASTER, // time used to calculate the RTT (master -> slave)
   RTT_CAL_SLAVE,  // time used to calculate the RTT (slave -> master)
   RTT_VAL,        // calculated value of RTT
   TIME            // time to synchronize the time
} message_type_t;

typedef struct message {
   message_type_t type;
   uint64_t content;
} __attribute__((packed)) message_data_t;

#endif // SYNC_TIME_H