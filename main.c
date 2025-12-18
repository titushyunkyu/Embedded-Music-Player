#include <ti/devices/msp/msp.h>
#include "../LP_MSPM0G3507/MKII.h"
#include "../LP_MSPM0G3507/timer.h"
#include "../LP_MSPM0G3507/tone_pitch.h"
#include "../LP_MSPM0G3507/bsp.h"
#include "../LP_MSPM0G3507/LCD.h"
#include "FSM.h"

#define PROJECT_CLOCK_FREQUENCY 32000000
#define PRESCALE 31
#define DIV 0

// SONG DATA
const uint32_t songNotes[] =     { 
                                    NOTE_E5, NOTE_E5, NOTE_E5, 
                                    NOTE_E5, NOTE_E5, NOTE_E5,
                                    NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5,

                                   NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
                                   NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, 
                                   NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_G5,

                                   NOTE_E5, NOTE_E5, NOTE_E5, 
                                   NOTE_E5, NOTE_E5, NOTE_E5,
                                   NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5,

                                   NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
                                   NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5,
                                   NOTE_G5, NOTE_G5, NOTE_F5, NOTE_D5, NOTE_C5
                                 };
char *noteNames[] =              {
                                    "E5", "E5","E5", 
                                    "E5", "E5", "E5",
                                    "E5", "G5", "C5", "D5", "E5",

                                   "F5", "F5", "F5", "F5",
                                   "F5", "E5", "E5", "E5",
                                   "E5", "D5", "D5", "E5", "D5", "G5",

                                   "E5", "E5", "E5", 
                                   "E5", "E5", "E5",
                                   "E5", "G5", "C5", "D5", "E5",

                                   "F5", "F5", "F5", "F5",
                                   "F5", "E5", "E5", "E5",
                                   "G5", "G5", "F5", "D5", "C5"
                                 };
const uint32_t noteDurations[] = {
                                    15, 15, 30, 
                                    15, 15, 30,
                                    15, 15, 15, 15, 65,

                                    15, 15, 15, 15,
                                    15, 15, 15, 15, 
                                    15, 15, 15, 15, 35, 35,

                                    15, 15, 30,
                                    15, 15, 30,
                                    15, 15, 15, 15, 65,

                                    15, 15, 15, 15,
                                    15, 15, 15, 15,
                                    15, 15, 15, 15, 65
                                 };
const uint32_t song_length = 49;

Timer_ClockConfig TimerA1ClockConfig =
{
    .clockSel = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE,
    .divideRatio = GPTIMER_CLKDIV_RATIO_DIV_BY_1,
    .prescale = PRESCALE
};

Timer_TimerConfig TimerA1TimerConfig =
{
    .period = 0,
    .timerMode = (GPTIMER_CTRCTL_CM_DOWN | GPTIMER_CTRCTL_REPEAT_REPEAT_1)
};

Timer_PWMConfig TimerA1PWMConfig =
{
    .index = PB4INDEX,
    .iomuxMode = (IOMUX_PINCM_PC_CONNECTED | IOMUX_MODE4),
    .ccr = 0,
    .ccpd = GPTIMER_CCPD_C0CCP0_OUTPUT,
    .ccctl = GPTIMER_CCCTL_01_COC_COMPARE,
    .octl = GPTIMER_OCTL_01_CCPO_FUNCVAL,
    .ccact = (GPTIMER_CCACT_01_CDACT_CCP_LOW | GPTIMER_CCACT_01_LACT_CCP_HIGH),
    .duty = 0
};

FSMState NextStateAlgorithm(void *FSM);
void OutputPWM(void *FSM);
static void Delay1ms(void);
static void stopPWM(void);


int main (void)
{
    FSMType Song_Player_FSM =
    {INACTIVE, Stop, NextStateAlgorithm, OutputPWM};

    InitializeLaunchpad(PROJECT_CLOCK_FREQUENCY);
    InitializeBoosterpack(PROJECT_CLOCK_FREQUENCY);
    InitializeTimerClock(TIMA1,&TimerA1ClockConfig);
    InitializeTimerCompare(TIMA1,&TimerA1TimerConfig);
    InitializeTimerPWM(TIMA1,&TimerA1PWMConfig);
    LCD_Init();
    LCD_FillScreen(LCD_BLACK);
    LCD_SetTextColor(LCD_CYAN);
    LCD_SetCursor(0, 0);

    while (1){
        uint32_t S1 = BP_ReadS1();
        uint32_t S2 = BP_ReadS2();
        uint32_t input = (S1 << 1) | S2;
        Song_Player_FSM.CurrentInput = input;

        Song_Player_FSM.CurrentState = Song_Player_FSM.NextStateFuncPtr((void *)&Song_Player_FSM);

        Song_Player_FSM.OutputFuncPtr((void *)&Song_Player_FSM);

        Delay1ms();
    }
    return 0;
}

