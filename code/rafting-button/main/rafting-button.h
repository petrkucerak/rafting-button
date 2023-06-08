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

/// @brief Size of the array used for calculation round trip time.
#define BALANCER_SIZE 100
/// @brief The count of possible used devices is 10. Therefore the count of
/// neighbors is 9 devices.
#define NEIGHBOURS_COUNT 9
/// @brief Size of the array used for storing logs with DS events.
#define EVENT_HISTORY 50

/// @brief Priority for the task that starts synchronization round trip time.
#define PRIORITY_RTT_START 2
/// @brief Priority for the task that starts synchronization DS time.
#define PRIORITY_TIME_START 2
/// @brief Priority for the task that handles ESP-NOW events.
#define PRIORITY_HANDLER 4
/// @brief Priority for the task that handles DS events, specifically logs.
#define PRIORITY_HANDLE_DS_EVENT 2
/// @brief Priority for the task that sends a message with a vote request.
#define PRIORITY_REQUEST_TASK 2
/// @brief Priority for the task that handles ISR events.
#define PRIORITY_HANDLE_ISR_EVENT 3

/// @brief Define the size of the queue used to handle ESP-NOW events.
#define ESPNOW_QUEUE_SIZE 10
/// @brief Define the queue size used to handle DS events, specifically logs.
/// Max size should be 10. DS is limited to 10 devices.
#define LOG_QUEUE_SIZE 10
/// @brief Define the queue size used to send timestamps from ISR.
#define ISR_QUEUE_SIZE 1

/// @brief Maximum processor tick count deadline for ESP-NOW events.
#define ESPNOW_MAXDELAY 10
/// @brief Maximum processor tick count deadline for reparetly sending timestamp
/// from ISR.
#define CLEANING_DELAY 1
/// @brief Maximum processor tick count deadline for DS events, specifically log
/// distribution.
#define DS_MAXDELAY 100
/// @brief Define stack size for tasks.
#define STACK_SIZE 4096

/// @brief Deviation limit used in time synchronization.
#define DEVIATION_LIMIT 200
/// @brief Deviation maximum constant used in time synchronization.
#define DEVIATION_MAX_CONSTANT 25
/// @brief Config universal ESP-NOW message length.
#define CONFIG_ESPNOW_SEND_LEN 250
/// @brief Define the limit of undelivered messages that indicates inactive
/// devices.
#define COUNT_ERROR_MESSAGE_TO_INACTIVE 5

/// @brief Defines the vote timeout. The current value is 1s.
#define VOTE_TIMEOUT 1000000
/// @brief Defines the inactive master timeout. The current value is 2s.
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