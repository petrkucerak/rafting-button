#ifndef __USBC_H
#define __USBC_H

#include <Arduino.h>
#include <USB.h>

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
static USBCDC USBSerial;
#endif

typedef struct {
   /**
    * @brief This function defines USB callback
    *
    * @param arg
    * @param event_base
    * @param event_id
    * @param event_data
    */
   void (*usb_event_callback)(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data);
} usbc_s;

/**
 * @brief This structure contains usb callback.
 *
 */
extern usbc_s usbc_dev;

#endif // __USBC_H