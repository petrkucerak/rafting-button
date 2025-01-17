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
/// @brief The count of possible used devices is 20 (esp-now limitation).
#define NEIGHBORS_MAX_COUNT 20
/// @brief The count of possible send by one message is 10
#define NEIGHBORS_MAX_MESSAGE_COUNT 10
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
 * @brief Enumeration representing the ESP-NOW event ID.
 */
typedef enum espnow_event_id {
   /// @brief Event type for ESP-NOW send callback.
   ESPNOW_SEND_CB,
   /// @brief Event type for ESP-NOW receive callback.
   ESPNOW_RECV_CB,
} espnow_event_id_t;

/**
 * @brief Enumeration representing the DS task.
 */
typedef enum ds_task {
   /// @brief Instruction to send log to all nodes.
   SEND,
   /// @brief Only save log to log memory.
   SAVE,
} ds_task_t;

/**
 * @brief Enumeration representing the DS event type.
 */
typedef enum ds_event {
   /// @brief DS event type: short push.
   PUSH,
   /// @brief DS event type: long push, means reset.
   RESET,
   /// @brief Empty DS event.
   EMPTY,
} ds_event_t;

/**
 * @brief Enumeration representing the types of ESP-NOW event messages.
 *
 * @note Comments type of messages is only translated from the bachelor's
 * thesis.
 *
 */
typedef enum message_type {
   /// @brief HELLO_DS: The device verifies if the sender is an unknown device.
   /// If it is, the device adds it to its list. If the device is already known,
   /// it only sets its status as active. As a response, it sends a list of all
   /// devices in the network.
   HELLO_DS,
   /// @brief neighborS: The device adds all unknown neighbors to its list and
   /// updates the status and title of each device.
   NEIGHBORS,
   /// @brief RTT_CAL_MASTER: The device sends back messages to the sender with
   /// the same content. This message is used for calculating the round trip
   /// time.
   RTT_CAL_MASTER,
   /// @brief RTT_CAL_SLAVE: The device calculates the round trip time and sends
   /// it to the sender.
   RTT_CAL_SLAVE,
   /// @brief RTT: The device stores the transmission time value in an array to
   /// calculate the average round trip time. If it is the first such
   /// information, the array is filled with this value.
   RTT,
   /// @brief TIME: The device calculates the time synchronization error 𝑂 and
   /// sets the constant 𝑐 for calculating the 𝑇𝐷𝑆 time according to the
   /// algorithm described in Chapter 5.4.5 bachelor's thesis. This message is
   /// also used to
   /// maintain the leader's authority. If a device in candidate state receives
   /// this type of message, it ends its candidacy and switches to follower
   /// state. A timestamp is stored with each processing of this message, which
   /// is subsequently used to calculate the timeout 𝑡𝑠𝑦𝑛𝑐. If it exceeds a
   /// specified constant, the device transitions to the candidate state,
   /// increases the epoch ID, and initializes the elections.
   TIME,
   /// @brief REQUEST_VOTE: The device sends a message to the sender with a
   /// vote.
   REQUEST_VOTE,
   /// @brief GIVE_VOTE: The device stores the message. If it receives more
   /// responses than half of the active devices in the network, it declares
   /// itself as the leader and the other devices as followers.
   GIVE_VOTE,
   /// @brief LOG: The log is saved and distributed to all devices in the DS.
   LOG,
} message_type_t;

/**
 * @brief Structure representing the ESP-NOW event callback for send operations.
 *
 */
typedef struct espnow_event_send_cb {
   /// @brief Target MAC address
   uint8_t mac_addr[ESP_NOW_ETH_ALEN];
   /// @brief Send status of the ESP-NOW operation
   esp_now_send_status_t status;
} espnow_event_send_cb_t;

/**
 * @brief Structure representing the ESP-NOW event callback for receive
 * operations.
 *
 */
