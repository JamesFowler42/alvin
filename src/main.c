/* 
 * Alvin IFTTT Control Application
 *
 * Copyright (c) 2015 James Fowler
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "pebble.h"
#include "alvin.h"

static ConfigData config_data;
static bool save_config_requested = false;
static bool version_sent = false;
static AppTimer *redraw_timer = NULL;

/*
 * Send a message to javascript
 */
static void send_to_phone(const uint32_t key, int32_t tophone) {

  Tuplet tuplet = TupletInteger(key, tophone);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    LOG_WARN("no outbox");
    return;
  }

  dict_write_tuplet(iter, &tuplet);
  dict_write_end(iter);

  app_message_outbox_send();

}

/*
 * Update the menu, save the config etc.
 */
static void update_redraw(void *data) {
  redraw_timer = NULL;
  trigger_config_save();
  reload_menu();
}

/*
 * Queue a menu re-draw after all the incoming messages reconfiguring menu have been processed
 */
static void request_update_redraw() {
  if (redraw_timer == NULL) {
    redraw_timer = app_timer_register(MENU_REDRAW_TIME_MS, update_redraw, NULL);
  } else {
    app_timer_reschedule(redraw_timer, MENU_REDRAW_TIME_MS);
  }
}

/*
 * Menu item fired
 */
EXTFN void menu_item_fired(int32_t menu_item) {

  int32_t turnon = config_data.entry[menu_item].on ? 0 : 1; // Menu item not set yet, so previous value used

  Tuplet tuplet1 = TupletInteger(KEY_FIRE, menu_item);
  Tuplet tuplet2 = TupletInteger(KEY_TURNON, turnon);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    LOG_WARN("no outbox");
    return;
  }

  dict_write_tuplet(iter, &tuplet1);
  dict_write_tuplet(iter, &tuplet2);
  dict_write_end(iter);

  app_message_outbox_send();

}

/*
 * Incoming message handler
 */
