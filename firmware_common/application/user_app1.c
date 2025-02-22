/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
static u32 UserApp1_U32Counter = 0;                       /* Counter used to wait for DHT20 wait tasks */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  /* If good initialization, wait for sensor to initialize */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_WaitInitializeDHT20;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }
} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait 100ms to ensure the DHT20 sensor is fully initialized */
static void UserApp1SM_WaitInitializeDHT20(void) {
  UserApp1_U32Counter++;

  if (UserApp1_U32Counter == U32_DHT20_WAIT_INITIALIZATION_MS) {
    UserApp1_U32Counter = 0;
    UserApp1_pfStateMachine = UserApp1SM_InitializeDHT20Pins;
  }
}

static void UserApp1SM_InitializeDHT20Pins(void) {
  u32 UserApp1_u32ReadByte = 0;
  u8 UserApp1_DHT20CheckStatus = U8_DHT20_STATUS_CHECK;
  ErrorStatusType eErrorStatus = SUCCESS;
  eErrorStatus += BladeRequestPin(BLADE_PIN8, PERIPHERAL);
  eErrorStatus += BladeRequestPin(BLADE_PIN9, PERIPHERAL);

  if (eErrorStatus)
  {
    DebugPrintf("DHT20 Blade pin resources not available\n\r");
  } else
  {
    DebugPrintf("DHT20 Blade pin resources allocated\n\r");
  }

  if (eErrorStatus == SUCCESS) {
    /* Check if sensor is working as expected and is ready to measure */
    UserApp1_u32ReadByte = TwiWriteData(U8_DHT20_I2C_ADDRESS, 1, &UserApp1_DHT20CheckStatus, TWI_NO_STOP);
    DebugPrintf("Measured byte: ");
    DebugPrintNumber(UserApp1_u32ReadByte);
    DebugLineFeed();
    if(UserApp1_u32ReadByte == U8_DHT20_EXPECTED_STATUS) {
      /* Report ID returned */
      DebugPrintf("DHT20 returned expected ID: ");
      DebugPrintNumber(UserApp1_u32ReadByte);
      DebugLineFeed();
    }
    else if (UserApp1_u32ReadByte == U8_DHT20_EXPECTED_STATUS) {
      DebugPrintf("DHT20 returned unexpected ID: ");
      DebugPrintNumber(UserApp1_u32ReadByte);
      DebugLineFeed();
      eErrorStatus = ERROR;
    } 
    else {
      DebugPrintf("Unknown error occured.");
      DebugLineFeed();
      eErrorStatus = ERROR;
    }
  }
  
  /* If sensor check succeeded, proceed */
  if (eErrorStatus == SUCCESS) {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else {
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

}

/* What does this state do? */
static void UserApp1SM_Idle(void)
{
     
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
