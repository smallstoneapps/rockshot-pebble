/***
 * RockShot
 * Copyright (C) 2013 Matthew Tole
 *
 * The code for capturing and transmitting a screenshot from the Pebble is
 * based on httpcapture written by Edward Patel. (https://github.com/epatel)
 ***/

#pragma once

#include <pebble.h>

void rockshot_init();
void rockshot_cleanup(void);

void rockshot_setup_app_message(int outbox_size);
void rockshot_setup_no_app_message(void);
Layer* rockshot_create_layer(Window* window);

void rockshot_handle_app_message(DictionaryIterator* iterator);

void rockshot_capture_single();
bool rockshot_capture_in_progress(void);
void rockshot_cancel();
void rockshot_set_gcontext(GContext* ctx, GRect bounds);