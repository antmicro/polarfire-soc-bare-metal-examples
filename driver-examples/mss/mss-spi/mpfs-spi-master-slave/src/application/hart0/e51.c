/*******************************************************************************
 * Copyright 2019-2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on E51
 */

#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"

volatile uint32_t count_sw_ints_h0 = 0U;

/* 
 * Main function for the hart0(e51 processor).
 * Application code running on hart1 is placed here
 */

void e51(void)
{
    uint64_t hartid = read_csr(mhartid);
    volatile uint32_t icount = 0U;

    /* Clear pending software interrupt in case there was any. */

    clear_soft_interrupt();
    set_csr(mie, MIP_MSIP);

    (void)mss_config_clk_rst(MSS_PERIPH_MMUART0, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);

    MSS_UART_init( &g_mss_uart0_lo,
            MSS_UART_115200_BAUD,
            MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

    MSS_UART_polled_tx_string(&g_mss_uart0_lo ,
            (const uint8_t*)"\r\nThis code is running from E51. Please observe UART-1 for application messages\r\n");

#if (IMAGE_LOADED_BY_BOOTLOADER == 0)

    /* Raise software interrupt to wake hart 1 */
    raise_soft_interrupt(1U);

#endif

    __enable_irq();

    /* Clear pending software interrupt in case there was any.
     * Enable only the software interrupt so that the U54_1 core can bring this
     * core out of WFI by raising a software interrupt. */
    while (1U)
    {
        icount++;
        if (0x100000U == icount)
        {
            icount = 0U;
        }
    }
    /* never return */
}

/* hart1 Software interrupt handler */
void Software_h0_IRQHandler(void)
{
    uint64_t hart_id = read_csr(mhartid);
	count_sw_ints_h0++;
}
