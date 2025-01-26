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
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
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

  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_UNCONFIGURED)
  {
    sChannelInfo.AntChannel = U8_ANT_CHANNEL_PERIOD_HI_USERAPP;
    sChannelInfo.AntChannelType = CHANNEL_TYPE_MASTER;
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
    
    AntAssignChannel(&sChannelInfo);
  }  

  /* Initialize the LCD message */

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_WaitAntReady;
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
/* Wait for ANT channel to be configured */
static void UserApp1SM_WaitAntReady(void)
{

  // Ideally we set a wait timeout so that we don't wait forever if it comes to that
  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    if(AntOpenChannelNumber(U8_ANT_CHANNEL_USERAPP))
    {
      UserApp1_pfStateMachine = UserApp1SM_WaitChannelOpen;
    }
    else
    {
      UserApp1_pfStateMachine = UserApp1SM_Error;
    }

  } /* end if(AntOpenChannelNumber)... */
} /* end UserApp1SM_WaitAntReady() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Hold here until ANT confirms the channel is open */
static void UserApp1SM_WaitChannelOpen(void)
{
  // Same thing about adding a wait timeout here, but let's get the code working first
  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    UserApp1_pfStateMachine = UserApp1SM_ChannelOpen;
  }

} /* end UserApp1SM_WaitChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Process messages while channel is open */
static void UserApp1SM_ChannelOpen(void)
{
  // static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  // static PixelAddressType sStringLocation;
  // u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";

  // extern PixelBlockType G_sLcdClearLine7;  /* From lcd-NHD-C12864LZ.c */

  // if( AntReadAppMessageBuffer() )
  // {
  //   if(G_eAntApiCurrentMessageClass == ANT_DATA)
  //   {
  //     /* We got some data! Convert it to displayable ASCII characters*/   
  //     for(u8 i = 0; i < ANT_DATA_BYTES; i++)
  //     {
  //       au8DataContent[2 *i] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] / 16);
  //       au8DataContent[(2 * i) + 1] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] % 16);
  //     }

  //     /* Write the board string in the middle of last row */
  //     sStringLocation.u16PixelColumnAddress =  
  //       U16_LCD_CENTER_COLUMN - ( strlen((char const*)au8DataContent) * (U8_LCD_SMALL_FONT_COLUMNS + U8_LCD_SMALL_FONT_SPACE) / 2 );
  //     sStringLocation.u16PixelRowAddress = U8_LCD_SMALL_FONT_LINE7;
  //     LcdClearPixels(&G_sLcdClearLine7);
  //     LcdLoadString(au8DataContent, LCD_FONT_SMALL, &sStringLocation);
  //   }
  //   else if (G_eAntApiCurrentMessageClass == ANT_TICK)
  //   {
  //     /* Check the buttons and update corresponding slot in au8TestMessage */
  //     au8TestMessage[0] = 0x00;
  //     au8TestMessage[1] = 0x00;
  //     au8TestMessage[2] = 0x00;
  //     au8TestMessage[3] = 0x00;

  //     if( IsButtonPressed(BUTTON0) )
  //     {
  //       au8TestMessage[0] = 0xff;
  //     }
      
  //     if( IsButtonPressed(BUTTON1) )
  //     {
  //       au8TestMessage[1] = 0xff;
  //     }

  //     /* A channel period has gone by: typically this is when new
  //     data should be queued to send */
  //     au8TestMessage[7]++;
  //     if(au8TestMessage[7] == 0)
  //     {
  //       au8TestMessage[6]++;
  //       if(au8TestMessage[6] == 0)
  //       {
  //         au8TestMessage[5]++;
  //       }
  //     }
  //     AntQueueBroadcastMessage(U8_ANT_CHANNEL_USERAPP, au8TestMessage);
  //   } /* end ANT_TICK */
  // } /* end AntReadAppMessageBuffer() */

  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0}; // BUTTONS 0-4 STATUS, CONSTANT, 3 BYTE COUNTER (256^2 + 256^1 + 256^0)
  static PixelAddressType sStringLocation;
  u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  extern PixelBlockType G_sLcdClearLine7; /* This has type "extern" since we are obtaining the ALREADY EXISTING PixelBlockType
                                          that handles clearing Line 7 from lcd-NHD-C12864LZ.c (extern allows us to use objects in other files,
                                          as long as that object in the other file also has the type "extern") */
                                                                                                 
  if (AntReadAppMessageBuffer()) { // Simply reading the message like this clears the message buffer

      if (G_eAntApiCurrentMessageClass == ANT_DATA) {
        // We reach this point if we (the master) receive data from the slave (our laptop)

        // Parses each of the bytes sent by Ant so we can display it in ASCII on the LCD
        for (u8 i = 0; i < ANT_DATA_BYTES; i++) {
          // Each byte is given two spaces on the LCD (2 * i, then 2 * i + 1). If confused, think of the indices when i = 0
          au8DataContent[2 * i] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] / 16); 
          au8DataContent[(2 * i) + 1] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] % 16);
        }

        sStringLocation.u16PixelColumnAddress =
        U16_LCD_CENTER_COLUMN - ( strlen((char const*)au8DataContent) * (U8_LCD_SMALL_FONT_COLUMNS + U8_LCD_SMALL_FONT_SPACE) / 2); 
        /* The line above obtains the center pixel in the LCD even if the size of the message changes. It does this by
        taking the length of the string in code, maps it to how many pixels that string would take up in "small font" on
        the LCD, gets half that length, and then subtracts the center column by that amount so that half the string will
        be printed before the centerpoint, and half will be printed after, thus printing the string in the middle . */
        sStringLocation.u16PixelRowAddress = U8_LCD_SMALL_FONT_LINE7;
        LcdClearPixels(&G_sLcdClearLine7);
        LcdLoadString(au8DataContent, LCD_FONT_SMALL, &sStringLocation);
      }

      else if (G_eAntApiCurrentMessageClass == ANT_TICK) {
        // An ANT_TICK is sent by Ant whenever a channel period has passed (250ms)
        // This is when new data is queued to send (thus is also when the message counter should be incremented)

        au8TestMessage[0] = 0x00;
        au8TestMessage[1] = 0x00;
        au8TestMessage[2] = 0x00;
        au8TestMessage[3] = 0x00;

        if (IsButtonPressed(BUTTON0)) {
          au8TestMessage[0] = 0xff;
        }

        if (IsButtonPressed(BUTTON1)) {
          au8TestMessage[1] = 0xff;
        }

        // Just a simple 3 byte counter incrementer
        au8TestMessage[7]++;
        if (au8TestMessage[7] == 0) {
          au8TestMessage[6]++;
          if (au8TestMessage[6] == 0) {
            au8TestMessage[5]++;
          }
        }

        AntQueueBroadcastMessage(U8_ANT_CHANNEL_USERAPP, au8TestMessage); // Broadcast the message au8TestMessage to channel U8_ANT_CHANNEL_USERAPP

      }
  }

} /* end UserApp1SM_ChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
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
