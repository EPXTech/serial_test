/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application. It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware. However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called. That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define UART_BUFFER_SIZE 100

// Buffer to hold received data
uint8_t uartRxBuffer[UART_BUFFER_SIZE];

// Flag to indicate UART data reception
volatile bool uartDataReady = false;

/* Application Data
   Holds application data
   This structure should be initialized by the APP_Initialize function.
*/
APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

// UART read callback function
void UART_ReadCallback(uintptr_t context)
{
    uartDataReady = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    /* Register the UART read callback function */
    SERCOM3_USART_ReadCallbackRegister(UART_ReadCallback, (uintptr_t)NULL);

    /* Start UART read operation */
    SERCOM3_USART_Read(uartRxBuffer, sizeof(uartRxBuffer));
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;

            if (appInitialized)
            {
                // Send a welcome message via UART
                char message[] = "UART Communication Initialized.\r\n";
                SERCOM3_USART_Write(message, sizeof(message) - 1);

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            // Check if UART data has been received
            if (uartDataReady)
            {
                uartDataReady = false;

                // Echo received data back
                SERCOM3_USART_Write(uartRxBuffer, sizeof(uartRxBuffer));

                // Reinitiate UART read
                SERCOM3_USART_Read(uartRxBuffer, sizeof(uartRxBuffer));
            }
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
