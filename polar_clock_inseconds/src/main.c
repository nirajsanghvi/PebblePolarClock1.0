/*

  Polar Clock watch (with optional seconds)

  Thanks to Team Pebble's Segment Six watchface...it was a big help!

	Ported by ollien.
 */



#include "pebble.h"

#define SHOW_SECONDS 1
#define BIG_SECONDS 0 //Only applies when SHOW_SECONDS is selected
#define SHOW_TEXT_TIME 1 //Don't use this option with BIG_SECONDS
#define SHOW_TEXT_DATE 1
#define ROW_DATE 1

#define TWENTY_FOUR_HOUR_DIAL 0

Window * window;

Layer * minute_display_layer;
Layer * hour_display_layer;


#if SHOW_SECONDS
Layer * second_display_layer;
#endif

#if SHOW_TEXT_TIME
TextLayer * text_time_layer;
#endif

#if SHOW_TEXT_DATE
TextLayer * text_date_layer;
#endif

struct tm * t;
time_t now;
const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-7, -70}, // 70 = radius + fudge; 7 = 70*tan(6 degrees); 6 degrees per minute;
    {7,  -70},
  }
};

GPath * minute_segment_path;


const GPathInfo HOUR_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-6, -58}, // 58 = radius + fudge; 6 = 58*tan(6 degrees); 30 degrees per hour;
    {6,  -58},
  }
};

GPath * hour_segment_path;


#if SHOW_SECONDS

const GPathInfo SECOND_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-5, -48}, // 48 = radius + fudge; 5 = 48*tan(6 degrees); 6 degrees per second;
    {5,  -48},
  }
};

GPath * second_segment_path;

#endif



void minute_display_layer_update_callback(Layer *me, GContext* ctx) {


  time(&now);
	t=localtime(&now);

  unsigned int angle = t->tm_min * 6;


  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, GPoint(layer_get_bounds(me).size.w/2,layer_get_bounds(me).size.h/2), 65);

  graphics_context_set_fill_color(ctx, GColorBlack);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, minute_segment_path);

  }

  graphics_fill_circle(ctx, GPoint(layer_get_bounds(me).size.w/2,layer_get_bounds(me).size.h/2), 60);

}


void hour_display_layer_update_callback(Layer *me, GContext* ctx) {



  time(&now);

  unsigned int angle;

  #if TWENTY_FOUR_HOUR_DIAL
    angle = (t->tm_hour * 15) + (t->tm_min / 4);
  #else
    angle = (( t->tm_hour % 12 ) * 30) + (t->tm_min / 2);
  #endif

  angle = angle - (angle % 6);


  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, GPoint(layer_get_bounds(me).size.w/2,layer_get_bounds(me).size.h/2), 55);

  graphics_context_set_fill_color(ctx, GColorBlack);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(hour_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, hour_segment_path);
  }

  graphics_fill_circle(ctx, GPoint(layer_get_bounds(me).size.w/2,layer_get_bounds(me).size.h/2), 50);
}

#if SHOW_SECONDS

void second_display_layer_update_callback(Layer *me, GContext* ctx) {



	time(&now);

  unsigned int angle = t->tm_sec * 6;


  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, GPoint(layer_get_bounds(me).size.w/2,layer_get_bounds(me).size.h/2), 45);

  graphics_context_set_fill_color(ctx, GColorBlack);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(second_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, second_segment_path);

  }

#if BIG_SECONDS
  graphics_fill_circle(ctx, GPoint(layer_get_bounds(me).size.w/2,layer_get_bounds(me).size.h/2), 25);
#else
  graphics_fill_circle(ctx, GPoint(layer_get_bounds(me).size.w/2,layer_get_bounds(me).size.h/2), 40);
#endif

}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {


  //only update hour and minute arcs once every minute
  if(t->tm_sec == 0)
  {
    layer_mark_dirty(minute_display_layer);
    layer_mark_dirty(hour_display_layer);
  }

  layer_mark_dirty(second_display_layer);

  #if SHOW_TEXT_TIME

  // Need to be static because it's used by the system later.
  static char time_text[] = "00:00";

  char *time_format;

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, t);

  text_layer_set_text(text_time_layer, time_text);

  #endif

  #if SHOW_TEXT_DATE

    #if ROW_DATE
    static char date_text[] = "00 Xxx";
    strftime(date_text, sizeof(date_text), "%e %b", t);
    #else
    static char date_text[] = "Xxx 00";
    strftime(date_text, sizeof(date_text), "%b %e", t);
    #endif

  text_layer_set_text(text_date_layer, date_text);

  #endif

}

