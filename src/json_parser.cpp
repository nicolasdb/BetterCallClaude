#include "json_parser.h"

const char* JsonParser::START_TAG = "<quote>";
const char* JsonParser::END_TAG = "</quote>";

String JsonParser::extractQuote(const String& jsonString) {
    int startPos = jsonString.indexOf(START_TAG);
    if (startPos == -1) {
        return ""; // Tag not found
    }
    
    startPos += strlen(START_TAG); // Move to end of start tag
    int endPos = jsonString.indexOf(END_TAG, startPos);
    
    if (endPos == -1) {
        return ""; // End tag not found
    }
    
    // Extract and trim the content between tags
    String quote = jsonString.substring(startPos, endPos);
    return trim(quote);
}

String JsonParser::trim(const String& str) {
    String result = str;
    
    // Remove leading whitespace and newlines
    while (result.length() > 0 && (result[0] == ' ' || result[0] == '\n' || result[0] == '\r' || result[0] == '\t')) {
        result = result.substring(1);
    }
    
    // Remove trailing whitespace and newlines
    while (result.length() > 0 && (result[result.length() - 1] == ' ' || 
           result[result.length() - 1] == '\n' || 
           result[result.length() - 1] == '\r' || 
           result[result.length() - 1] == '\t')) {
        result = result.substring(0, result.length() - 1);
    }
    
    return result;
}
