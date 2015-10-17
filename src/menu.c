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

// Constants
#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS 2
#define FLOURISH_LEFT ((width - 144) / 2)

#define STATE_NORMAL 0
#define STATE_SENDING 1
#define STATE_SENT 2
#define STATE_FAILED 3
#define STATE_SUCCESS 4
  
// Private
static Window *window;
static MenuLayer *menu_layer = NULL;
static GBitmap *menu_icons[NUM_MENU_ICONS];
static bool connected;
static int16_t selected_row;
static int16_t centre;
static int16_t width;
static uint8_t state = STATE_NORMAL;
static GBitmap *flourish;
static AppTimer *restore_timer = NULL;

/*
 * Reload menu
 */
EXTFN void reload_menu() {
  menu_layer_reload_data(menu_layer);
}

/*
 * Force the menu to redraw
 */
static void redraw_menu() {
  layer_mark_dirty(menu_layer_get_layer_jf(menu_layer));
}

/*
 * Restore the menu item after sending
 */
static void restore_menu(void *data) {
  state = STATE_NORMAL;
  restore_timer = NULL;
  redraw_menu();
}

/*
 * Set the menu item to a failed state
 */
EXTFN void set_failed() {
  state = STATE_FAILED;
  redraw_menu();
  if (restore_timer == NULL) {
    restore_timer = app_timer_register(MENU_STATE_TIME_MS, restore_menu, NULL);
  } else {
    app_timer_reschedule(restore_timer, MENU_STATE_TIME_MS);
  }
  vibes_short_pulse();
}

/*
 * Set the menu item to a success state
 */
EXTFN void set_success() {
  state = STATE_SUCCESS;
  // Change the state of the item
  if (get_config_data()->entry[selected_row].toggle) {
    get_config_data()->entry[selected_row].on = !get_config_data()->entry[selected_row].on;
  }
  redraw_menu();
  if (restore_timer == NULL) {
    restore_timer = app_timer_register(MENU_STATE_TIME_MS, restore_menu, NULL);
  } else {
    app_timer_reschedule(restore_timer, MENU_STATE_TIME_MS);
  }
  vibes_double_pulse();
}

/*
 * Set the menu item to a sent state
 */
EXTFN void set_sent() {
  if (state != STATE_SENDING) {
    return;
  }
  state = STATE_SENT;
  redraw_menu();
  if (restore_timer == NULL) {
    restore_timer = app_timer_register(MENU_STATE_TIME_MS, restore_menu, NULL);
  } else {
    app_timer_reschedule(restore_timer, MENU_STATE_TIME_MS);
  }
}

/*
 * Set the menu item to a sending state
 */
static void set_sending() {
  state = STATE_SENDING;
  redraw_menu();
  if (restore_timer == NULL) {
    restore_timer = app_timer_register(MENU_STATE_TIME_MS, restore_menu, NULL);
  } else {
    app_timer_reschedule(restore_timer, MENU_STATE_TIME_MS);
  }
}

/*
 * A callback is used to specify the amount of sections of menu items
 * With this, you can dynamically add and remove sections
 */
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

/*
 * Each section has a number of items;  we use a callback to specify this
 * You can also dynamically add and remove items using this
 */
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  uint16_t rows = 0;
  for (int8_t i = 0; i < MAX_MENU_ENTRY; i++) {
    if (get_config_data()->entry[i].menu_text[0] != '\0') {
      rows = i + 1;
    }
  }
  return rows;
}

/*
 * A callback is used to specify the height of the section header
 */
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

/*
 * Here we draw what each header is
 */
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
     
  if (connected) {
    #ifdef PBL_COLOR
      graphics_context_set_compositing_mode(ctx, GCompOpSet);
    #endif
    graphics_draw_bitmap_in_rect(ctx, flourish, GRect(FLOURISH_LEFT, 0, 144, 16));
  } else {
    graphics_context_set_text_color(ctx, MENU_HEAD_COLOR);
    graphics_draw_text(ctx, NO_BLUETOOTH, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(0, -2, width, 32), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  }
  
}

/*
 * This is the menu item draw callback where you specify what each item should look like
 */
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

