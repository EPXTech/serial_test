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

// Define buffer size and buffer
#define UART_BUFFER_SIZE 100
#define UART_PERIODIC_MESSAGE_INTERVAL_MS 1000  // Interval in milliseconds

// Buffer to hold received data
uint8_t uartRxBuffer[UART_BUFFER_SIZE];

// Flag to indicate UART data reception
volatile bool uartDataReady = false;

// Flag to indicate periodic UART transmission
volatile bool uartPeriodicTransmit = false;

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

// Timer callback function for periodic UART transmission
void TimerCallback(uintptr_t context)
{
    uartPeriodicTransmit = true;
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
    appData.state = APP_STATE_INIT;

    // Register the UART read callback function
    SERCOM3_USART_ReadCallbackRegister(UART_ReadCallback, (uintptr_t)NULL);

    // Start UART read operation
    SERCOM3_USART_Read(uartRxBuffer, sizeof(uartRxBuffer));

    // Start the timer for periodic UART transmission
    SYS_TIME_HANDLE timerHandle = SYS_TIME_CallbackRegisterMS(TimerCallback, (uintptr_t)NULL, UART_PERIODIC_MESSAGE_INTERVAL_MS, SYS_TIME_PERIODIC);

    if (timerHandle == SYS_TIME_HANDLE_INVALID)
    {
        // Handle timer creation error
        char errorMessage[] = "Failed to create timer.\r\n";
        SERCOM3_USART_Write(errorMessage, sizeof(errorMessage) - 1);
    }
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    switch ( appData.state )
    {
        case APP_STATE_INIT:
        {
            bool appInitialized = true;

            if (appInitialized)
            {
                char message[] = "UART Communication Initialized.\r\n";
                SERCOM3_USART_Write(message, sizeof(message) - 1);

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            // Check if data is received
            if (uartDataReady)
            {
                uartDataReady = false;
                SERCOM3_USART_Write(uartRxBuffer, sizeof(uartRxBuffer));
                SERCOM3_USART_Read(uartRxBuffer, sizeof(uartRxBuffer));
            }

            // Check if it's time to send the periodic message
            if (uartPeriodicTransmit)
            {
                uartPeriodicTransmit = false;
                char periodicMessage[] = "Periodic UART Message.\r\n";
                SERCOM3_USART_Write(periodicMessage, sizeof(periodicMessage) - 1);
            }

            break;
        }

        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
