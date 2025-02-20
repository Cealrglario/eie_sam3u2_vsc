/*!*********************************************************************************************************************
@file user_app1.h                                                                
@brief Header file for user_app1

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_Idle(void);    
static void UserApp1SM_WaitInitializeDHT20(void);
static void UserApp1SM_InitializeDHT20Pins(void);
static void UserApp1SM_Error(void);         



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* Sensor wait times */
#define U32_DHT20_WAIT_MEASUREMENT_MS           (u32)80 /* Amount of time in milliseconds to wait for sensor to finish reading measurement */
#define U32_DHT20_WAIT_INITIALIZATION_MS        (u32)100 /* Amount of time in milliseconds to wait for sensor to initialize internally */

/* Sensor address and commands */
#define U8_DHT20_I2C_ADDRESS                    (u8)0x38 /* DHT20 sensor's I2C address */
#define U8_DHT20_STATUS_CHECK                   (u8)0x71 /* Command to send to DHT20 sensor to initiate status check */
#define U8_DHT20_EXPECTED_STATUS                (u8)0x18 /* Expected reply from DHT20 sensor when performing status check */
#define U8_DHT20_TRIGGER_MEASUREMENT            (u8)0xAC /* Command to send to the DHT20 sensor to request a new measurement */

#endif /* __USER_APP1_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
