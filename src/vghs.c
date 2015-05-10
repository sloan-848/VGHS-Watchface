#include <pebble.h>

static Window *window;
static TextLayer *date_layer;
static TextLayer *time_layer;

static GFont acme_font;

static GBitmap *crest;
static BitmapLayer *crest_layer;

static PropertyAnimation *crest_fall_animation;
static PropertyAnimation *time_slide_animation;
static PropertyAnimation *date_slide_animation;

static void trigger_crest_fall_animation() {
  Layer * crest_parent = bitmap_layer_get_layer(crest_layer);

  // Set start and end point
  GRect from_frame = layer_get_frame(crest_parent);
  GRect to_frame = GRect(0,7,from_frame.size.w,from_frame.size.h);

  crest_fall_animation = property_animation_create_layer_frame(crest_parent, &from_frame, &to_frame);

  animation_set_duration((Animation*) crest_fall_animation, 1000);
  animation_set_curve((Animation*) crest_fall_animation, AnimationCurveEaseOut);

  animation_schedule((Animation*) crest_fall_animation);
}

static void trigger_time_slide_animation() {
  Layer *time_parent = text_layer_get_layer(time_layer);

  // Set start and end point
  GRect from_frame = layer_get_frame(time_parent);
  GRect to_frame = GRect(6, 135, from_frame.size.w, from_frame.size.h);

  time_slide_animation = property_animation_create_layer_frame(time_parent, &from_frame, &to_frame);

  animation_set_duration((Animation*) time_slide_animation, 1000);
  animation_set_curve((Animation*) time_slide_animation, AnimationCurveEaseOut);

  animation_schedule((Animation*) time_slide_animation);
}

static void trigger_date_slide_animation() {
  Layer *date_parent = text_layer_get_layer(date_layer);

  // Set start and end point
  GRect from_frame = layer_get_frame(date_parent);
  GRect to_frame = GRect(90, 135, from_frame.size.w, from_frame.size.h);

  date_slide_animation = property_animation_create_layer_frame(date_parent, &from_frame, &to_frame);

  animation_set_duration((Animation*) date_slide_animation, 1000);
  animation_set_curve((Animation*) date_slide_animation, AnimationCurveEaseOut);

  animation_schedule((Animation*) date_slide_animation);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  int hour = tick_time->tm_hour;
  int minute = tick_time->tm_min;

  int month = tick_time->tm_mon;
  int day = tick_time->tm_mday;

  static char time_buffer[16];
  static char date_buffer[16];
  if(!clock_is_24h_style()){
    if(hour > 12){
      hour -= 12;
    }
  }
  // Update the TextLayer
  snprintf(time_buffer, sizeof(time_buffer), "%i:%02i", hour, minute);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Time update: %s", time_buffer);
  text_layer_set_text(time_layer, time_buffer);

  // Update the DateLayer
  snprintf(date_buffer, sizeof(date_buffer), "%i/%i", month, day);
  text_layer_set_text(date_layer, date_buffer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  acme_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ACME_22));

  window_set_background_color(window, GColorCobaltBlue);

  // Set up Time Text
  static char firstTime[16];
  time_layer = text_layer_create((GRect) { .origin = { -49, 135}, .size = { 55, 30 } });
  text_layer_set_font(time_layer, acme_font);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  // Set up Date Text
  static char firstDate[16];
  clock_copy_time_string(firstDate, sizeof(firstDate));
  date_layer = text_layer_create((GRect) { .origin = { 140, 135}, .size = { 54, 30 } });
  text_layer_set_text(date_layer, firstDate);
  text_layer_set_font(date_layer, acme_font);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_background_color(date_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));

  // Set up crest png layer
  int logo_size = 144;
  crest = gbitmap_create_with_resource(RESOURCE_ID_CREST);
  crest_layer = bitmap_layer_create(GRect(0, -100, bounds.size.w, logo_size));
  bitmap_layer_set_bitmap(crest_layer, crest);
  bitmap_layer_set_compositing_mode(crest_layer, GCompOpSet);

  layer_add_child(window_layer, bitmap_layer_get_layer(crest_layer));

  trigger_crest_fall_animation();
  trigger_time_slide_animation();
  trigger_date_slide_animation();
}

static void window_unload(Window *window) {
  gbitmap_destroy(crest);
  bitmap_layer_destroy(crest_layer);
//  property_animation_destroy(crest_fall_animation);
//  property_animation_destroy(time_slide_animation);
//  property_animation_destroy(date_slide_animation);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  // Subscribe to minute tick
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Force first tick event
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  tick_handler(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
