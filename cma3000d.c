
#include <ioCC2540.h>
#include "cma3000d.h"


//***********************************************************************************
// Defines

// Accelerometer connected at (rev0.6/rev1.0):
// P0_6 = DATA_READY/-
// P1_2 = -/CS
// P1_3 = SCK
// P1_4 = MISO
// P1_5 = MOSI
// P1_7 = CS/DATA_READY

#define SCK             P1_3
#define MISO            P1_4
#define MOSI            P1_5

#ifdef REV_1_0
  #define CS              P1_2
#elif (defined REV_0_6)
  #define CS              P1_7
#endif

#define CS_DISABLED     1
#define CS_ENABLED      0


//***********************************************************************************
// Function prototypes
void spiWriteByte(uint8 write);
void spiReadByte(uint8 *read, uint8 write);


/** \brief	Initialize SPI interface and CMA3000-D01 accelerometer
*
* This will initialize the SPI interface and CMA3000-D01 accelerometer
*
*/
void accInit(void)
{
    //*** Setup USART 0 SPI at alternate location 2 ***

    // USART 0 at alternate location 2
    PERCFG |= 0x01;
    // Peripheral function on SCK, MISO and MOSI (P1_3-5)
    P1SEL |= 0x38;
    // Configure CS (P1_7/P1_2) as output
#ifdef REV_1_0
    P1DIR |= 0x04;
#elif (defined REV_0_6)
    P1DIR |= 0x80;
#endif
    CS = CS_DISABLED;

    //*** Setup the SPI interface ***
    // SPI master mode
    U0CSR = 0x00;
    // Negative clock polarity, Phase: data out on CPOL -> CPOL-inv
    //                                 data in on CPOL-inv -> CPOL
    // MSB first
    U0GCR = 0x20;
    // SCK frequency = 480.5kHz (max 500kHz)
    U0GCR |= 0x0D;
    U0BAUD = 0xEC;

    uint8 readValue;
    accWriteReg(CTRL, RANGE_2G | MODE_100HZ_MEAS);
    WAIT_1_3US(80);
    do{
        accReadReg(STATUS, &readValue);
        WAIT_1_3US(80);
    }while(readValue & 0x08);
}

/** \brief	Write one byte to a sensor register
*
* Write one byte to a sensor register
*
* \param[in]       reg
*     Register address
* \param[in]       val
*     Value to write
*/
void accWriteReg(uint8 reg, uint8 val)
{
    CS = CS_ENABLED;
    spiWriteByte(reg|0x02);
    spiWriteByte(val);
    CS = CS_DISABLED;
}


/** \brief	Read one byte from a sensor register
*
* Read one byte from a sensor register
*
* \param[in]       reg
*     Register address
* \param[in]       *pVal
*     Pointer to variable to put read out value
*/
void accReadReg(uint8 reg, uint8 *pVal)
{
    CS = CS_ENABLED;
    WAIT_1_3US(2);
    spiWriteByte(reg);
    spiReadByte(pVal, 0xFF);
    CS = CS_DISABLED;
}

/** \brief	Read x, y and z acceleration data
*
* Read x, y and z acceleration data in one operation.
* NOTE: No sensor access must be made immidiately folloing this operation
* without enabling the last Wait call.
*
* \param[in]       *pXVal
*     Pointer to variable to put read out X acceleration
* \param[in]       *pYVal
*     Pointer to variable to put read out Y acceleration
* \param[in]       *pZVal
*     Pointer to variable to put read out Z acceleration
*/

void accReadAcc(int8 *pXVal, int8 *pYVal, int8 *pZVal)
{
    // X
    accReadReg(DOUTX, (uint8*)pXVal);
    WAIT_1_3US(80);

    // Y
    accReadReg(DOUTY, (uint8*)pYVal);
    WAIT_1_3US(80);

    //Z
    accReadReg(DOUTZ, (uint8*)pZVal);

    //WAIT_1_3US(80);


}


/** \brief	Write one byte to SPI interface
*
* Write one byte to SPI interface
*
* \param[in]       write
*     Value to write
*/
void spiWriteByte(uint8 write)
{
        U0CSR &= ~0x02;                 // Clear TX_BYTE
        U0DBUF = write;
        while (!(U0CSR & 0x02));        // Wait for TX_BYTE to be set
}

/** \brief	Read one byte from SPI interface
*
* Read one byte from SPI interface
*
* \param[in]       read
*     Read out value
* \param[in]       write
*     Value to write
*/
void spiReadByte(uint8 *read, uint8 write)
{
        U0CSR &= ~0x02;                 // Clear TX_BYTE
        U0DBUF = write;
        while (!(U0CSR & 0x02));        // Wait for TX_BYTE to be set
        *read = U0DBUF;
}