typedef struct espnow_event_recv_cb {
   /// @brief Source MAC address
   uint8_t mac_addr[ESP_NOW_ETH_ALEN];
   /// @brief Pointer to the received data
   uint8_t *data;
   /// @brief Length of the received data
   /// @note In this project, the length of data for all ESP-NOW messages is
   /// CONFIG_ESPNOW_SEND_LEN (250 bytes).
   int data_len;
} espnow_event_recv_cb_t;

/**
 * @brief Structure defining the ESP-NOW event information.
 *
 */
typedef union espnow_event_info {
   /// @brief ESP-NOW event callback structure for send operations
   espnow_event_send_cb_t send_cb;
   /// @brief ESP-NOW event callback structure for receive operations
   espnow_event_recv_cb_t recv_cb;
} espnow_event_info_t;

/**
 * @brief Structure defining an ESP-NOW event.
 *
 */
typedef struct espnow_event {
   /// @brief ESP-NOW event ID
   espnow_event_id_t id;
   /// @brief ESP-NOW event information
   espnow_event_info_t info;
   /// @brief Timestamp of the ESP-NOW event, created upon receiving in the
   /// callback
   uint64_t timestamp;
} espnow_event_t;

/**
 * @brief Structure defining a neighbor device.
 *
 */
typedef struct neighbor {
   /// @brief Device title in the distributed system (DS)
   device_title_t title;
   /// @brief Device hardware status
   device_status_t status;
   /// @brief Device MAC address
   uint8_t mac_addr[ESP_NOW_ETH_ALEN];
} neighbor_t;

/**
 * @brief Structure defining a message.
 *
 */
typedef struct message_data {
   /// @brief ESP-NOW message type
   message_type_t type;
   /// @brief Epoch ID
   uint32_t epoch_id;
   /// @brief Content variable, used for distributing uint64_t number values
   /// like time, round trip time, etc.
   uint64_t content;
   /// @brief Array of device neighbors
   ds_event_t event_type;
   /// @brief DS event MAC address
   uint8_t event_mac_addr[ESP_NOW_ETH_ALEN];
   /// @brief DS event task
   ds_task_t event_task;
   /// @brief Neighbor check 1 - NOT_INITIALIZED, 100 - INACTIVE, 10000 - ACTIVE
   uint32_t neighbor_check;
   /// @brief Message payload
   /// @note The message payload fills all remaining bytes after the data with
   /// random values.
   uint8_t payload[0];
} __attribute__((packed)) message_data_t;

/**
 * @brief Structure defining a specific neighbor message.
 *
 */
typedef struct message_neighbor_data {
   /// @brief ESP-NOW message type
   message_type_t type;
   /// @brief Epoch ID
   uint32_t epoch_id;
   /// @brief Array of device neighbors
   neighbor_t neighbor[NEIGHBORS_MAX_MESSAGE_COUNT];
   /// @brief Message payload
   /// @note The message payload fills all remaining bytes after the data with
   /// random values.
   uint8_t payload[0];
} __attribute__((packed)) message_neighbor_data_t;

/**
 * @brief Structure defining parameters for ESP-NOW message transmission.
 *
 */
typedef struct espnow_send_param {
   /// @brief ESP-NOW message type
   message_type_t type;
   /// @brief Content variable, used for distributing uint64_t number values
   /// like time, round trip time, etc.
   uint64_t content;
   /// @brief Epoch ID
   uint32_t epoch_id;
   /// @brief Array of device neighbors
   uint32_t neighbor_check;
   /// @brief DS event type
   ds_event_t event_type;
   /// @brief DS event MAC address
   uint8_t event_mac_addr[ESP_NOW_ETH_ALEN];
   /// @brief DS event task
   ds_task_t event_task;
   /// @brief Data length
   int data_len;
   /// @brief Data buffer
   uint8_t *buf;
   /// @brief Target (destination) MAC address
   uint8_t dest_mac[ESP_NOW_ETH_ALEN];
} espnow_send_param_t;

/**
 * @brief Structure defining parameters for ESP-NOW message neighbor type
 * transmission.
 *
 */
