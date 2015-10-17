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

#ifdef PBL_SDK_3

static DictationSession *ds = NULL;
static char s1buff[MENU_TEXT_LEN];
static char s2buff[MENU_TEXT_LEN];

/*
 * Generate a compare string, ignoring duplicate letters, spaces and switches to lowercase
 */
static void build_compare_string(char *out, char *in, size_t n) {
  // Clear buffer
  memset(out, 0, n);
  
  // Copy ignoring spaces and turning uppercase to lower. Ignores duplicate letters too.
  char *ob = out;
  char *ib = in;
  char l = '$';
  for (uint8_t i = 0; i < n; i++) {
    char a = *ib++;
    if (a == l) {
      continue;
    }
    l = a;
    if (a == ' ')
      continue;
    if (a == '\0')
      break;
    if (('A' <= a) && (a <= 'Z')) 
      a = 'a' + (a - 'A');
    *ob++ = a;
  }
}

/*
 * Does a case, duplicate blind and space blind comparision
 */
static int strncmp_ignore_spaces_and_case (char *s1, char *s2, size_t n) {
  if (n == 0)
    return 0;
  
  build_compare_string(s1buff, s1, n);
  build_compare_string(s2buff, s2, n);
  
  return strncmp(s1buff, s2buff, n);
}

/*
 * Once dication has happened this fires, for better or worse
 */
static void voice_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) {
  
  // If for worse then this is where we go
  if (status != DictationSessionStatusSuccess) {
    LOG_WARN("dictation failed");
    dictation_session_stop(session);
    vibes_short_pulse();
    return;
  }
  
  // If we were lucky then we try to match the transcription to a menu item
  dictation_session_stop(session);
  
  LOG_INFO("dictation got %s", transcription);
  bool found = false;
  for (int8_t i = 0; i < MAX_MENU_ENTRY; i++) {
    if (get_config_data()->entry[i].menu_text[0] != '\0') {
      
      if (strncmp_ignore_spaces_and_case(get_config_data()->entry[i].menu_text, transcription, MENU_TEXT_LEN) == 0) {
        
        // If we get a match then, select the menu item and go
        LOG_INFO("Invoking menu %s", get_config_data()->entry[i].menu_text);
        callback_from_voice(i);
        found = true;
        break;
      }
    }
  }
  
  // Feedback if we didn't find anything
  if (!found) {
    vibes_short_pulse();
  }
  
}

/*
 * Start voice control
 */
EXTFN void voice_control() {
  
  if (ds == NULL) {
    ds = dictation_session_create(MENU_TEXT_LEN, voice_callback, NULL);
    dictation_session_enable_confirmation(ds, false);
    dictation_session_enable_error_dialogs(ds, false);
  }
  
  dictation_session_start(ds);
  
}

/*
 * Close away voice control
 */
EXTFN void tidy_voice() {
    if (ds != NULL) {
      dictation_session_destroy(ds);
    }
}

#endif
