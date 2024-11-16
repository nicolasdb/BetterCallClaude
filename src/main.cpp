#include "config.h"
#include "wifi_manager.h"
#include "api_client.h"
#include "quote_parser.h"
#include "printer_driver.h"
#include "input_handler.h"
#include "feedback_display.h"

void setup() {
  // Initialize modules
  wifi_manager_init();
  api_client_init();
  quote_parser_init();
  printer_driver_init();
  input_handler_init();
  feedback_display_init();
}

void loop() {
  // Run state machine
  switch (state) {
    case INITIALIZATION:
      // Initialize device and connect to WiFi
      wifi_manager_connect();
      break;
    case IDLE:
      // Wait for button presses or sensor inputs
      input_handler_wait_for_input();
      break;
    case QUOTE_GENERATION:
      // Send API request and parse response
      api_client_send_request();
      quote_parser_parse_response();
      break;
    case PRINTING:
      // Handle printer output and display feedback state
      printer_driver_print_quote();
      feedback_display_show_printing_state();
      break;
    case ERROR:
      // Handle errors and display error state
      feedback_display_show_error_state();
      break;
  }
}