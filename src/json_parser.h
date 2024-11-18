#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <Arduino.h>

class JsonParser {
public:
    JsonParser() {}
    
    // Extract content between <quote> tags from a JSON string
    static String extractQuote(const String& jsonString);
    
    // Helper method to trim whitespace and newlines
    static String trim(const String& str);

private:
    static const char* START_TAG;
    static const char* END_TAG;
};

#endif // JSON_PARSER_H
