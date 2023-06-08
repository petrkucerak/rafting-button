/**
 * @file rafting-button.h
 * @author Petr Kucera (kucerp28@fel.cvut.cz)
 * @brief The main file defines the program structure. The parent function is
 * app_main. Detail description is in the :
 * @version 1.0
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RAFTING_BUTTON_H
#define RAFTING_BUTTON_H

#define BALANCER_SIZE 100
#define NEIGHBOURS_COUNT 9
#define EVENT_HISTORY 50

#define PRIORITY_RTT_START 2
#define PRIORITY_TIME_START 2
#define PRIORITY_HANDLER 4
#define PRIORITY_HANDLE_DS_EVENT 2
#define PRIORITY_REQUEST_TASK 2
#define PRIORITY_HANDLE_ISR_EVENT 3

#define ESPNOW_QUEUE_SIZE 10
#define PRINT_QUEUE_SIZE 4
#define LOG_QUEUE_SIZE 10 // min, device count
#define ISR_QUEUE_SIZE 1

#define ESPNOW_MAXDELAY 10
#define CLEANING_DELAY 1
#define DS_MAXDELAY 100
#define STACK_SIZE 4096

#define DEVIATION_LIMIT 200
#define DEVIATION_MAX_CONSTANT 25

#define CONFIG_ESPNOW_SEND_LEN 250

#define COUNT_ERROR_MESSAGE_TO_INACTIVE 5

#define VOTE_TIMEOUT 1000000
#define MASTER_TIMEOUT 2000000

#include <espnow.h>
#include <inttypes.h>

typedef enum device_title {
   SLAVE,     // nasledovnik
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

typedef enum ds_task {
   SEND,
   SAVE,
} ds_task_t;

typedef enum ds_event {
   PUSH,
   RESET,
   EMPTY,
} ds_event_t;

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
   ds_event_t event_type;
   uint8_t event_mac_addr[ESP_NOW_ETH_ALEN];
   ds_task_t event_task;
   neighbour_t neighbour[NEIGHBOURS_COUNT];
   uint8_t payload[0];
} __attribute__((packed)) message_data_t;

typedef struct espnow_send_param {
   message_type_t type;
   uint64_t content;
   uint32_t epoch_id;
   neighbour_t neighbour[NEIGHBOURS_COUNT];
   ds_event_t event_type;
   uint8_t event_mac_addr[ESP_NOW_ETH_ALEN];
   ds_task_t event_task;
   int data_len;
   uint8_t *buf;
   uint8_t dest_mac[ESP_NOW_ETH_ALEN];
} espnow_send_param_t;

typedef struct log_event {
   uint64_t timestamp;
   ds_event_t type;
   uint8_t mac_addr[ESP_NOW_ETH_ALEN];
   ds_task_t task;
} log_event_t;

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
   log_event_t events[EVENT_HISTORY];
} node_info_t;

#endif // RAFTING_BUTTON_H