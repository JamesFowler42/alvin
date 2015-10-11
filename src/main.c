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

  last_request = time(NULL);

}

/*
 * Incoming message handler
 */
static void in_received_handler(DictionaryIterator *iter, void *context) {

  // Got a ctrl message
  Tuple *ctrl_tuple = dict_find(iter, KEY_CTRL);
  if (ctrl_tuple) {
    
    int32_t ctrl_value = ctrl_tuple->value->int32;

    // If transmit is done then mark it
    if (ctrl_value & CTRL_TRANSMIT_DONE) {
      internal_data.transmit_sent = true;
      set_icon(true, IS_EXPORT);
      // If we're waiting for previous nights data to be sent, it now has been, reset and go
      if (complete_outstanding) {
        app_timer_register(COMPLETE_OUTSTANDING_MS, reset_sleep_period_action, NULL);
      }
      // If Morpheuz has woken to send data, then once the data is sent, speed up the shutdown
      // Normally around for 5 minutes but no need for that once data has been sent
      if (auto_shutdown_timer != NULL) {
        app_timer_reschedule(auto_shutdown_timer, TEN_SECONDS_MS);
      }
    }

    // If version is done then mark it
    if (ctrl_value & CTRL_VERSION_DONE) {
      version_sent = true;
      config_data.lazarus = ctrl_value & CTRL_LAZARUS;
      trigger_config_save();
    }

    // If gone off is done then mark that
    if (ctrl_value & CTRL_GONEOFF_DONE) {
      internal_data.gone_off_sent = true;
    }

    // Only let the last sent become it's new value after confirmation
    // from the JS
    if (ctrl_value & CTRL_SET_LAST_SENT) {
      LOG_DEBUG("in_received_handler - CTRL_SET_LAST_SENT to %d", new_last_sent);
      internal_data.last_sent = new_last_sent;
    }

    // If the request is to continue then do so.
    if (ctrl_value & CTRL_DO_NEXT) {
      app_timer_register(SHORT_RETRY_MS, transmit_next_data, NULL);
    }

    // Yes - must have comms
    set_icon(true, IS_COMMS);
    last_response = time(NULL);

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

  LOG_DEBUG("internal_data %d, config_data %d", sizeof(internal_data), sizeof(config_data));

  // Register message handlers
  app_message_register_inbox_received(in_received_handler);

  // Incoming size
  Tuplet in_values[] = { TupletInteger(KEY_CTRL, 0) };

  uint32_t inbound_size = dict_calc_buffer_size_from_tuplets(in_values, ARRAY_LENGTH(in_values)) + FUDGE;

  LOG_DEBUG("I(%ld) O(%ld)", inbound_size, inbound_size);

  // Open buffers
  app_message_open(inbound_size, inbound_size);

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
  config_data.fromhr = FROM_HR_DEF;
  config_data.frommin = FROM_MIN_DEF;
  config_data.tohr = TO_HR_DEF;
  config_data.tomin = TO_MIN_DEF;
  config_data.from = to_mins(FROM_HR_DEF,FROM_MIN_DEF);
  config_data.to = to_mins(TO_HR_DEF,TO_MIN_DEF);
  config_data.lazarus = true;
  config_data.config_ver = CONFIG_VER;
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
    
}

/*
 * Main
 */
EXTFN int main(void) {
  handle_init();
  app_event_loop();
}



