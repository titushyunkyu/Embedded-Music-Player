#ifndef FSM_H_
#define FSM_H_

#include <ti/devices/msp/msp.h>
#include "../LP_MSPM0G3507/bsp.h"

// Defines for pushbutton values
#define INACTIVE 0
#define ACTIVE 1

// FSM state type
typedef enum {
    Start, Stop
} FSMState;

typedef struct {
    uint32_t CurrentInput;                      // Current input of the FSM
    FSMState CurrentState;                      // Current state of the FSM
    FSMState (*NextStateFuncPtr)(void *FSM);    // Next-state function pointer
    void (*OutputFuncPtr)(void *FSM);           // Output function pointer
} FSMType;

typedef struct FSMTableStruct {
    const FSMState CurrentState;
    const struct FSMTableStruct *NextState[2];   // Next state of the FSM
} FSMTableType;

// Function Prototypes
FSMState NextStateFunction(void *FSM);
void OutputFunction(void *FSM);

#endif /* FSM_H_ */
