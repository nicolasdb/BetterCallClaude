#ifndef CONFIG_H
#define CONFIG_H

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Debug verbosity: 0 for reduced output, 1 for verbose debug
#define DEBUG_VERBOSE 0

// Pin definitions for different boards
#if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define PIN_BUTTON 10
    #define PIN_LED 2
    #define LED_TYPE SK6812
#elif defined(ARDUINO_M5STACK_STAMPS3)
    #define PIN_BUTTON 10
    #define PIN_LED 21
    #define LED_TYPE WS2812
#else
    #error "Unsupported board selected"
#endif

// Prompt selector pins (updated)
#define PIN_PROMPT_SELECTOR_0 8
#define PIN_PROMPT_SELECTOR_1 7
#define PIN_PROMPT_SELECTOR_2 6
#define NUM_PROMPTS 3  // Updated to match the number of prompts

// Analog input pins for potentiometers
#define PIN_POTENTIOMETER_1 3
#define PIN_POTENTIOMETER_2 4

// Thermal printer configuration
#define THERMAL_PRINTER_RX 1
#define THERMAL_PRINTER_TX 0
#define THERMAL_PRINTER_SERIAL Serial1
#define THERMAL_PRINTER_BAUD_RATE 9600
#define THERMAL_PRINTER_MAX_CHAR_PER_LINE 30

// Claude API configuration
// names : claude-3-5-sonnet-20240620 , claude-3-haiku-20240307
#define CLAUDE_MODEL "claude-3-haiku-20240307"
#define CLAUDE_MAX_TOKENS 150
#define CLAUDE_TEMPERATURE 1
#define CLAUDE_ROLE "user"

// Claude temperature and max token control
#define CLAUDE_TEMPERATURE_MIN 0.0
#define CLAUDE_TEMPERATURE_MAX 1.0
#define CLAUDE_MAX_TOKENS_MIN 50
#define CLAUDE_MAX_TOKENS_MAX 300

// Random seed range for Claude
#define CLAUDE_SEED_MIN 1
#define CLAUDE_SEED_MAX 1000000

// System prompts
// 1. Context Generator
#define SYSTEM_PROMPT_1 \
    "You are a quirky scenario generator for a creative thinking exercise. Your task is to generate a brief, unexpected scenario related to personal well-being in an educational or professional setting. The scenario should be no longer than 20 words, be somewhat absurd or unusual, and involve elements of personal wellness or work-life balance. Do not include any explanations or additional context - just provide the scenario itself."

// 2. Pain Point Identifier
#define SYSTEM_PROMPT_2 \
    "Generate a brief, unusual challenge related to managing or applying knowledge in a learning or work environment. Be creative and concise, describing the problem in no more than 20 words. Focus solely on the issue without offering solutions or additional context. Vary your sentence structure and avoid using introductory phrases."

// 3. CULTIVATE Principle Quote
#define SYSTEM_PROMPT_3 \
    "You are an inspiring quote generator for an organizational development tool. Your task is to create a short, memorable quote that embodies one of the following principles: Collective Intelligence, Uniqueness, Learning, Technology Use, Integrity, Valuing Collaboration, Awareness, Transparency, or Enabling Experiences. The quote should be no longer than 15 words, be thought-provoking, and not explicitly mention the principle itself. Provide only the quote, without any additional explanation or context."

#define MESSAGE_TEMPLATE "Generate a conversation starter."

// LED configuration
#define NUM_LEDS 1
#define LED_BRIGHTNESS 34

// LED states
typedef enum {
    WAITING_FOR_WIFI,
    WAITING_FOR_INPUT,
    WAITING_FOR_API,
    IDLE
} LedState;

#endif // CONFIG_H