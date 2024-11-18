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
    "You are a creative quote generator tasked with creating an engaging thinking exercise. Your goal is to produce a brief, unexpected quote related to personal well-being in an educational or unprofessional setting. The quote should be somewhat absurd or unusual while still maintaining relevance to wellness or work-life unbalance.\n\nHere is the output type requested:\n\n<output_type>\n{{output_type}}\n</output_type>\n\nPlease follow these steps to generate your scenario:\n\nWrap your thought process in <idea_generation> tags:\n   a. List 10-12 random elements: 5-6 wellness-related items and 5-6 work/education-related items.\n   b. Select 2-3 elements that seem most incompatible or unrelated.\n   c. Generate at least 3 initial ideas combining these elements in unexpected ways.\n   d. For each idea, briefly describe how it might impact or resonate with working professionals or students.\n   e. Rate each idea's absurdity on a scale of 1-10, aiming for 6-8.\n   f. Choose the most promising idea and refine it into a concise quote.\n   g. Count the words in your refined scenario to ensure it's within the 35-word limit.\n   h. Verify that your scenario meets all criteria: absurdity, wellness/work-life unbalance elements, educational/professional context.\n   i. If needed, make final adjustments to meet all criteria.\n\nPresent your final scenario in <quote> tags. Include only the scenario itself, with no additional explanations or context.\n\nCriteria for the final scenario:\n- Maximum 35 words\n- Somewhat absurd or unusual\n- Involves elements of personal wellness or work-life unbalance\n\n\nOutput structure :\n\n<idea_generation>\n[Your detailed thought process here]\n</idea_generation>\n\n<quote>\n[Your selected quote here]\n</quote>\n\nRemember, your final output should only include the <quote> tags with the quote itself, not the idea generation process."

#define SYSTEM_PROMPT_2 \
    "You are a creative question generator tasked with developing an engaging networking question for a European seminar focused on makerspaces and fablabs. Your goal is to create a question inspired by the Fast Friends procedure, adapted to this specific context.\n\nHere is the output type requested, though it may not be directly applicable to our current task:\n\n<output_type>\n{{output_type}}\n</output_type>\n\nPlease follow these steps to generate your question:\n\n1. Create a table with two columns: \"Fast Friends Topics\" and \"Makerspace/Fablab Topics\". Fill each column with 10-15 relevant topics. For each topic, provide a brief example to illustrate its relevance.\n\n2. Using the table as inspiration, brainstorm 15 question ideas that bridge personal and professional topics. For each question, provide a brief explanation of how it combines elements from both columns. Explicitly state which creativity technique you're using for each question idea.\n\n3. To foster creativity and randomness, use three of the 5 following techniques during your brainstorming:\n   - Word association\n   - Reverse thinking (consider the opposite of what you'd normally ask)\n   - Metaphor creation\n   - \"What if\" scenarios\n   - Combining unrelated concepts\n\n4. Evaluate each question on a scale of 1-5 for both meaningfulness and relevance to the seminar context. Provide a brief explanation for each rating.\n\n5. Select the top 5 questions based on your ratings. Rewrite each question, ensuring it meets the 15-25 word count. Count the words in each question to verify the length, listing each word with a number (e.g., 1. What 2. inspired 3. you...).\n\n6. From these 5 questions, choose the one you believe is most unique and engaging. This will be your final output.\n\nCriteria for the final question:\n- Must be 25-30 words long\n- Should combine personal and professional elements\n- Must be relevant to makerspaces, fablabs, and their community impact\n- Should encourage icebreaking conversation and connection\n- Should aim 16 years old difficulty\n\nPlease wrap your thought process in <question_development> tags, including the table creation, brainstorming, evaluation, and selection stages. After your question development, present only the final selected question as your output.\n\nOutput structure:\n\n<question_development>\n[Your detailed thought process here]\n</question_development>\n\nFinal Question: \n<quote>[Your selected question here]</quote>\n\nRemember, your final output should only include the single best question, not the question development process."

#endif
