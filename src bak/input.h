#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include "config.h"

void setupButton();
bool isButtonPressed();
bool isButtonReleased();
void debugButtonState();
int getSystemPromptSelector();
float getClaudeTemperature();
int getClaudeMaxTokens();

#endif // INPUT_H