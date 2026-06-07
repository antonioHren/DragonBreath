#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

enum GameState {
  IDLE,
  INHALE,
  SUCCESS,
  FAILED
};

void display_init();
void display_update(GameState state, float targetFlow, float duration);

#endif
