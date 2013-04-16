#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x16, 0x54, 0xF7, 0xAA, 0x3D, 0x24, 0x4E, 0xF5, 0xAB, 0xDA, 0x6F, 0xC0, 0x15, 0x12, 0xB1, 0xC7 }
PBL_APP_INFO(MY_UUID,
             "SimplicityPlus", "Jim Ho",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;


int firstrun = 1; // Variable to check for first run. 1 - YES. 0 - NO.

TextLayer text_day_layer;
TextLayer text_date_layer;
TextLayer text_time_layer;
TextLayer text_time_second_layer;

Layer line_layer;


void line_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me;
    
    graphics_context_set_stroke_color(ctx, GColorWhite);
    
    graphics_draw_line(ctx, GPoint(6, 75), GPoint(144-6, 75));
    graphics_draw_line(ctx, GPoint(6, 76), GPoint(144-6, 76));
    
}


void handle_init(AppContextRef ctx) {
    
    (void)ctx;
    
    
    window_init(&window, "SimplicityPlus");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);
    
    resource_init_current_app(&APP_RESOURCES);
    
    // Day
    text_layer_init(&text_day_layer, window.layer.frame);
    text_layer_set_text_color(&text_day_layer, GColorWhite);
    text_layer_set_background_color(&text_day_layer, GColorClear);
    layer_set_frame(&text_day_layer.layer, GRect(6, 15, 144-6, 25));
    text_layer_set_font(&text_day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
    text_layer_set_text_alignment(&text_day_layer, GTextAlignmentLeft);
    layer_add_child(&window.layer, &text_day_layer.layer);
    
    
    // Month and Date
    text_layer_init(&text_date_layer, window.layer.frame);
    text_layer_set_text_color(&text_date_layer, GColorWhite);
    text_layer_set_background_color(&text_date_layer, GColorClear);
    layer_set_frame(&text_date_layer.layer, GRect(6, 40, 144-6, 30));
    text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
    text_layer_set_text_alignment(&text_date_layer, GTextAlignmentLeft);
    layer_add_child(&window.layer, &text_date_layer.layer);
    
    
    // Hour and Minute
    text_layer_init(&text_time_layer, window.layer.frame);
    text_layer_set_text_color(&text_time_layer, GColorWhite);
    text_layer_set_background_color(&text_time_layer, GColorClear);
    layer_set_frame(&text_time_layer.layer, GRect(6, 80, 144-6, 66));
    text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
    text_layer_set_text_alignment(&text_time_layer, GTextAlignmentLeft);
    layer_add_child(&window.layer, &text_time_layer.layer);
    
    
    // Second
    text_layer_init(&text_time_second_layer, window.layer.frame);
    text_layer_set_text_color(&text_time_second_layer, GColorWhite);
    text_layer_set_background_color(&text_time_second_layer, GColorClear);
    layer_set_frame(&text_time_second_layer.layer, GRect(106, 128, 30, 30));
    text_layer_set_font(&text_time_second_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_21)));
    text_layer_set_text_alignment(&text_time_second_layer, GTextAlignmentLeft);
    layer_add_child(&window.layer, &text_time_second_layer.layer);
    
    
    layer_init(&line_layer, window.layer.frame);
    line_layer.update_proc = &line_layer_update_callback;
    layer_add_child(&window.layer, &line_layer);
    
}


void handle_tick(AppContextRef ctx, PebbleTickEvent *t) {
    
    (void)ctx;
    
    static char second_text[] = "00"; // Need to be static because they're used by the system later.
    
    // Update Every Second
    char *second_format;
    
    // Second Formatting Type
    second_format = "%S";
    
    // Second Formatting and Update
    string_format_time(second_text, sizeof(second_text), second_format, t->tick_time);
    text_layer_set_text(&text_time_second_layer, second_text);
    
    // Get Time From Pebble To Check Which Layers To Update
    PblTm pebble_time;
    get_time(&pebble_time);
    
    // Update Every Minute
    if (pebble_time.tm_sec == 0 || firstrun == 1) {
        
        static char time_text[] = "00:00"; // Need to be static because they're used by the system later.
        
        char *time_format;
        
        // Hour & Minute Formatting Type
        if (clock_is_24h_style()) {
            time_format = "%R";
        } else {
            time_format = "%I:%M";
        }
        
        // Hour & Minute Formatting and Update
        string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);
        
        // Kludge to handle lack of non-padded hour format string
        // for twelve hour clock.
        if (!clock_is_24h_style() && (time_text[0] == '0')) {
            memmove(time_text, &time_text[1], sizeof(time_text) - 1);
        }
        
        text_layer_set_text(&text_time_layer, time_text);
    }
    
    
    // Update Every Hour
    if (pebble_time.tm_min == 0 || firstrun == 1) {
        
        static char day_text[] = "XXX W00 D000"; // Need to be static because they're used by the system later.
        static char date_text[] = "0000-00-0000"; // Need to be static because they're used by the system later.
        
        // Day Formatting and Update
        string_format_time(day_text, sizeof(day_text), "%a W%W D%j", t->tick_time);
        text_layer_set_text(&text_day_layer, day_text);
        
        // Month & Date Formatting and Update
        string_format_time(date_text, sizeof(date_text), "%Y-%m-%d", t->tick_time);
        text_layer_set_text(&text_date_layer, date_text);
    }
    
    
    // Set firstrun to 0
    if (firstrun == 1) {
        firstrun = 0;
    }
}


void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        
        .tick_info = {
            .tick_handler = &handle_tick,
            .tick_units = SECOND_UNIT
        }
        
    };
    app_event_loop(params, &handlers);
}
