#include <pebble.h>

// Window and layer handles
static Window *s_main_window;
static Layer *s_canvas_layer;

// Animation state
static int s_animation_counter = 0;

// Electron trail effect
#define MAX_TRAIL_LENGTH 6  // Longer trails for better visual effect
#define NUM_ELECTRONS 5
static GPoint s_electron_trails[NUM_ELECTRONS][MAX_TRAIL_LENGTH];

// Model parameters
static const int NUCLEUS_RADIUS = 10;
static const int ORBIT_RADIUS = 45;  // Using circular orbits as requested

// Update procedure for the display
static void update_proc(Layer *layer, GContext *ctx) {
  // Get screen dimensions
  GRect bounds = layer_get_bounds(layer);
  int width = bounds.size.w;
  int height = bounds.size.h;
  
  // Center point for the atom
  GPoint center = GPoint(width / 2, height / 2 - 20);
  
  // Draw nucleus (red on color, white on B&W)
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorRed);
  #else
    graphics_context_set_fill_color(ctx, GColorWhite);
  #endif
  graphics_fill_circle(ctx, center, NUCLEUS_RADIUS);
  
  // Draw the star shape as the electron path
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_context_set_stroke_width(ctx, 1);
  
  // Draw star shape with curved paths between points
  for (int i = 0; i < NUM_ELECTRONS; i++) {
    int next_idx = (i + 2) % NUM_ELECTRONS;  // Connect to point 2 ahead for star shape
    
    // Calculate the star points
    int32_t angle1 = (i * TRIG_MAX_ANGLE) / NUM_ELECTRONS;
    int32_t angle2 = (next_idx * TRIG_MAX_ANGLE) / NUM_ELECTRONS;
    
    // Star points
    int x1 = center.x + (sin_lookup(angle1) * ORBIT_RADIUS / TRIG_MAX_RATIO);
    int y1 = center.y - (cos_lookup(angle1) * ORBIT_RADIUS / TRIG_MAX_RATIO);
    int x2 = center.x + (sin_lookup(angle2) * ORBIT_RADIUS / TRIG_MAX_RATIO);
    int y2 = center.y - (cos_lookup(angle2) * ORBIT_RADIUS / TRIG_MAX_RATIO);
    
    // Draw the line connecting the star points
    graphics_draw_line(ctx, GPoint(x1, y1), GPoint(x2, y2));
  }
  
  // Draw electrons with enhanced trail effect
  for (int i = 0; i < NUM_ELECTRONS; i++) {
    // Calculate current position along the star path
    int line_segment = (s_animation_counter / 15) % NUM_ELECTRONS;
    int progress = (s_animation_counter % 15) * 100 / 15;  // 0-100% progress along current line
    
    // The starting point is determined by the line segment we're on
    int start_point = (line_segment + i) % NUM_ELECTRONS;
    int end_point = (start_point + 2) % NUM_ELECTRONS;  // Star pattern connects to point 2 ahead
    
    // Calculate angles for start and end points
    int32_t start_angle = (start_point * TRIG_MAX_ANGLE) / NUM_ELECTRONS;
    int32_t end_angle = (end_point * TRIG_MAX_ANGLE) / NUM_ELECTRONS;
    
    // Calculate start and end coordinates
    int start_x = center.x + (sin_lookup(start_angle) * ORBIT_RADIUS / TRIG_MAX_RATIO);
    int start_y = center.y - (cos_lookup(start_angle) * ORBIT_RADIUS / TRIG_MAX_RATIO);
    int end_x = center.x + (sin_lookup(end_angle) * ORBIT_RADIUS / TRIG_MAX_RATIO);
    int end_y = center.y - (cos_lookup(end_angle) * ORBIT_RADIUS / TRIG_MAX_RATIO);
    
    // Interpolate between start and end based on progress
    int current_x = start_x + ((end_x - start_x) * progress) / 100;
    int current_y = start_y + ((end_y - start_y) * progress) / 100;
    
    // Store current position for trail effect
    // Shift existing trail positions
    for (int t = MAX_TRAIL_LENGTH - 1; t > 0; t--) {
      s_electron_trails[i][t] = s_electron_trails[i][t-1];
    }
    s_electron_trails[i][0] = GPoint(current_x, current_y);
    
    // Draw the trail effect (from oldest to newest for proper layering)
    for (int t = MAX_TRAIL_LENGTH - 1; t >= 0; t--) {
      if (t == 0 || (s_electron_trails[i][t].x != 0 && s_electron_trails[i][t].y != 0)) {
        #ifdef PBL_COLOR
          // Different colors for each electron in color mode
          switch (i % 5) {
            case 0: graphics_context_set_fill_color(ctx, GColorBlue); break;
            case 1: graphics_context_set_fill_color(ctx, GColorGreen); break;
            case 2: graphics_context_set_fill_color(ctx, GColorMagenta); break;
            case 3: graphics_context_set_fill_color(ctx, GColorYellow); break;
            case 4: graphics_context_set_fill_color(ctx, GColorCyan); break;
          }
        #else
          graphics_context_set_fill_color(ctx, GColorWhite);
        #endif
        
        // Electron size varies with trail position - larger head, gradually smaller tail
        int radius = 5 - t;
        if (radius < 1) radius = 1;
        
        graphics_fill_circle(ctx, s_electron_trails[i][t], radius);
      }
    }
  }
  
  // Draw time
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  static char time_text[8];
  strftime(time_text, sizeof(time_text), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);
  
  graphics_context_set_text_color(ctx, GColorWhite);
  GFont time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  GRect time_bounds = GRect(0, height / 2 + 15, width, 50);
  graphics_draw_text(ctx, time_text, time_font, time_bounds, 
                    GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  
  // Draw date
  static char date_text[16];
  strftime(date_text, sizeof(date_text), "%b %d", t);
  
  GFont date_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  GRect date_bounds = GRect(0, height / 2 + 55, width, 28);
  graphics_draw_text(ctx, date_text, date_font, date_bounds,
                    GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
                    
  // Draw battery level in top right corner
  BatteryChargeState battery = battery_state_service_peek();
  
  // Battery icon dimensions
  int battery_width = 20;
  int battery_height = 10;
  int battery_x = width - battery_width - 10;  // 10px from right edge
  int battery_y = 10;  // 10px from top
  
  // Battery outline
  graphics_context_set_stroke_color(ctx, GColorWhite);
  GRect battery_outline = GRect(battery_x - 2, battery_y, battery_width + 4, battery_height);
  graphics_draw_rect(ctx, battery_outline);
  
  // Battery terminal
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(battery_x + battery_width + 2, battery_y + 2, 2, 6), 0, GCornerNone);
  
  // Battery level
  int level_width = (battery_width * battery.charge_percent) / 100;
  #ifdef PBL_COLOR
    if (battery.charge_percent > 50) {
      graphics_context_set_fill_color(ctx, GColorGreen);
    } else if (battery.charge_percent > 20) {
      graphics_context_set_fill_color(ctx, GColorYellow);
    } else {
      graphics_context_set_fill_color(ctx, GColorRed);
    }
  #else
    graphics_context_set_fill_color(ctx, GColorWhite);
  #endif
  graphics_fill_rect(ctx, GRect(battery_x, battery_y + 2, level_width, 6), 0, GCornerNone);
  
  // Draw charging indicator if applicable
  if (battery.is_charging) {
    static char charging_text[] = "+";
    graphics_draw_text(ctx, charging_text, fonts_get_system_font(FONT_KEY_GOTHIC_14), 
                      GRect(battery_x - 12, battery_y - 3, 15, 15),
                      GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  }
  
  // Draw battery percentage next to the icon
  static char battery_text[6];
  snprintf(battery_text, sizeof(battery_text), "%d%%", battery.charge_percent);
  
  graphics_context_set_text_color(ctx, GColorWhite);
  GFont battery_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  GRect battery_text_bounds = GRect(battery_x - 30, battery_y - 3, 25, 16);
  graphics_draw_text(ctx, battery_text, battery_font, battery_text_bounds,
                    GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
}

// Handler for time changes
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Update animation state at a good speed for fluid motion
  s_animation_counter = (s_animation_counter + 2) % (15 * NUM_ELECTRONS);
  
  // Force the screen to redraw
  layer_mark_dirty(s_canvas_layer);
}

// Battery state change handler
static void battery_callback(BatteryChargeState state) {
  layer_mark_dirty(s_canvas_layer);
}

// Window load event
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create the main display layer
  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, update_proc);
  layer_add_child(window_layer, s_canvas_layer);
  
  // Initialize electron trail arrays
  for (int i = 0; i < NUM_ELECTRONS; i++) {
    for (int t = 0; t < MAX_TRAIL_LENGTH; t++) {
      s_electron_trails[i][t] = GPoint(0, 0);
    }
  }
}

// Window unload event
static void window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

// Initialize the watchface
static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  
  // Subscribe to the tick timer service for animation
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  // Subscribe to battery state service for battery updates
  battery_state_service_subscribe(battery_callback);
  
  window_stack_push(s_main_window, true);
}

// Cleanup when exiting
static void deinit() {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  window_destroy(s_main_window);
}

// Main program
int main() {
  init();
  app_event_loop();
  deinit();
  return 0;
}