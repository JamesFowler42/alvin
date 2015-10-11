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

#ifndef ALVIN_H_
#define ALVIN_H_

#include "pebble.h"

#define VERSION 10
#define VERSION_TXT "1.0"

// Comment out for production build - leaves errors on BASALT/CHALK and nothing on APLITE as this is much tighter for memory
//#define TESTING_BUILD

#ifdef TESTING_BUILD
  #define LOG_ERROR(fmt, args...) app_log(APP_LOG_LEVEL_ERROR, "", 0, fmt, ## args)
  #define LOG_WARN(fmt, args...) app_log(APP_LOG_LEVEL_WARNING, "", 0, fmt, ## args)
  #define LOG_INFO(fmt, args...) app_log(APP_LOG_LEVEL_INFO, "", 0, fmt, ## args)
  #define LOG_DEBUG(fmt, args...) app_log(APP_LOG_LEVEL_DEBUG, "", 0, fmt, ## args)
#else
  #ifndef PBL_PLATFORM_APLITE 
    #define LOG_ERROR(fmt, args...) app_log(APP_LOG_LEVEL_ERROR, "", 0, fmt, ## args)
  #else
    #define LOG_ERROR(fmt, args...)
  #endif
  #define LOG_WARN(fmt, args...) 
  #define LOG_INFO(fmt, args...) 
  #define LOG_DEBUG(fmt, args...) 
#endif
  
// Read clock mode from OS
#define IS_24_HOUR_MODE clock_is_24h_style()
//#define IS_24_HOUR_MODE false

// Only do this to make greping for external functions easier (lot of space to be saved with statics)
#define EXTFN

#define FUDGE 4

#define POST_MENU_ACTION_DISPLAY_UPDATE_MS 900
#define MENU_ACTION_MS 750
#define MENU_ACTION_HIDE_MS 500

#ifdef PBL_COLOR 
  #define BACKGROUND_COLOR GColorDukeBlue
  #define MENU_TEXT_COLOR GColorWhite
  #define MENU_HIGHLIGHT_BACKGROUND_COLOR GColorBlack
  #define MENU_BACKGROUND_COLOR BACKGROUND_COLOR
  #define MENU_HEAD_COLOR GColorWhite
#else
  #define BACKGROUND_COLOR GColorBlack
  #define MENU_HEAD_COLOR GColorBlack
#endif

// These save space and time to run and a direct cast is claimed to be supported in the documentation
#define bitmap_layer_get_layer_jf(x) ((Layer *)(x))
#define text_layer_get_layer_jf(x) ((Layer *)(x))
#define inverter_layer_get_layer_jf(x) ((Layer *)(x))
#define menu_layer_get_layer_jf(x) ((Layer *)(x))

enum AlvinKey {
  KEY_POINT = 1,
  KEY_CTRL = 2,
  KEY_FROM = 3,
  KEY_TO = 4,
  KEY_BASE = 5,
  KEY_VERSION = 6,
  KEY_GONEOFF = 7,
  KEY_TRANSMIT = 8,
  KEY_AUTO_RESET = 9
};

enum CtrlValues {
  CTRL_TRANSMIT_DONE = 1,
  CTRL_VERSION_DONE = 2,
  CTRL_GONEOFF_DONE = 4,
  CTRL_DO_NEXT = 8,
  CTRL_SET_LAST_SENT = 16,
  CTRL_LAZARUS = 32
};

#define PERSIST_CONFIG_KEY 12122
#define PERSIST_CONFIG_MS 30000
#define SHORT_RETRY_MS 200
#define LONG_RETRY_MS 60000
#define VERSION_SEND_INTERVAL_MS (1000)
#define VERSION_SEND_SLOW_INTERVAL_MS (60*1000)

#define MENU_TEXT_LEN 20
#define MAX_MENU_ENTRY 10

typedef struct {
  char menu_text[MENU_TEXT_LEN];
  bool toggle;
  bool on;
} MenuEntry;
  
// Change the CONFIG_VER only if the ConfigData struct changes
#define CONFIG_VER 42
typedef struct {
  uint8_t config_ver;
  MenuEntry entry [MAX_MENU_ENTRY];
} ConfigData;


// Externals

#endif /* ALVIN_H_ */

