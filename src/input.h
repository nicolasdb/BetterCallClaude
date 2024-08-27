#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include "config.h"

void setupButton();
bool isButtonPressed();
int getSystemPromptSelector();

#endif // INPUT_H