typedef struct espnow_send_neighbor_param {
   /// @brief ESP-NOW message type
   message_type_t type;
   /// @brief Epoch ID
   uint32_t epoch_id;
   /// @brief Array of device neighbors
   neighbor_t neighbor[NEIGHBORS_MAX_MESSAGE_COUNT];
   /// @brief Data length
   int data_len;
   /// @brief Data buffer
   uint8_t *buf;
   /// @brief Target (destination) MAC address
   uint8_t dest_mac[ESP_NOW_ETH_ALEN];
} espnow_send_neighbor_param_t;

/**
 * @brief Structure representing a distributed system (DS) event.
 *
 */
typedef struct log_event {
   /// @brief Timestamp of the DS event
   uint64_t timestamp;
   /// @brief Type of the DS event
   ds_event_t type;
   /// @brief MAC source address of the DS event
   uint8_t mac_addr[ESP_NOW_ETH_ALEN];
   /// @brief Task associated with the DS event
   ds_task_t task;
} log_event_t;

/**
 * @brief Metadata information about a device in the Distributed System (DS).
 * @note In this project, the terms "node" and "device" are used
 * interchangeably.
 *
 */
typedef struct node_info {
   /// @brief Time correction value
   /// @note Time correction is used for calculating DS time (𝑇𝐷𝑆 = 𝑡𝑙𝑜𝑐𝑎𝑙 − 𝑐)
   uint64_t time_correction;
   /// @brief Array used for calculating round trip time
   uint32_t rtt_balancer[BALANCER_SIZE];
   /// @brief Index of the last inserted value into the round trip time array
   uint16_t rtt_balancer_index;
   /// @brief Boolean value indicating if round trip time is set up for the
   /// first time
   bool is_first_setup_rtt;
   /// @brief Boolean value indicating if time is synced to the limit
   bool is_time_synced;
   /// @brief Average deviation value
   int32_t deviation_avg;
   /// @brief List of device neighbors
   neighbor_t neighbor[NEIGHBORS_MAX_COUNT];
   /// @brief Counts of unsuccessful message sends to neighbor devices
   uint8_t neighbor_error_count[NEIGHBORS_MAX_COUNT];
   /// @brief Epoch ID of the device
   uint32_t epoch_id;
   /// @brief Title of the device
   device_title_t title;
   /// @brief Synchronization time timeout
   /// @note The value is represented by a timestamp
   uint64_t timeout_sync;
   /// @brief Voting timeout
   /// @note The value is represented by a timestamp
   uint64_t timeout_vote;
   /// @brief Count of votes
   uint8_t count_of_vote;
   /// @brief List of DS events
   log_event_t events[EVENT_HISTORY];
} node_info_t;

/**
 * @brief Calculate the average value of the RTT (Round-Trip Time) balancer
 * array.
 *
 * This function calculates the average value of the RTT balancer array by
 * summing up all the elements and dividing the sum by the size of the array.
 *
 * @return The average value of the RTT balancer array.
 */
uint32_t get_rtt_avg(void);

/**
 * @brief Get the current time.
 *
 * This function retrieves the current time by calling `esp_timer_get_time()`
 * and subtracting the time correction value from it.
 *
 * @return The current time.
 */
uint64_t get_time(void);

/**
 * @brief Get the time with a provided ESP timer value.
 *
 * This function calculates the time with the given ESP timer value by
 * subtracting the time correction value from it.
 *
 * @param esp_time The ESP timer value.
 * @return The calculated time.
 */
uint64_t get_time_with_timer(uint64_t esp_time);

/**
 * @brief Print the list of neighbors.
 *
 * This function prints the list of neighbors along with their statuses and
 * titles. It retrieves the device MAC address and uses it to display the
 * current device's title. The neighbors' statuses and titles are logged using
 * ESP_LOGI.
 */
void print_neighbors(void);

