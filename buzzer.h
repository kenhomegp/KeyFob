
#ifndef BUZZER_H
#define BUZZER_H

#include "hal_types.h"

// Function prototypes
extern void buzzerInit(void);
extern uint8 buzzerStart(uint16 frequency);
extern void buzzerStop(void);
#endif
