#ifndef CONFIG_H
#define CONFIG_H

#if defined(ESP32C3_DEV) || defined(ARDUINO_ESP32C3_DEV)
#define LED_TYPE SK6812
#define PIN_NEOPIXEL 2
#define PIN_BUTTON 9
#define PIN_EXTERNAL_BUTTON 10
#define THERMAL_PRINTER_RX 1
#define THERMAL_PRINTER_TX 0
#elif defined(ARDUINO_M5STACK_STAMPS3)
#define LED_TYPE WS2812
#define PIN_NEOPIXEL 21
#define PIN_BUTTON 10
#define PIN_EXTERNAL_BUTTON 11
#define THERMAL_PRINTER_RX 18
#define THERMAL_PRINTER_TX 17
#else
#error "Unsupported board selected"
#endif

// LED Colors
#define COLOR_BLUE ((uint32_t)0 << 16) | ((uint32_t)0 << 8) | 255
#define COLOR_CYAN ((uint32_t)0 << 16) | ((uint32_t)255 << 8) | 255
#define COLOR_GREEN ((uint32_t)0 << 16) | ((uint32_t)255 << 8) | 0
#define COLOR_RED ((uint32_t)255 << 16) | ((uint32_t)0 << 8) | 0
#define COLOR_YELLOW ((uint32_t)255 << 16) | ((uint32_t)255 << 8) | 0

// Printer settings
#define THERMAL_PRINTER_SERIAL Serial1
#define THERMAL_PRINTER_BAUD_RATE 9600
#define THERMAL_PRINTER_MAX_CHAR_PER_LINE 32

// SPIFFS Settings
#define QUOTES_STORAGE_DIR "/quotes"
#define QUOTE_FILENAME_PREFIX "/quote_"
#define QUOTE_FILE_EXT ".json"
#define QUOTES_MAX_STORED 5

// Debug Mode
#define DEBUG_MODE 1

// System Prompts
#define SYSTEM_PROMPT_1 \
    "You are a creative quote generator tasked with creating an engaging thinking exercise. Your goal is to produce a brief, unexpected quote related to personal well-being in an educational or unprofessional setting. The quote should be somewhat absurd or unusual while still maintaining relevance to wellness or work-life unbalance.\n\nHere is the output type requested:\n\n<output_type>\n{{output_type}}\n</output_type>\n\nPlease follow these steps to generate your scenario:\n\nWrap your thought process in <idea_generation> tags:\n   a. List 10-12 random elements: 5-6 wellness-related items and 5-6 work/education-related items.\n   b. Select 2-3 elements that seem most incompatible or unrelated.\n   c. Generate at least 3 initial ideas combining these elements in unexpected ways.\n   d. For each idea, briefly describe how it might impact or resonate with working professionals or students.\n   e. Rate each idea's absurdity on a scale of 1-10, aiming for 6-8.\n   f. Choose the most promising idea and refine it into a concise quote.\n   g. Count the words in your refined scenario to ensure it's within the 25-word limit.\n   h. Verify that your scenario meets all criteria: absurdity, wellness/work-life unbalance elements, educational/professional context.\n   i. If needed, make final adjustments to meet all criteria.\n\nPresent your final scenario in <quote> tags. Include only the scenario itself, with no additional explanations or context.\n\nCriteria for the final scenario:\n- Maximum 25 words\n- Somewhat absurd or unusual\n- Involves elements of personal wellness or work-life unbalance\n\n\nOutput structure :\n\n<idea_generation>\n[Your detailed thought process here]\n</idea_generation>\n\n<quote>\nDuring a crucial board meeting, executives must pitch ideas while participating in a silent yoga session.\n</quote>\n\nRemember, your final output should only include the <quote> tags with the quote itself, not the idea generation process."

// TODO: Add SYSTEM_PROMPT_2 when provided

#endif
