/*

  Polar Clock watch (with optional seconds)

  Thanks to Team Pebble's Segment Six watchface...it was a big help!

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x57, 0x90, 0xEA, 0xC3, 0x00, 0xD5, 0x4C, 0xB2, 0xB1, 0x8F, 0x22, 0x0C, 0x4F, 0xB5, 0x89, 0x1B }
PBL_APP_INFO(MY_UUID, 
	"Polar Clock Lite", "Niraj Sanghvi",
	1, 0,
	RESOURCE_ID_IMAGE_MENU_ICON,
	APP_INFO_WATCH_FACE);

#define SHOW_SECONDS 0
#define BIG_SECONDS 0 //Only applies when SHOW_SECONDS is selected
#define SHOW_TEXT_TIME 1 //Don't use this option with BIG_SECONDS
#define SHOW_TEXT_DATE 1
#define ROW_DATE 1

#define TWENTY_FOUR_HOUR_DIAL 0

Window window;

Layer minute_display_layer;
Layer hour_display_layer;

#if SHOW_SECONDS
Layer second_display_layer;
#endif

#if SHOW_TEXT_TIME
TextLayer text_time_layer;
#endif

#if SHOW_TEXT_DATE
TextLayer text_date_layer;
#endif


const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-7, -70}, // 70 = radius + fudge; 7 = 70*tan(6 degrees); 6 degrees per minute;
    {7,  -70},
  }
};

GPath minute_segment_path;


const GPathInfo HOUR_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-6, -58}, // 58 = radius + fudge; 6 = 58*tan(6 degrees); 30 degrees per hour;
    {6,  -58},
  }
};

GPath hour_segment_path;


#if SHOW_SECONDS

const GPathInfo SECOND_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-5, -48}, // 48 = radius + fudge; 5 = 48*tan(6 degrees); 6 degrees per second;
    {5,  -48},
  }
};

GPath second_segment_path;

#endif


void minute_display_layer_update_callback(Layer *me, GContext* ctx) {

  PblTm t;

  get_time(&t);

  unsigned int angle = t.tm_min * 6;

  GPoint center = grect_center_point(&me->frame);

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, 65);

  graphics_context_set_fill_color(ctx, GColorBlack);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(&minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, &minute_segment_path);

  }

  graphics_fill_circle(ctx, center, 60);

}


void hour_display_layer_update_callback(Layer *me, GContext* ctx) {

  PblTm t;

  get_time(&t);

  unsigned int angle;

  #if TWENTY_FOUR_HOUR_DIAL
    angle = (t.tm_hour * 15) + (t.tm_min / 4);
  #else
    angle = (( t.tm_hour % 12 ) * 30) + (t.tm_min / 2);
  #endif

  angle = angle - (angle % 6);

  GPoint center = grect_center_point(&me->frame);

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, 55);

  graphics_context_set_fill_color(ctx, GColorBlack);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(&hour_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, &hour_segment_path);
  }

  graphics_fill_circle(ctx, center, 50);
}

#if SHOW_SECONDS

void second_display_layer_update_callback(Layer *me, GContext* ctx) {

  PblTm t;

  get_time(&t);

  unsigned int angle = t.tm_sec * 6;

  GPoint center = grect_center_point(&me->frame);

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, 45);

  graphics_context_set_fill_color(ctx, GColorBlack);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(&second_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, &second_segment_path);

  }

#if BIG_SECONDS
  graphics_fill_circle(ctx, center, 25);
#else
  graphics_fill_circle(ctx, center, 40);
#endif

}

void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)ctx;

  //only update hour and minute arcs once every minute
  if(t->tick_time->tm_sec == 0)
  {
    layer_mark_dirty(&minute_display_layer);
    layer_mark_dirty(&hour_display_layer);
  }

  layer_mark_dirty(&second_display_layer);

  #if SHOW_TEXT_TIME

  // Need to be static because it's used by the system later.
  static char time_text[] = "00:00";

  char *time_format;

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);

  text_layer_set_text(&text_time_layer, time_text);

  #endif

  #if SHOW_TEXT_DATE

    #if ROW_DATE
    static char date_text[] = "00 Xxx";
    string_format_time(date_text, sizeof(date_text), "%e %b", t->tick_time);
    #else
    static char date_text[] = "Xxx 00";
    string_format_time(date_text, sizeof(date_text), "%b %e", t->tick_time);
    #endif

  text_layer_set_text(&text_date_layer, date_text);

  #endif

}

#else

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  layer_mark_dirty(&minute_display_layer);
  layer_mark_dirty(&hour_display_layer);

  #if SHOW_TEXT_TIME

  // Need to be static because it's used by the system later.
  static char time_text[] = "00:00";

  char *time_format;

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);

  text_layer_set_text(&text_time_layer, time_text);

  #endif

  #if SHOW_TEXT_DATE

    #if ROW_DATE
    static char date_text[] = "00 Xxx";
    string_format_time(date_text, sizeof(date_text), "%e %b", t->tick_time);
    #else
    static char date_text[] = "Xxx 00";
    string_format_time(date_text, sizeof(date_text), "%b %e", t->tick_time);
    #endif

  text_layer_set_text(&text_date_layer, date_text);

  #endif

}

#endif

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Polar Clock watch");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  // Init the layer for the minute display
  layer_init(&minute_display_layer, window.layer.frame);
  minute_display_layer.update_proc = &minute_display_layer_update_callback;
  layer_add_child(&window.layer, &minute_display_layer);

  // Init the minute segment path
  gpath_init(&minute_segment_path, &MINUTE_SEGMENT_PATH_POINTS);
  gpath_move_to(&minute_segment_path, grect_center_point(&minute_display_layer.frame));

  // Init the layer for the hour display
  layer_init(&hour_display_layer, window.layer.frame);
  hour_display_layer.update_proc = &hour_display_layer_update_callback;
  layer_add_child(&window.layer, &hour_display_layer);

  // Init the hour segment path
  gpath_init(&hour_segment_path, &HOUR_SEGMENT_PATH_POINTS);
  gpath_move_to(&hour_segment_path, grect_center_point(&hour_display_layer.frame));

  #if SHOW_SECONDS

  // Init the layer for the second display
  layer_init(&second_display_layer, window.layer.frame);
  second_display_layer.update_proc = &second_display_layer_update_callback;
  layer_add_child(&window.layer, &second_display_layer);

  // Init the second segment path
  gpath_init(&second_segment_path, &SECOND_SEGMENT_PATH_POINTS);
  gpath_move_to(&second_segment_path, grect_center_point(&second_display_layer.frame));
  
  #endif

  #if SHOW_TEXT_TIME

  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);

  #if SHOW_TEXT_DATE
  layer_set_frame(&text_time_layer.layer, GRect(47, 57, 144-47, 168-57));
  #else
  layer_set_frame(&text_time_layer.layer, GRect(47, 70, 144-47, 168-70));
  #endif

  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_time_layer.layer);

  #endif

  #if SHOW_TEXT_DATE

  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
    #if SHOW_TEXT_TIME
    layer_set_frame(&text_date_layer.layer, GRect(44, 80, 144-44, 168-80));
    #else
    layer_set_frame(&text_date_layer.layer, GRect(44, 70, 144-44, 168-70));
    #endif
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_date_layer.layer);

  #endif

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    // Handle time updates
    .tick_info = {
      #if SHOW_SECONDS
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
      #else
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
      #endif
    }

  };
  app_event_loop(params, &handlers);
}