#ifdef PBL_COLOR
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
#endif

  // Pick up names from the array except for the one instance where we fiddle with it
  int16_t index = cell_index->row;
  char *menu_text = get_config_data()->entry[index].menu_text;

  GColor menu_color = MENU_HEAD_COLOR;
  if (menu_layer_get_selected_index(menu_layer).row == cell_index->row) {
    switch (state) {
      case STATE_SENDING:
        menu_text = TEXT_SENDING;
        menu_color = MENU_SENDING_COLOR;
        break;
      case STATE_SENT:
        menu_text = TEXT_SENT;
        menu_color = MENU_SENDING_COLOR;
        break;
      case STATE_FAILED:
        menu_text = TEXT_FAILED;
        menu_color = MENU_FAILED_COLOR;
        break;
      case STATE_SUCCESS:
        menu_text = TEXT_SUCCESS;
        menu_color = MENU_SUCCESS_COLOR;
        break;
    }
  }

  int8_t state = get_config_data()->entry[index].on ? 1 : 0;
  GBitmap *icon = get_config_data()->entry[index].toggle ? menu_icons[state] : NULL;

  GRect text_pos = GRect(40, 4, width - 40, 24);
  GTextAlignment text_align = GTextAlignmentLeft;
  if (!get_config_data()->active) {
    text_pos = GRect(0, 4, width, 24);
    text_align = GTextAlignmentCenter;
  }

  graphics_context_set_text_color(ctx, menu_color);
  graphics_draw_text(ctx, menu_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), text_pos, GTextOverflowModeTrailingEllipsis, text_align, NULL);

#ifndef PBL_ROUND
  if (icon != NULL) {
    graphics_draw_bitmap_in_rect(ctx, icon, GRect(10, 7, 24, 28));
  }
#else
  if (icon != NULL && menu_layer_get_selected_index(menu_layer).row == cell_index->row) {
    graphics_draw_bitmap_in_rect(ctx, icon, GRect(10, 7, 24, 28));
  }
#endif

}


/*
 * Here we capture when a user selects a menu item
 */
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  if (!get_config_data()->active || !connected) {
    return;
  }

  // Use the row to specify which item will receive the select action
  selected_row = cell_index->row;
  
  // Sending state
  set_sending();

  // Always do the action
  menu_item_fired(selected_row);
}

#ifdef PBL_SDK_3

/*
 * Menu callback from void
 */
EXTFN void callback_from_voice(int32_t menu_item) {

  // Move to the row in the menu
  MenuIndex index;
  index.section = 0;
  index.row = menu_item;
  
  menu_layer_set_selected_index(menu_layer, index, MenuRowAlignCenter, true);
  
  // Fire as if this were a menu click
  selected_row = menu_item;
  
  // Sending state
  set_sending();

  // Always do the action
  menu_item_fired(selected_row);
  
}

#endif

/*
 * Long click on select
 */
static void menu_select_long_click(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  
  #ifdef PBL_SDK_3
  
  if (!get_config_data()->active || !connected) {
    return;
  }
  
  voice_control();
  
  #endif
  
}

static void bluetooth_handler(bool is_connected) {
  if (connected != is_connected) {
    connected = is_connected;
    redraw_menu();
  }
}

/*
 * This initializes the menu upon window load
 */
static void window_load(Window *window) {

  connected = bluetooth_connection_service_peek();

  menu_icons[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_NO);
  menu_icons[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_YES);
  
  flourish = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FLOURISH);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  centre = bounds.size.w / 2;
  width = bounds.size.w;

  menu_layer = menu_layer_create(bounds);

#ifdef PBL_ROUND
  menu_layer_set_center_focused(menu_layer, true);
#endif

  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks ) 
  { .get_num_sections = menu_get_num_sections_callback, 
   .get_num_rows = menu_get_num_rows_callback, 
   .get_header_height = menu_get_header_height_callback, 
   .draw_header = menu_draw_header_callback, 
   .draw_row = menu_draw_row_callback, 
   .select_click = menu_select_callback, 
   .select_long_click = menu_select_long_click,
  });

  menu_layer_set_click_config_onto_window(menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer_jf(menu_layer));

#ifdef PBL_COLOR
  menu_layer_set_normal_colors(menu_layer, MENU_BACKGROUND_COLOR, MENU_TEXT_COLOR);
  menu_layer_set_highlight_colors(menu_layer, MENU_HIGHLIGHT_BACKGROUND_COLOR, MENU_TEXT_COLOR);
#endif

  bluetooth_connection_service_subscribe(bluetooth_handler);
}

/*
 * Unload the menu window
 */
static void window_unload(Window *window) {
  #ifdef PBL_SDK_3
    tidy_voice();
  #endif
  
  MenuLayer *temp_menu_layer = menu_layer;
  menu_layer = NULL;
  menu_layer_destroy(temp_menu_layer);
  gbitmap_destroy(menu_icons[0]);
  gbitmap_destroy(menu_icons[1]);
  save_config_data(NULL);
  bluetooth_connection_service_unsubscribe();
}

/*
 * Show the menu
 */
EXTFN void show_menu() {

  window = window_create();
#ifdef PBL_SDK_2
  window_set_fullscreen(window, true);
#endif
  window_set_window_handlers(window, (WindowHandlers ) { .load = window_load, .unload = window_unload, });
  window_stack_push(window, true);
}

