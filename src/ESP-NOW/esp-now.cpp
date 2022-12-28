#include "esp-now.h"

void format_mac_address(const uint8_t *mac_address, char *buffer,
                        int max_size) {
  snprintf(buffer, max_size, "%02x:%02x:%02x:%02x:%02x:%02x", mac_address[0],
           mac_address[1], mac_address[2], mac_address[3], mac_address[4],
           mac_address[5]);
}