/**
 * @brief Print a log event.
 *
 * This function prints a log event by logging its type, source MAC address, and
 * timestamp using ESP_LOGI.
 *
 * @param data The log event data to be printed.
 */
void print_log_event(log_event_t data);

/**
 * @brief Print the log.
 *
 * This function prints the log by iterating over the events array and logging
 * non-empty events along with their indices, MAC addresses, types, and
 * timestamps using ESP_LOGI.
 */
void print_log(void);

/**
 * @brief Get the count of active devices in the neighbors array.
 *
 * This function counts the number of active devices in the neighbors array by
 * iterating over it and incrementing the count for each device with the ACTIVE
 * status.
 *
 * @return The count of active devices.
 */
uint8_t get_cout_active_devices(void);

/**
 * @brief Task to handle ISR events.
 *
 * This task waits for log events in the `isr_event` queue. It receives events,
 * determines the type of event based on the button release time, sets the event
 * type accordingly, and sends the event to the `log_event` queue. Bounced
 * interrupts are removed from the `isr_event` queue.
 */
void handle_isr_event_task(void);

/**
 * @brief Parse ESP-NOW data and extract information.
 *
 * This function parses the received ESP-NOW data and extracts information such
 * as message type, content, epoch ID, neighbor list, and log event. It
 * populates the provided variables with the extracted data.
 *
 * @param data The received ESP-NOW data.
 * @param data_len The length of the received data.
 * @param type Pointer to a variable to store the message type.
 * @param content Pointer to a variable to store the content.
 * @param epoch_id Pointer to a variable to store the epoch ID.
 * @param neighbor Pointer to an array to store the neighbor list.
 * @param event Pointer to a log_event_t variable to store the log event.
 * @return The message type extracted from the data, or -1 if the data length is
 * invalid.
 *
 * @note The function checks if the length of the received data is sufficient to
 * contain the expected structure. If the data is too short, an error is logged
 * and the function returns -1.
 */
int espnow_data_parse(uint8_t *data, int data_len, message_type_t *type,
                      uint64_t *content, uint32_t *epoch_id,
                      neighbor_t *neighbor, log_event_t *event);

/**
 * @brief Parse ESP-NOW data and extract neighbor information.
 *
 * This function parses the received ESP-NOW data specific to neighbor messages.
 * It extracts the message type, epoch ID, and neighbor list, and populates
 * the provided variables with the extracted data.
 *
 * @param data Pointer to the received ESP-NOW data.
 * @param data_len The length of the received data.
 * @param type Pointer to a variable to store the message type.
 * @param epoch_id Pointer to a variable to store the epoch ID.
 * @param neighbor Pointer to an array to store the neighbor list. The array
 * should have space for at least `NEIGHBORS_MAX_MESSAGE_COUNT` elements.
 * @return The message type extracted from the data, or -1 if the data length is
 * invalid.
 *
 * @note The function checks if the length of the received data is sufficient to
 * contain the expected structure. If the data is too short, an error is logged
 * and the function returns -1.
 */
int espnow_data_neighbor_parse(uint8_t *data, int data_len,
                               message_type_t *type, uint32_t *epoch_id,
                               neighbor_t *neighbor);

/**
 * @brief Get the message type from incoming message and return it as a enum
 * type message_type_t.
 *
 * @param data The received ESP-NOW data.
 * @param data_len The length of the received data.
 * @return message_type_t The type of received message.
 */
message_type_t get_message_type(uint8_t *data, int data_len);

/**
 * @brief Prepare ESP-NOW data for sending.
 *
 * This function prepares the ESP-NOW data to be sent by populating the
 * send_param structure. It sets the message type, epoch ID, content, event
 * task, event type, event MAC address, and neighbor list in the data buffer.
 *
 * @param send_param Pointer to the espnow_send_param_t structure.
 */
void espnow_data_prepare(espnow_send_param_t *send_param);