#else

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {



  layer_mark_dirty(minute_display_layer);
  layer_mark_dirty(hour_display_layer);

  #if SHOW_TEXT_TIME

  // Need to be static because it's used by the system later.
  static char time_text[] = "00:00";

  char *time_format;

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, t);

  text_layer_set_text(text_time_layer, time_text);

  #endif

  #if SHOW_TEXT_DATE

    #if ROW_DATE
    static char date_text[] = "00 Xxx";
    strftime(date_text, sizeof(date_text), "%e %b", t);
    #else
    static char date_text[] = "Xxx 00";
    strftime(date_text, sizeof(date_text), "%b %e", t);
    #endif

  text_layer_set_text(text_date_layer, date_text);

  #endif

}

#endif

void handle_init(void) {

  window=window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);


  // Init the layer for the minute display
minute_display_layer=layer_create(layer_get_frame(window_get_root_layer(window)));
	layer_set_update_proc(minute_display_layer, minute_display_layer_update_callback);
  layer_add_child(window_get_root_layer(window), minute_display_layer);

  // Init the minute segment path
	minute_segment_path=gpath_create(&MINUTE_SEGMENT_PATH_POINTS);
  gpath_move_to(minute_segment_path, GPoint(layer_get_bounds(minute_display_layer).size.w/2,layer_get_bounds(minute_display_layer).size.h/2));

  // Init the layer for the hour display
  hour_display_layer=layer_create(layer_get_bounds(window_get_root_layer(window)));
	layer_set_update_proc(hour_display_layer, hour_display_layer_update_callback);
  layer_add_child(window_get_root_layer(window), hour_display_layer);

  // Init the hour segment path
  hour_segment_path=gpath_create(&HOUR_SEGMENT_PATH_POINTS);
  gpath_move_to(hour_segment_path, GPoint(layer_get_bounds(hour_display_layer).size.w/2,layer_get_bounds(hour_display_layer).size.h/2));

  #if SHOW_SECONDS

  // Init the layer for the second display
  second_display_layer=layer_create(layer_get_bounds(window_get_root_layer(window)));
  layer_set_update_proc(second_display_layer, second_display_layer_update_callback);
  layer_add_child(window_get_root_layer(window), second_display_layer);

  // Init the second segment path
  second_segment_path=gpath_create(&SECOND_SEGMENT_PATH_POINTS);
  gpath_move_to(second_segment_path, GPoint(layer_get_bounds(second_display_layer).size.w/2,layer_get_bounds(second_display_layer).size.h/2));
  
  #endif

  #if SHOW_TEXT_TIME

  text_time_layer=text_layer_create(layer_get_bounds(window_get_root_layer(window)));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
	  text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
layer_add_child(window_get_root_layer(window),text_layer_get_layer(text_time_layer));
#endif
									
  #if SHOW_TEXT_DATE
  layer_set_frame(text_layer_get_layer(text_time_layer), GRect(47, 57, 144-47, 168-57));
  #else
  layer_set_frame(text_layer_get_layer(text_time_layer), GRect(47, 70, 144-47, 168-70));

  text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_time_layer));

  #endif

  #if SHOW_TEXT_DATE

  text_date_layer=text_layer_create(layer_get_frame(window_get_root_layer(window)));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
    #if SHOW_TEXT_TIME
    layer_set_frame(text_layer_get_layer(text_date_layer), GRect(44, 80, 144-44, 168-80));
    #else
    layer_set_frame(text_layer_get_layer(text_date_layer.layer), GRect(44, 70, 144-44, 168-70));
    #endif
  text_layer_set_font(text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_date_layer));

  #endif
									
									      #if SHOW_SECONDS
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
      #else
		  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
      #endif

}

void handle_deinit(void){
	text_layer_destroy(text_time_layer);
	window_destroy(window);
	tick_timer_service_unsubscribe();
}

int main(void) {


handle_init();

  app_event_loop();
  handle_deinit();
}
