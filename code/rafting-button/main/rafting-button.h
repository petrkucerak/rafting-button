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

/// @brief Size of the array used for calculating round trip time.
#define BALANCER_SIZE 100
/// @brief The count of possible used devices is 10, therefore the count of
/// neighbors is 9 devices.
#define NEIGHBORS_COUNT 9
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

/// @brief Defines the size of the queue used to handle ESP-NOW events.
#define ESPNOW_QUEUE_SIZE 10
/// @brief Defines the queue size used to handle DS events, specifically logs.
/// The max size should be 10. DS is limited to 10 devices.
#define LOG_QUEUE_SIZE 10
/// @brief Defines the queue size used to send timestamps from the ISR.
#define ISR_QUEUE_SIZE 1

/// @brief Maximum processor tick count deadline for ESP-NOW events.
#define ESPNOW_MAX_DELAY 10
/// @brief Maximum processor tick count deadline for repeatedly sending
/// timestamps from the ISR.
#define CLEANING_DELAY 1
/// @brief Maximum processor tick count deadline for DS events, specifically log
/// distribution.
#define DS_MAX_DELAY 100
/// @brief Defines the stack size for tasks.
#define STACK_SIZE 4096

/// @brief Deviation limit used in time synchronization.
#define DEVIATION_LIMIT 200
/// @brief Maximum deviation constant used in time synchronization.
#define DEVIATION_MAX_CONSTANT 25
/// @brief Configures the universal ESP-NOW message length.
#define CONFIG_ESPNOW_SEND_LEN 250
/// @brief Defines the limit of undelivered messages that indicates inactive
/// devices.
#define COUNT_ERROR_MESSAGE_TO_INACTIVE 5

/// @brief Defines the vote timeout in microseconds. The current value is 1
/// second.
#define VOTE_TIMEOUT 1000000
/// @brief Defines the inactive master timeout in microseconds. The current
/// value is 2 seconds.
#define MASTER_TIMEOUT 2000000

#include <espnow.h>
#include <inttypes.h>

/**
 * @brief Enumeration representing the title of a device.
 */
typedef enum device_title {
   /// @brief Represents a slave device (následovník).
   SLAVE,
   /// @brief Represents a master candidate device (kandidát na lídra).
   CANDIDATE,
   /// @brief Represents a master device (lídr).
   MASTER,
} device_title_t;

/**
 * @brief Enumeration representing the status of a device.
 */
typedef enum device_status {
   /// @brief Device is not initialized.
   NOT_INITIALIZED,
   /// @brief Device is inactive in the distributed system (DS).
   INACTIVE,
   /// @brief Device is active in the distributed system (DS).
   ACTIVE,
} device_status_t;

/**
 * @brief Enumeration of representing the ESP-NOW event ID.
 *
 */
typedef enum espnow_event_id {
   /// @brief ESP-NOW event type is send
   ESPNOW_SEND_CB,
   /// @brief ESP-NOW event type is receive
   ESPNOW_RECV_CB,
} espnow_event_id_t;

/**
 * @brief Enumeration of representing the DS task
 *
 */
typedef enum ds_task {
   /// @brief Instruction to send log to all nodes
   SEND,
   /// @brief Only save log to log memory
   SAVE,
} ds_task_t;

/**
 * @brief Enumeration of representing the DS event type
 *
 */
typedef enum ds_event {
   /// @brief DS event type short push
   PUSH,
   /// @brief DS event type long push, means reset
   RESET,
   /// @brief DS event is an empyt
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