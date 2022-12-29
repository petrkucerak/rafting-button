#include "esp-now.h"

#include "callback.h"

extern USBCDC USBSerial;

void format_mac_address(const uint8_t *mac_address, char *buffer, int max_size)
{
   snprintf(buffer, max_size, "%02X:%02X:%02X:%02X:%02X:%02X", mac_address[0],
            mac_address[1], mac_address[2], mac_address[3], mac_address[4],
            mac_address[5]);
}

void test_esp_now(uint8_t count_of_neighbours)
{
   // allocate testing strucutre
   Testing_message *etap = NULL;
   etap =
       (Testing_message *)malloc(sizeof(Testing_message) * count_of_neighbours);
   if (etap == NULL) {
      USBSerial.printf("ERROR: can't allocate memory! [esp-now.cpp]\n");
      ESP.restart();
   }
}