static void in_received_handler(DictionaryIterator *iter, void *context) {

  // Got a ctrl message
  Tuple *ctrl_tuple = dict_find(iter, KEY_CTRL);
  if (ctrl_tuple) {

    int32_t ctrl_value = ctrl_tuple->value->int32;

    // If version is done then mark it
    if (ctrl_value & CTRL_VERSION_DONE) {
      LOG_DEBUG("CTRL_VERSION_DONE");
      version_sent = true;
    }

    if (ctrl_value & CTRL_GOT_REQUEST) {
      LOG_DEBUG("CTRL_GOT_REQUEST");
      set_sent();
    }

    if (ctrl_value & CTRL_REQUEST_OK) {
      LOG_DEBUG("CTRL_REQUEST_OK");
      set_success();
    }

    if (ctrl_value & CTRL_REQUEST_FAIL) {
      LOG_DEBUG("CTRL_REQUEST_FAIL");
      set_failed();
    }
  }

  // Got a menu name
  Tuple *mn = dict_find(iter, KEY_MENU_NAME_1);
  if (mn) {
    strncpy(config_data.entry[0].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }
  mn = dict_find(iter, KEY_MENU_NAME_2);
  if (mn) {
    strncpy(config_data.entry[1].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }
  mn = dict_find(iter, KEY_MENU_NAME_3);
  if (mn) {
    strncpy(config_data.entry[2].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }
  mn = dict_find(iter, KEY_MENU_NAME_4);
  if (mn) {
    strncpy(config_data.entry[3].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }
  mn = dict_find(iter, KEY_MENU_NAME_5);
  if (mn) {
    strncpy(config_data.entry[4].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }
  mn = dict_find(iter, KEY_MENU_NAME_6);
  if (mn) {
    strncpy(config_data.entry[5].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }
  mn = dict_find(iter, KEY_MENU_NAME_7);
  if (mn) {
    strncpy(config_data.entry[6].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }
  mn = dict_find(iter, KEY_MENU_NAME_8);
  if (mn) {
    strncpy(config_data.entry[7].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }
  mn = dict_find(iter, KEY_MENU_NAME_9);
  if (mn) {
    strncpy(config_data.entry[8].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }
  mn = dict_find(iter, KEY_MENU_NAME_10);
  if (mn) {
    strncpy(config_data.entry[9].menu_text, mn->value->cstring, MENU_TEXT_LEN);
    request_update_redraw();
  }

  // Which menu entries are toggles
  Tuple *tog_tuple = dict_find(iter, KEY_MENU_TOGGLE);
  if (tog_tuple) {

    int32_t tog_value = tog_tuple->value->int32;
    config_data.entry[0].toggle = (tog_value & 1) == 1;
    config_data.entry[1].toggle = (tog_value & 2) == 2;
    config_data.entry[2].toggle = (tog_value & 4) == 4;
    config_data.entry[3].toggle = (tog_value & 8) == 8;
    config_data.entry[4].toggle = (tog_value & 16) == 16;
    config_data.entry[5].toggle = (tog_value & 32) == 32;
    config_data.entry[6].toggle = (tog_value & 64) == 64;
    config_data.entry[7].toggle = (tog_value & 128) == 128;
    config_data.entry[8].toggle = (tog_value & 256) == 256;
    config_data.entry[9].toggle = (tog_value & 512) == 512;
    for (int8_t i = 0; i < MAX_MENU_ENTRY; i++) {
      config_data.entry[i].on = false;
    }

    config_data.active = config_data.entry[0].menu_text[0] != '\0';
    request_update_redraw();

  }

}

/*
 * Send the version on initialisation
 */
static void send_version(void *data) {
  if (!version_sent) {
    if (bluetooth_connection_service_peek()) {
      app_timer_register(VERSION_SEND_INTERVAL_MS, send_version, NULL);
      send_to_phone(KEY_VERSION, VERSION);
    } else {
      app_timer_register(VERSION_SEND_SLOW_INTERVAL_MS, send_version, NULL);
    }
  }

}

/*
 * Calcuate buffer sizes and open comms
 */
EXTFN void open_comms() {

  // Register message handlers
  app_message_register_inbox_received(in_received_handler);

  // Incoming size
  Tuplet out_values[] = { TupletInteger(KEY_FIRE, 0), TupletInteger(KEY_TURNON, 0) };
  Tuplet in_values[] = { TupletCString(KEY_MENU_NAME_1, FULL_STRING), TupletCString(KEY_MENU_NAME_2, FULL_STRING), TupletCString(KEY_MENU_NAME_3, FULL_STRING), TupletCString(KEY_MENU_NAME_4, FULL_STRING), TupletCString(KEY_MENU_NAME_5, FULL_STRING), TupletCString(KEY_MENU_NAME_6, FULL_STRING), TupletCString(KEY_MENU_NAME_7, FULL_STRING), TupletCString(KEY_MENU_NAME_8, FULL_STRING), TupletCString(KEY_MENU_NAME_9, FULL_STRING), TupletCString(KEY_MENU_NAME_10, FULL_STRING), TupletInteger(KEY_MENU_TOGGLE, 0) };

  uint32_t outbound_size = dict_calc_buffer_size_from_tuplets(out_values, ARRAY_LENGTH(out_values)) + FUDGE;
  uint32_t inbound_size = dict_calc_buffer_size_from_tuplets(in_values, ARRAY_LENGTH(in_values)) + FUDGE;

  LOG_DEBUG("I(%ld) O(%ld)", inbound_size, outbound_size);

  // Open buffers
  app_message_open(inbound_size, outbound_size);

  // Tell JS our version and keep trying until a reply happens
  app_timer_register(VERSION_SEND_INTERVAL_MS, send_version, NULL);
}

/*
 * Save the config data structure
 */
EXTFN void save_config_data(void *data) {
  LOG_DEBUG("save_config_data (%d)", sizeof(config_data));
  int written = persist_write_data(PERSIST_CONFIG_KEY, &config_data, sizeof(config_data));
  if (written != sizeof(config_data)) {
    LOG_ERROR("save_config_data error (%d)", written);
  }
  save_config_requested = false;
}

/*
 * Clear config if needed
 */
static void clear_config_data() {
  memset(&config_data, 0, sizeof(config_data));
  config_data.config_ver = CONFIG_VER;
  strncpy(config_data.entry[0].menu_text, NO_CONFIG_1, MENU_TEXT_LEN);
  strncpy(config_data.entry[1].menu_text, NO_CONFIG_2, MENU_TEXT_LEN);
}

/*
 * Read the config data (or create it if missing)
 */
EXTFN void read_config_data() {
  int read = persist_read_data(PERSIST_CONFIG_KEY, &config_data, sizeof(config_data));
  if (read != sizeof(config_data) || config_data.config_ver != CONFIG_VER) {
    clear_config_data();
  }
}

/*
 * Provide config data structure to other units
 */
EXTFN ConfigData *get_config_data() {
  return &config_data;
}

/*
 * Get the config to save at some point in the future
 */
EXTFN void trigger_config_save() {
  if (!save_config_requested) {
    app_timer_register(PERSIST_CONFIG_MS, save_config_data, NULL);
    save_config_requested = true;
  }
}

/*
 * Create main window
 */
static void handle_init() {
  // Check which defines are defined
#ifdef PBL_SDK_2
  LOG_INFO("PBL_SDK_2");
#endif
#ifdef PBL_SDK_3
  LOG_INFO("PBL_SDK_3");
#endif
#ifdef PBL_PLATFORM_APLITE
  LOG_INFO("PBL_PLATFORM_APLITE");
#endif
#ifdef PBL_PLATFORM_BASALT
  LOG_INFO("PBL_PLATFORM_BASALT");
#endif
#ifdef PBL_PLATFORM_CHALK
  LOG_INFO("PBL_PLATFORM_CHALK");
#endif
#ifdef PBL_COLOR
  LOG_INFO("PBL_COLOR");
#endif
#ifdef PBL_BW
  LOG_INFO("PBL_BW");
#endif
#ifdef PBL_ROUND
  LOG_INFO("PBL_ROUND");
#endif
#ifdef PBL_RECT
  LOG_INFO("PBL_RECT");
#endif

  read_config_data();

  show_menu();

  open_comms();

}

/*
 * Main
 */
EXTFN int main(void) {
  handle_init();
  app_event_loop();
}