FSMState NextStateAlgorithm(void *FSM)
{
    FSMType * FSM_l = (FSMType *) FSM;
    FSMState NextState = FSM_l->CurrentState;

    switch (FSM_l->CurrentState) {
    case Stop:
        if (FSM_l->CurrentInput == 2){
            NextState = Start;
        }
        else {
            NextState = Stop;
        }
        break;
    case Start:
        if ((FSM_l->CurrentInput == 0) || (FSM_l->CurrentInput == 2)) {
            NextState = Start;
        } else {
            NextState = Stop;
        }
        break;
    }

    return NextState;
}

void OutputPWM(void *FSM)
{
    FSMType *fsm = (FSMType *)FSM;

    static uint32_t currentNoteIndex = 0;
    static uint32_t nextNoteIndex = 0;
    static uint32_t ticksRemaining = 0;
    static uint8_t hasStarted = 0;
    static FSMState lastState = Stop;


    if (fsm->CurrentState == Stop){
        // if current state == stop, turn off timer for PWM
        stopPWM();
        if (hasStarted){
            LCD_SetCursor(0, 0);
            LCD_OutString("PAUSED           \n\n\n\n ");
        } else {
            LCD_SetCursor(0, 0);
            LCD_OutString("PRESS S1 TO PLAY");
        }
        lastState = fsm->CurrentState;
        return;
    } else if (fsm->CurrentState == Start){
        // only used for screen display
        hasStarted = 1;   
        if (lastState == Stop){
            // print on LCD
            LCD_SetCursor(0, 0);
            LCD_OutString("PLAYING           \n");
            LCD_OutString("Current Note: ");
            LCD_OutString(noteNames[currentNoteIndex]);
        }  
        if (ticksRemaining == 0){
            stopPWM();
            for (int i = 0; i < 10; i++) Delay1ms();

            // if we are at the last note loop back to the first note
            if (nextNoteIndex >= song_length){
                nextNoteIndex = 0;
            } 
            // if we are done with a note, move on to the next
            currentNoteIndex = nextNoteIndex;
            nextNoteIndex++;
            ticksRemaining = noteDurations[currentNoteIndex];
            uint32_t period = PROJECT_CLOCK_FREQUENCY / ((PRESCALE+1)*(DIV+1)*(songNotes[currentNoteIndex]));
            // load values for the new note into the pwm
            TIMA1->COUNTERREGS.LOAD = period;
            
            TimerA1PWMConfig.duty = (period * 4) / 10; //period>>1;
            UpdateDutyCycle(TIMA1, &TimerA1PWMConfig);
            EnableTimer(TIMA1);

            // print on LCD
            LCD_SetCursor(0, 0);
            LCD_OutString("PLAYING           \n");
            LCD_OutString("Current Note: ");
            LCD_OutString(noteNames[currentNoteIndex]);
        } else {
            // if we were in the middle of the note, resume the current note with its ticksRemaining
            uint32_t period = PROJECT_CLOCK_FREQUENCY / ((PRESCALE+1)*(DIV+1)*(songNotes[currentNoteIndex]));
            TIMA1->COUNTERREGS.LOAD = period;
            TimerA1PWMConfig.duty = (period * 4) / 10; //period>>1;
            UpdateDutyCycle(TIMA1, &TimerA1PWMConfig);
            EnableTimer(TIMA1);
            // consume a tick
            ticksRemaining--;
        }
        lastState = fsm->CurrentState;
    }

}

static void Delay1ms(void)
{
    for (volatile uint32_t i = 0; i < 32000; i++) {
        __NOP();
    }
}

void stopPWM(void) 
{
    TIMA1->COUNTERREGS.CTRCTL &= ~GPTIMER_CTTRIGCTL_CTEN_ENABLE;
}
