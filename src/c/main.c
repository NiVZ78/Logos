#include "pebble.h"


#define LOGOS_PER_IMAGE 4
#define LOGO_WIDTH 55
#define LOGO_HEIGHT 55
  

// Set the maximum no of images - this should be the number of images you have in the resources
static int max_images = 8;
 
// Array to hold the resource ID numbers - need to add each of the IDENTIFIER names you gave to each resource
const int IMAGE_ID[] = {
  RESOURCE_ID_IMAGE_1,
  RESOURCE_ID_IMAGE_2
};


// Integer to hold the number of image we are currently displaying
static int image_no = 0;

// Pointer to main window and layer
static Window *s_main_window;
static Layer *s_main_window_layer;

// Pointer to mockup image and layer
static GBitmap *s_multi_logo;
static GBitmap *s_logo;
static BitmapLayer *s_logo_layer;

// GPoint to hold X,Y of center of screen
static GPoint s_main_window_center;

static void change_image(){

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "IMAGE: %d", image_no);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "HEAP BEFORE CHANGE: %zu", heap_bytes_free());
  
  bitmap_layer_set_bitmap(s_logo_layer, NULL);
  gbitmap_destroy(s_logo);
  gbitmap_destroy(s_multi_logo);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "HEAP AFTER DESTROY: %zu", heap_bytes_free());
  
  // Divide by the number of logos in each image to get the array index of the multi logo image to load
  int multi_logo_number = image_no / LOGOS_PER_IMAGE;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "** Using MULTI IMAGE: %d", multi_logo_number);
  s_multi_logo = gbitmap_create_with_resource(IMAGE_ID[multi_logo_number]);
    
  // Get the remainder of dividing by 4 to select the individual logo to cut out
  int logo_number = image_no%4;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "** Using SUB IMAGE: %d", logo_number);
  s_logo = gbitmap_create_as_sub_bitmap(s_multi_logo, GRect(55 * logo_number, 0, LOGO_WIDTH, LOGO_HEIGHT));
  
  uint8_t image_width = gbitmap_get_bounds(s_logo).size.w;
  uint8_t image_height = gbitmap_get_bounds(s_logo).size.h;
  
  layer_set_bounds(bitmap_layer_get_layer(s_logo_layer), GRect(s_main_window_center.x - (image_width/2), s_main_window_center.y - (image_height/2), image_width, image_height));
    
  bitmap_layer_set_bitmap(s_logo_layer, s_logo);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "HEAP AFTER CHANGE: %zu", heap_bytes_free());
    
}


// Handle the button clicks
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {

  // Nothing for select to do - could make it display image no
  
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  
  // decrease to show previous image
  image_no -= 1;
  
  // Check if we are at minimum and loop round by setting to max_images - 1
  if (image_no < 0){
    image_no = max_images - 1;
  }
  
  // Call the function that switches images and displays it on the layer
  change_image();
}


static void down_click_handler(ClickRecognizerRef recognizer, void *context) {  
  
  // increase for next image
  image_no += 1;
  
  // check if we are at maximum and loop round by setting to zero
  if (image_no > max_images - 1){
    image_no = 0;
  }
  
  // Call the function that switches images and displays it on the layer
  change_image();
  
}

static void click_config_provider(void *context) {
  
    // Setup each of the click providers
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}




// WINDOW LOAD
static void main_window_load(Window *window) {

  // Get the root window layer
  s_main_window_layer = window_get_root_layer(s_main_window);

  // Get the size of the main window - as size now different for Rect or Round
  GRect s_main_window_layer_frame = layer_get_frame(s_main_window_layer);
  
  // Get the centre point of the screen
  s_main_window_center = grect_center_point(&s_main_window_layer_frame);
  
  // Create the layer that will hold the mockup bitmap image
  s_logo_layer = bitmap_layer_create(s_main_window_layer_frame);

  // Set the bitmap layer to align the image in the center - useful if using the same 144x168 rect image on round
  bitmap_layer_set_alignment(s_logo_layer, GAlignCenter);
  
  // Add the mockup window to the main window
  layer_add_child(s_main_window_layer, bitmap_layer_get_layer(s_logo_layer));
  
  // Load the first image
  change_image();
  
}


// WINDOW UNLOAD
static void main_window_unload(Window *window) {

  //Destroy the mockup layer and image
  layer_remove_from_parent(bitmap_layer_get_layer(s_logo_layer));
  bitmap_layer_destroy(s_logo_layer);
  gbitmap_destroy(s_logo);
  s_logo = NULL;
    
}


// INIT
static void init(void) {

  // Create main Window
  s_main_window = window_create();
    
  window_set_background_color(s_main_window, GColorBlack);
  
  // Handle clicks
  window_set_click_config_provider(s_main_window, click_config_provider);
  
  // Set handlers to manage the loading and unloading of elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
    
}


// DEINIT
static void deinit(void) {

  // Destroy the main window
  window_destroy(s_main_window);
  
}


// MAIN PROGRAM LOOP
int main(void) {
  init();
  app_event_loop();
  deinit();
}