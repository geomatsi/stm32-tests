#include "init.h"

/* */

/* FreeRTOS needs this variable */
uint32_t SystemCoreClock;

/* */

static void rcc_clock_setup_in_hsi_out_84mhz(void)
{
	/* Enable power control block. */
	rcc_periph_clock_enable(RCC_PWR);

	/* Disable voltage scaling. */
	pwr_set_vos_scale(SCALE2);

	/* Enable internal high-speed oscillator. */
	rcc_osc_on(HSI);
	rcc_wait_for_osc_ready(HSI);

	/* Select HSI as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_HSI);

	/* Set prescalers for AHB, ADC, ABP1, ABP2. */
	rcc_set_hpre(RCC_CFGR_HPRE_DIV_NONE);
	rcc_set_ppre1(RCC_CFGR_PPRE_DIV_2);
	rcc_set_ppre2(RCC_CFGR_PPRE_DIV_NONE);

	/* */
	rcc_set_main_pll_hsi(16, 336, 4, 7);

	/* Enable PLL oscillator and wait for it to stabilize. */
	rcc_osc_on(PLL);
	rcc_wait_for_osc_ready(PLL);

	/* Configure flash settings. */
	flash_set_ws(FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_2WS);

	/* Select PLL as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_PLL);

	/* Wait for PLL clock to be selected. */
	rcc_wait_for_sysclk_status(PLL);

	/* Set the peripheral clock frequencies used */
	rcc_ahb_frequency  = 84000000;
	rcc_apb1_frequency = 42000000;
	rcc_apb2_frequency = 84000000;
}

/*
 *	Connectivity between nRF24L01 on Wireless Gateway Shield v1.0 and
 *  stm32f401re-nucleo:
 *		MOSI <-> PA7
 *		MISO <-> PA6
 *		SCK  <-> PA5
 *		CSN  <-> PA9
 *		CE   <-> PA8
 *		IRQ  <-> PA10
 *		VCC  <-> 3V
 *		GND  <-> GND
 */

void hw_init(void)
{
	/* TODO: sync SystemCoreClock with rcc_clock_setup_xxx settings */
	rcc_clock_setup_in_hsi_out_84mhz();
	SystemCoreClock = 84000000;

	/*
	 * clocks
	 *
	 */

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_SPI1);
	rcc_periph_clock_enable(RCC_USART2);
	rcc_periph_clock_enable(RCC_SYSCFG);

	/*
	 * led on Wireless Gateway Shield v1.0
	 * NB: nucleo led PA5 is in use for SPI1 SCK
	 *
	 */

	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
	gpio_clear(GPIOB, GPIO10);

	/*
	 * pins for nRF24L01
	 *
	 */

	/* disable RFM69HW on Wireless Shield v1.0 which is on the same spi */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
	gpio_set(GPIOC, GPIO7);

	/* configure spi1 pins */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO6 | GPIO7);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO5 | GPIO6 | GPIO7);
	gpio_set_af(GPIOA, GPIO_AF5, GPIO5 | GPIO6 | GPIO7);

	/* configure nRF24L01 pins on Wireless Shield v1.0: CE, CSN, IRQ */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO10);

	/* start with spi communication disabled */
	gpio_set(GPIOA, GPIO9);
	gpio_clear(GPIOA, GPIO8);

	/* spi1 */

	spi_set_master_mode(SPI1);
	spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_8);
	spi_set_clock_polarity_0(SPI1);
	spi_set_clock_phase_0(SPI1);
	spi_set_full_duplex_mode(SPI1);
	spi_set_unidirectional_mode(SPI1);
	spi_enable_software_slave_management(SPI1);
	spi_send_msb_first(SPI1);
	spi_set_dff_8bit(SPI1);
	spi_set_nss_high(SPI1);
	SPI_I2SCFGR(SPI1) &= ~SPI_I2SCFGR_I2SMOD;
	spi_enable(SPI1);

	/* uart2 port: RX = PA3 TX = PA2 */

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2| GPIO3);

	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	usart_enable(USART2);

	/* user button */

	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO13);

	/* configure EXTI13 and EXTI15_10_isr to handle user button */

	exti_select_source(EXTI13, GPIOC);
	exti_set_trigger(EXTI13, EXTI_TRIGGER_FALLING);
	exti_enable_request(EXTI13);

	/* configure EXTI10 and EXTI15_10_isr to handle user button */

	exti_select_source(EXTI10, GPIOA);
	exti_set_trigger(EXTI10, EXTI_TRIGGER_FALLING);
	exti_enable_request(EXTI10);

	/* ATTENTION
	 * It is essential that interrupt handlers that make use of the FreeRTOS API
	 * have a logical priority equal to or below that set by
	 * the configMAX_SYSCALL_INTERRUPT_PRIORITY
	 *
	 * NOTE
	 * In ARM Cortex-M cores, numerically low priority values are used
	 * to specify logically high interrupt priorities
	 *
	 * NOTE
	 * Here we use minimal logical priority for user button interrupt
	 */

	nvic_set_priority(NVIC_EXTI15_10_IRQ, configKERNEL_INTERRUPT_PRIORITY);
	nvic_enable_irq(NVIC_EXTI15_10_IRQ);

	/*
		http://www.freertos.org/RTOS-Cortex-M3-M4.html

		Preempt priority and subpriority
		If you are using an STM32 then ensure that all the priority bits are assigned to
		be preempt priority bits by the following function before FreeRTOS is started:
		  - NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4) if you are using CMSIS
		  - scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP16_NOSUB) if you are using libopencm3
	*/

	scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP16_NOSUB);

	/* DON'T PUT ANYTHING HERE AFTER NVIC PRIO GROUP ARE PROPERLY CONFIGURED FOR FREERTOS */
}


