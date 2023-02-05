#include "usb_names.h"

#define MIDI_NAME   {'D','M','X','2','M','I','D','I'}
#define MIDI_NAME_LEN  8

#define MANUFACTURER_NAME  {'N','o','t','e','s',' ','a','n','d',' ','V','o','l','t','s'}
#define MANUFACTURER_NAME_LEN 15

struct usb_string_descriptor_struct usb_string_product_name = {
  2 + MIDI_NAME_LEN * 2,
  3,
  MIDI_NAME
};

struct usb_string_descriptor_struct usb_string_manufacturer_name = {
  2 + MANUFACTURER_NAME_LEN * 2,
  3,
  MANUFACTURER_NAME
};
