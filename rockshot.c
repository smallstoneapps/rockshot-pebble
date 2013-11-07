/***
 * RockShot
 * Copyright (C) 2013 Matthew Tole
 *
 * The code for capturing and transmitting a screenshot from the Pebble is
 * based on httpcapture written by Edward Patel. (https://github.com/epatel)
 ***/

#include <pebble.h>
#include "rockshot.h"

#define ROCKSHOT_DATA_PADDING 64
#define ROCKSHOT_COOKIE_SEND 7625002

#define ROCKSHOT_KEY_OFFSET 76250
#define ROCKSHOT_KEY_DATA 76251
#define ROCKSHOT_KEY_CHECK 76252
#define ROCKSHOT_KEY_OP 76253
#define ROCKSHOT_KEY_ID 76254
#define ROCKSHOT_KEY_BYTES_LEFT 76255
#define ROCKSHOT_KEY_SIZE 76256

typedef struct {
  unsigned char* data;
  int bytes_sent;
  int size;
  int id;
  GRect bounds;
} Screenshot;

struct GContext {
  void **ptr;
};

static void capture_screenshot(Screenshot* shot);
static void send_screenshot(Screenshot* shot);
static void send_bytes(Screenshot* shot);
static void send_more_bytes();
static void data_received(DictionaryIterator *received, void *context);
static void capture_layer_update(Layer* me, GContext* ctx);

static bool in_progress = false;
static Screenshot screenshot;
static AppTimer* timer_sending = NULL;
static int chunk_size = -1;
static GContext* gctx = NULL;

void rockshot_init(void) {
  in_progress = false;
  screenshot.data = NULL;
}

void rockshot_setup_app_message(int outbox_size) {
  chunk_size = outbox_size - ROCKSHOT_DATA_PADDING;
}

void rockshot_setup_no_app_message(void) {
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_register_inbox_received(data_received);
  chunk_size = app_message_outbox_size_maximum() - ROCKSHOT_DATA_PADDING;
}

void rockshot_handle_app_message(DictionaryIterator* iterator) {
  data_received(iterator, NULL);
}

void rockshot_cleanup(void) {
  if (screenshot.data != NULL) {
    free(screenshot.data);
  }
  if (timer_sending != NULL) {
    app_timer_cancel(timer_sending);
  }
}

void rockshot_setup(int outgoing_buffer_size) {
  in_progress = false;
}

void rockshot_capture_single(int id) {
  if (in_progress) { return; }
  in_progress = true;
  screenshot.id = id;
  capture_screenshot(&screenshot);
  send_screenshot(&screenshot);
}

bool rockshot_capture_in_progress() {
  return in_progress;
}

void rockshot_cancel(int id) {
  if (id != screenshot.id) {
    return;
  }
  if (timer_sending != NULL) {
    app_timer_cancel(timer_sending);
    timer_sending = NULL;
  }
  in_progress = false;
}

void rockshot_set_gcontext(GContext* ctx, GRect bounds) {
  gctx = ctx;
  screenshot.bounds = bounds;
  screenshot.size = bounds.size.h * (bounds.size.w / 8);
}

Layer* rockshot_create_layer(Window* window) {
  Layer* capture_layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
  layer_add_child(window_get_root_layer(window), capture_layer);
  layer_set_update_proc(capture_layer, capture_layer_update);
  return capture_layer;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void rockshot_timer_handler() {
  send_bytes(&screenshot);
}

static void capture_screenshot(Screenshot* screenshot) {
  screenshot->data = malloc(sizeof(unsigned char) * screenshot->size);
  unsigned char *ptr = (unsigned char *)(gctx->ptr);

  int len = 0;
  for (int y = 0; y < screenshot->bounds.size.h; y += 1) {
    for (int x=0; x < screenshot->bounds.size.w / 8; x += 1) {
      screenshot->data[len] = *ptr++;
      len += 1;
    }
    ptr++;
    ptr++;
  }
}

static void data_received(DictionaryIterator *received, void *context) {

  Tuple* tup_check = dict_find(received, ROCKSHOT_KEY_CHECK);
  Tuple* tup_id = dict_find(received, ROCKSHOT_KEY_ID);
  Tuple* tup_op = dict_find(received, ROCKSHOT_KEY_OP);

  if (tup_check == NULL) {
    return;
  }

  if (strcmp(tup_check->value->cstring, "rockshot") != 0) {
    return;
  }

  if (tup_op == NULL) {
    return;
  }

  if (strcmp(tup_op->value->cstring, "single") == 0) {
    rockshot_cancel(screenshot.id);
    rockshot_capture_single(tup_id->value->int32);
  }
  else if (strcmp(tup_op->value->cstring, "cancel") == 0) {
    rockshot_cancel(tup_id->value->int32);
  }
}

static void send_screenshot(Screenshot* shot) {
  shot->bytes_sent = 0;
  send_more_bytes();
  app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
}

static void send_bytes(Screenshot* shot) {
  int bytes_to_send = shot->size - shot->bytes_sent;
  if (bytes_to_send > chunk_size) {
    bytes_to_send = chunk_size;
  }

  if (bytes_to_send <= 0) {
    in_progress = false;
    app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);
    return;
  }

  DictionaryIterator* dict;
  AppMessageResult result = app_message_outbox_begin(&dict);
  if (result == APP_MSG_BUSY) {
    send_more_bytes();
    return;
  }

  dict_write_int16(dict, ROCKSHOT_KEY_OFFSET, shot->bytes_sent);
  dict_write_int16(dict, ROCKSHOT_KEY_ID, shot->id);
  dict_write_int16(dict, ROCKSHOT_KEY_SIZE, shot->size);
  dict_write_int16(dict, ROCKSHOT_KEY_BYTES_LEFT, shot->bytes_sent);
  dict_write_data(dict, ROCKSHOT_KEY_DATA, &shot->data[shot->bytes_sent], bytes_to_send);

  result = app_message_outbox_send();
  if (result == APP_MSG_BUSY) {
    send_more_bytes();
    return;
  }

  shot->bytes_sent += bytes_to_send;
  send_more_bytes();
}

static void send_more_bytes() {
  timer_sending = app_timer_register(50, rockshot_timer_handler, NULL);
}

static void capture_layer_update(Layer* me, GContext* ctx) {
  rockshot_set_gcontext(ctx, layer_get_bounds(me));
}