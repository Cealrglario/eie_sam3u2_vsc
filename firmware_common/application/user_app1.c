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

extern u32 G_u32AntApiCurrentMessageTimeStamp;                            // From ant_api.c
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;            // From ant_api.c
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  // From ant_api.c
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;                // From ant_api.c


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;                 /*!< @brief The state machine function pointer */
static u32 UserApp1_u32DataMsgCount = 0;                    // ANT_DATA packet counter
static u32 UserApp1_u32TickMsgCount = 0;                    // ANT_TICK packet counter
static u8 UserApp1_u8LastState = 0xff;                      // Records the last state of the event code index of TICK message
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
  AntAssignChannelInfoType sChannelInfo;
  PixelAddressType sStringLocation;
  u8 au8WelcomeMessage[] = "ANT Slave Demo";
  extern PixelBlockType G_sLcdClearLine7;

  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_UNCONFIGURED)
  {
    sChannelInfo.AntChannel = U8_ANT_CHANNEL_PERIOD_HI_USERAPP;
    sChannelInfo.AntChannelType = CHANNEL_TYPE_SLAVE;
    sChannelInfo.AntChannelPeriodHi = U8_ANT_CHANNEL_PERIOD_HI_USERAPP;
    sChannelInfo.AntChannelPeriodLo = U8_ANT_CHANNEL_PERIOD_LO_USERAPP;
    
    sChannelInfo.AntDeviceIdHi = U8_ANT_DEVICE_HI_USERAPP;
    sChannelInfo.AntDeviceIdLo = U8_ANT_DEVICE_LO_USERAPP;
    sChannelInfo.AntDeviceType = U8_ANT_DEVICE_TYPE_USERAPP;
    sChannelInfo.AntTransmissionType = U8_ANT_TRANSMISSION_TYPE_USERAPP;
    
    sChannelInfo.AntFrequency = U8_ANT_FREQUENCY_USERAPP;
    sChannelInfo.AntTxPower = U8_ANT_TX_POWER_USERAPP;
    
    sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
    for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
    {
      sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    }
  }  

  /* Initialize the LCD message */
  LedOn(RED0);

  // Write the LCD message in the middle of the last row
  sStringLocation.u16PixelColumnAddress =
    U16_LCD_CENTER_COLUMN - (strlen((char const*)au8WelcomeMessage) * (U8_LCD_SMALL_FONT_COLUMNS + U8_LCD_SMALL_FONT_SPACE) / 2);
  sStringLocation.u16PixelRowAddress = U8_LCD_SMALL_FONT_LINE7;
  LcdClearPixels(&G_sLcdClearLine7);
  LcdLoadString(au8WelcomeMessage, LCD_FONT_SMALL, &sStringLocation);

  /* If good initialization, set state to Idle */
  if(AntAssignChannel(&sChannelInfo))
  {
    UserApp1_pfStateMachine = UserApp1SM_WaitAntReady;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    LedBlink(RED0, LED_4HZ);
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
/* Wait for ANT channel to be configured */
static void UserApp1SM_WaitAntReady(void)
{
  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    LedOn(GREEN0);
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
     /* end if(AntOpenChannelNumber)... */
} /* end UserApp1SM_WaitAntReady() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Hold here until ANT confirms the channel is open */
static void UserApp1SM_WaitChannelOpen(void) // GREEN0 is blinking at 2hz here
{
  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    LedOn(GREEN0);
    UserApp1_pfStateMachine = UserApp1SM_ChannelOpen;
  }

} /* end UserApp1SM_WaitChannelOpen() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Hold here until ANT confirms the channel is closed */
static void UserApp1SM_WaitChannelClose(void) // GREEN0 is blinking at 2hz here
{
  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    LedOn(GREEN0);
    LedOn(RED0);
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }

} /* end UserApp1SM_WaitChannelClose() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Process messages while channel is open */
static void UserApp1SM_ChannelOpen(void)
{
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  static PixelAddressType sStringLocation;
  u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  UserApp1_u8LastState = 0xff;
  static u8 au8TickMessage[] = "EVENT x\n\r"; // We'll replace "x" with the current event code

  extern PixelBlockType G_sLcdClearLine7;  /* From lcd-NHD-C12864LZ.c */

  if(WasButtonPressed(BUTTON0)) { // Button 0 will close the channel if it's already open
    ButtonAcknowledge(BUTTON0);

    AntCloseChannelNumber(U8_ANT_CHANNEL_USERAPP);

    LedBlink(GREEN0, LED_2HZ);
    UserApp1_pfStateMachine = UserApp1SM_WaitChannelClose;
  }

  // In case the slave channel closes on its own
  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) != ANT_OPEN) { //
    UserApp1_u8LastState = 0xff;
    DebugPrintf("Last connected Channel ID: ");
    DebugPrintf(U8_ANT_DEVICE_HI_USERAPP);
    DebugPrintf(U8_ANT_DEVICE_LO_USERAPP);
    DebugLineFeed();
    LedOff(BLUE0);
    LedOff(GREEN0);
    LedBlink(GREEN0, LED_2HZ);

    UserApp1_pfStateMachine = UserApp1SM_WaitChannelClose;
  }

  if( AntReadAppMessageBuffer() )
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      LedOff(GREEN0);
      LedOn(BLUE0);
      UserApp1_u32DataMsgCount++;
      /* We got some data! Convert it to displayable ASCII characters*/   
      for(u8 i = 0; i < ANT_DATA_BYTES; i++)
      {
        au8DataContent[2 *i] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] / 16);
        au8DataContent[(2 * i) + 1] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] % 16);
      }

      /* Write the board string in the middle of last row */
      sStringLocation.u16PixelColumnAddress =  
        U16_LCD_CENTER_COLUMN - ( strlen((char const*)au8DataContent) * (U8_LCD_SMALL_FONT_COLUMNS + U8_LCD_SMALL_FONT_SPACE) / 2 );
      sStringLocation.u16PixelRowAddress = U8_LCD_SMALL_FONT_LINE7;
      LcdClearPixels(&G_sLcdClearLine7);
      LcdLoadString(au8DataContent, LCD_FONT_SMALL, &sStringLocation);
    }
    else if (G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      UserApp1_u32TickMsgCount++;

      // If the current known state has changed via the event code in an ANT_TICK message
      if (UserApp1_u8LastState != G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX]) {
        UserApp1_u8LastState = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
        au8TickMessage[6] = HexToASCIICharUpper(UserApp1_u8LastState); // Update the event code in the debug terminal
        DebugPrintf(au8TickMessage); // Send a debug message to the debug terminal
        DebugLineFeed();

        switch (UserApp1_u8LastState) { // Reacting to different ANT event codes (refer to antdefines.h for meanings of codes)
          case RESPONSE_NO_ERROR: {
            break; // Don't worry about this for now
          }

          case EVENT_RX_FAIL: {
            LedOff(GREEN0);
            LedBlink(BLUE0, LED_4HZ);
            break;
          }

          case EVENT_RX_FAIL_GO_TO_SEARCH: {
            LedOff(BLUE0);
            LedOn(GREEN0);
            break;
          }

          case EVENT_RX_SEARCH_TIMEOUT: {
            DebugPrintf("Search timeout\r\n");
            break;
          }

          default: {
            DebugPrintf("Unknown event\r\n");
            break;
          }
        }
      }
    } /* end ANT_TICK */
  } /* end AntReadAppMessageBuffer() */

} /* end UserApp1SM_ChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp1SM_Idle(void) // GREEN0 and RED0 LED is on here (yellow)
{
  if(WasButtonPressed(BUTTON0)) {
    ButtonAcknowledge(BUTTON0);

    AntOpenChannelNumber(U8_ANT_CHANNEL_USERAPP);

    LedOff(RED0);
    LedBlink(GREEN0, LED_2HZ);

    UserApp1_pfStateMachine = UserApp1SM_WaitChannelOpen;
  }
     
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
 
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
