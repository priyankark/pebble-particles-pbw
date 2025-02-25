#include <pebble.h>

static Window *s_main_window;
static Layer *s_canvas_layer;
static GPath *s_triangle_path = NULL;
static GPath *s_hoodie_path = NULL;
static GColor s_squid_pink;

// Animation state
static int s_anim_counter = 0;
static bool s_animating = false;

// Triangle path points for guard mask - smaller and higher
static const GPathInfo TRIANGLE_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint[]) {{0, -12}, {-10, 4}, {10, 4}}
};

// Refined hoodie path points with better fit and details
static const GPathInfo HOODIE_PATH_INFO = {
  .num_points = 19,
  .points = (GPoint[]) {
    {-25, 5},      // Left collar start
    {-32, 3},      // Left shoulder point
    {-35, -2},     // Left shoulder sharp
    {-34, -15},    // Left side upper
    {-32, -25},    // Left hood base
    {-25, -35},    // Left hood curve start
    {-15, -42},    // Left hood curve
    {-8, -44},     // Left hood top
    {0, -45},      // Hood peak
    {8, -44},      // Right hood top
    {15, -42},     // Right hood curve
    {25, -35},     // Right hood curve start
    {32, -25},     // Right hood base
    {34, -15},     // Right side upper
    {35, -2},      // Right shoulder sharp
    {32, 3},       // Right shoulder point
    {25, 5},       // Right collar start
    {18, 28},      // Right bottom
    {-18, 28}      // Left bottom
  }
};

static void draw_guard(GContext *ctx, GPoint center, int anim_state) {
  // Draw filled hoodie
  if (!s_hoodie_path) {
    s_hoodie_path = gpath_create(&HOODIE_PATH_INFO);
  }
  graphics_context_set_stroke_width(ctx, 3);
  gpath_move_to(s_hoodie_path, center);
  graphics_context_set_fill_color(ctx, s_squid_pink);
  gpath_draw_filled(ctx, s_hoodie_path);
  graphics_context_set_stroke_color(ctx, s_squid_pink);
  gpath_draw_outline(ctx, s_hoodie_path);
  
  // Smoother animation using integer math
  int pulse = anim_state % 30;  // 0-29
  int radius = 35;  // Base radius
  
  // Gentle pulse effect using triangle wave
  if (pulse < 15) {
    radius += pulse / 7;  // Slowly increase
  } else {
    radius += (29 - pulse) / 7;  // Slowly decrease
  }
  graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, radius);
  graphics_context_set_stroke_color(ctx, s_squid_pink);
  graphics_draw_circle(ctx, center, radius);
  
  // Draw triangle inside with smoother animation
  if (!s_triangle_path) {
    s_triangle_path = gpath_create(&TRIANGLE_PATH_INFO);
  }
  // Calculate triangle position with subtle movement
  int offset = pulse < 15 ? pulse / 10 : (29 - pulse) / 10;  // 0-1 movement
  GPoint triangle_center = GPoint(
    center.x,
    center.y - 8 + offset  // Higher position, tiny movement
  );
  gpath_move_to(s_triangle_path, triangle_center);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 2);
  gpath_draw_outline(ctx, s_triangle_path);
}

static void update_time(Layer *layer, GContext *ctx) {
  // Get current time
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Create time string
  static char s_time_buffer[8];
  strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", tick_time);

  // Get bounds
  GRect bounds = layer_get_bounds(layer);
  int width = bounds.size.w;
  int height = bounds.size.h;

  // Set up drawing context
  graphics_context_set_stroke_color(ctx, s_squid_pink);
  graphics_context_set_fill_color(ctx, s_squid_pink);
  
  // Adjust layout based on platform
  #if defined(PBL_ROUND)
    // Round display (chalk) - adjust for circular screen
    int guard_y = height/3 - 15;  // Move up more on round display
    int time_y = height/2 + 5;    // Adjust time position
    int player_y = height*3/4 + 5;  // Adjust player number
    
    // Adjust bounds for round screen
    GRect adjusted_bounds = grect_inset(bounds, GEdgeInsets(20));
    width = adjusted_bounds.size.w;
  #else
    // Rectangle display (aplite, basalt, diorite, emery)
    int guard_y = height/3 - 10;
    int time_y = height/2;
    int player_y = height*4/5;
  #endif

  // Draw guard at top center
  GPoint guard_center = GPoint(width/2, guard_y);
  draw_guard(ctx, guard_center, s_anim_counter);

  // Draw time below guard with platform-specific spacing
  GFont time_font = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
  GRect time_bounds = GRect(0, time_y, width, 50);
  
  graphics_context_set_text_color(ctx, s_squid_pink);
  graphics_draw_text(ctx, s_time_buffer, time_font, time_bounds,
                    GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

  // Draw player number with platform-specific spacing
  static char s_player_buffer[32];
  snprintf(s_player_buffer, sizeof(s_player_buffer), "PLAYER 456");
  
  GFont player_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  GRect player_bounds = GRect(0, player_y, width, 30);
  
  graphics_draw_text(ctx, s_player_buffer, player_font, player_bounds,
                    GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void timer_callback(void *data) {
  s_anim_counter = (s_anim_counter + 1) % 60;
  layer_mark_dirty(s_canvas_layer);
  
  // Keep animation running
  app_timer_register(50, timer_callback, NULL);  // Slower animation for smoother feel
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_canvas_layer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, update_time);
  layer_add_child(window_layer, s_canvas_layer);
  
  // Start animation
  s_animating = true;
  app_timer_register(33, timer_callback, NULL);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
  if (s_triangle_path) {
    gpath_destroy(s_triangle_path);
  }
  if (s_hoodie_path) {
    gpath_destroy(s_hoodie_path);
  }
}

static void init() {
  s_main_window = window_create();
  
  // Set Squid Game pink color
  #if defined(PBL_COLOR)
    s_squid_pink = GColorFromRGB(255, 101, 193);  // FF65C1
  #else
    s_squid_pink = GColorWhite;  // White for B&W screens
  #endif
  
  window_set_background_color(s_main_window, GColorBlack);
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  
  window_stack_push(s_main_window, true);
  
  // Update time every minute
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