/**
 * @brief Prepare ESP-NOW data for sending special NEIGHBOR function.
 *
 * This function prepares the ESP-NOW data to be sent by populating the
 * send_param structure. It sets the message type, epoch ID and neighbor list in
 * the data buffer.
 *
 * @param send_param Pointer to the espnow_send_neighbor_param_t structure.
 */
void espnow_data_neighbor_prepare(espnow_send_neighbor_param_t *send_param);

/**
 * @brief Handle ESP-NOW send error.
 *
 * This function handles the ESP-NOW send error by logging an appropriate error
 * message based on the error code.
 *
 * @param code The ESP-NOW error code.
 */
void handle_espnow_send_error(esp_err_t code);

/**
 * @brief Compute a simple checksum to represent the state of neighbors.
 *
 * This function calculates a checksum of type `uint32_t` based on the state
 * of each node in the given neighbor structure. The checksum is computed
 * as follows:
 * - For each node in the `NOT_INITIALIZED` state, a value of 1 is added.
 * - For each node in the `INACTIVE` state, a value of 100 is added.
 * - For each node in the `ACTIVE` state, a value of 10,000 is added.
 *
 * @param neighbor A pointer to the neighbor structure containing the nodes.
 * @return uint32_t The calculated checksum representing the states of the
 * neighbors.
 */
uint32_t get_neighbor_check(neighbor_t *neighbor);

/**
 * @brief Handle ESP-NOW add peer error.
 *
 * This function handles the ESP-NOW add peer error by logging an appropriate
 * error message based on the error code.
 *
 * @param code The ESP-NOW error code.
 */
void handle_espnow_add_peer_error(esp_err_t code);

/**
 * @brief Function for handling ESP-NOW events.
 *
 * This function is a task that receives events from the ESP-NOW queue and
 * handles them accordingly. It performs various operations based on the type of
 * event received, such as sending and receiving data, managing neighbors,
 * calculating RTT, synchronizing time, and processing log events.
 */
void espnow_handler_task(void);

/**
 * @brief Sends a HELLO_DS message.
 */
void send_hello_ds_message(void);

/**
 * @brief Sends neighbor information using ESP-NOW protocol in two batches.
 *
 * This function prepares and sends the information about neighbors
 * over the ESP-NOW protocol. The neighbor data is divided into two
 * batches of `NEIGHBORS_MAX_MESSAGE_COUNT` neighbors each. The function
 * allocates the required memory, prepares the data, and sends it to
 * all active neighbors.
 *
 * Steps performed by the function:
 * 1. Allocates memory for the `espnow_send_neighbor_param_t` structure.
 * 2. Allocates a buffer for the data to be sent.
 * 3. Prepares and sends the first batch of neighbor data (first 10 neighbors).
 * 4. Prepares and sends the second batch of neighbor data (next 10 neighbors).
 *
 * @note The function handles memory allocation errors and ensures cleanup
 *       if any allocation fails.
 */
void send_neighbor_message_to_all(void);

/**
 * @brief Sends an RTT_CAL_MASTER message.
 * This function runs as a task and continuously sends the message to active
 * neighbors.
 */
void send_rtt_cal_master_task(void);

/**
 * @brief Sends a REQUEST_VOTE message.
 * This function runs as a task and continuously sends the message to active
 * neighbors. It also handles the voting process and updates the node's state
 * accordingly.
 */
void send_request_vote_task(void);

/**
 * @brief Sends a TIME message.
 * This function runs as a task and continuously sends the message to active
 * neighbors.
 */
void send_time_task(void);

/**
 * @brief Handles DS events.
 * This function runs as a task and processes incoming log events.
 * It sorts the events based on timestamp and distributes the relevant
 * information to active neighbors.
 */
void handle_ds_event_task(void);

/**
 * @brief Main function of the application.
 *
 * This function is the entry point of the application and serves as the main
 * execution loop. It initializes various components, sets up interrupts,
 * configures GPIO pins, initializes Wi-Fi and ESP-NOW, and creates multiple
 * tasks for handling different functionalities.
 */
void app_main(void);

#endif // RAFTING_BUTTON_H