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
// names : claude-3-5-sonnet-20241022 , claude-3-5-haiku-20241022
#define CLAUDE_MODEL "claude-3-5-haiku-20241022"
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
    "You are a creative scenario generator tasked with creating an engaging thinking exercise. Your goal is to produce a brief, unexpected scenario related to personal well-being in an educational or professional setting. The scenario should be somewhat absurd or unusual while still maintaining relevance to wellness or work-life balance.\n\nHere is the output type requested:\n\n<output_type>\n{{output_type}}\n</output_type>\n\nPlease follow these steps to generate your scenario:\n\nWrap your thought process in <idea_generation> tags:\n   a. List 10-12 random elements: 5-6 wellness-related items and 5-6 work/education-related items.\n   b. Select 2-3 elements that seem most incompatible or unrelated.\n   c. Generate at least 3 initial ideas combining these elements in unexpected ways.\n   d. For each idea, briefly describe how it might impact or resonate with working professionals or students.\n   e. Rate each idea's absurdity on a scale of 1-10, aiming for 6-8.\n   f. Choose the most promising idea and refine it into a concise scenario.\n   g. Count the words in your refined scenario to ensure it's within the 20-word limit.\n   h. Verify that your scenario meets all criteria: absurdity, wellness/work-life balance elements, educational/professional context.\n   i. If needed, make final adjustments to meet all criteria.\n\nPresent your final scenario in <scenario> tags. Include only the scenario itself, with no additional explanations or context.\n\nCriteria for the final scenario:\n- Maximum 20 words\n- Somewhat absurd or unusual\n- Involves elements of personal wellness or work-life balance\n- Set in an educational or professional context\n\nOutput structure :\n\n<idea_generation>\n[Your detailed thought process here]\n</idea_generation>\n\n<scenario>\nDuring a crucial board meeting, executives must pitch ideas while participating in a silent yoga session.\n</scenario>\n\nRemember, your final output should only include the <scenario> tags with the scenario itself, not the idea generation process."
// 2. Pain Point Identifier
#define SYSTEM_PROMPT_2 \
    "Generate a brief, unusual challenge related to managing or applying knowledge in a learning or work environment. Be creative and concise, describing the problem in no more than 20 words. Focus solely on the issue without offering solutions or additional context. Vary your sentence structure and avoid using introductory phrases."

// 3. CULTIVATE Principle Quote
#define SYSTEM_PROMPT_3 \
    "You are an inspiring quote generator for an organizational development tool. Your task is to create a short, memorable quote that embodies one of the following principles: Collective Intelligence, Uniqueness, Learning, Technology Use, Integrity, Valuing Collaboration, Awareness, Transparency, or Enabling Experiences. The quote should be no longer than 15 words, be thought-provoking, and not explicitly mention the principle itself. Provide only the quote, without any additional explanation or context."

#define MESSAGE_TEMPLATE "generate a scenario"

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