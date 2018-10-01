/* Guards against multiple inclusion */
#ifndef R_INIT_CLOCK_H 
#define R_INIT_CLOCK_H 

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* ---- Please set the main clock and the sub-clock ---- */
#define MAIN_CLOCK_Hz  (12500000L)       /* This sample code uses 12MHz */
#define SUB_CLOCK_Hz   (32768L)          /* This sample code uses 32.768kHz */

/* ---- Please set wait processing for the clock oscillation stabilization ---- */
#define WAIT_TIME_FOR_MAIN_OSCILLATION (11026000L)
#define WAIT_TIME_FOR_SUB_OSCILLATION  (2600000000L)
//#define WAIT_TIME_FOR_SUB_OSCILLATION  (3300000000L) /* 64Pin Package */
#define WAIT_TIME_FOR_PLL_OSCILLATION  (1865000L)
#define WAIT_TIME_FOR_HOCO_OSCILLATION (2000000L)

/* ---- Please choose the sub-clock pattern ---- */
#define PATTERN_A       (1)             /* Sub-clock pattern A */
#define PATTERN_B       (2)             /* Sub-clock pattern B */
#define PATTERN_C       (3)             /* Sub-clock pattern C */
#define PATTERN_D       (4)             /* Sub-clock pattern D */
#define PATTERN_E       (5)             /* Sub-clock pattern E */
#define PATTERN_48      (PATTERN_A)     /* Sub-clock pattern 48Pin Package */

/* Select the sub-clock pattern to use. (Pattern A to E) */
#define SELECT_SUB      (PATTERN_A)     /* This sample code uses pattern A. */

//#define LOW_CL                        /* Drive ability for Low CL Used */

/*******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 *******************************************************************************/
class initClock{
public:
	static void setClock(void);
private:
	static void CGC_oscillation_main(void);
	static void CGC_oscillation_PLL(void);
	static void CGC_oscillation_HOCO(void);
	static void CGC_no_use_subclk(void);
	static void CGC_disable_subclk_RTC_use_mainclk(void);
	static void CGC_subclk_as_sysclk(void);
	static void CGC_subclk_as_RTC(void);
	static void CGC_subclk_as_sysclk_RTC(void);

	/*******************************************************************************
	Private variables and functions
	*******************************************************************************/
	static void disable_subclk(void);
	static void oscillation_subclk(void);
	static void no_use_subclk_as_sysclk(void);
	static void resetting_wtcr_mainclk(void);
	static void resetting_wtcr_subclk(void);
	static void enable_RTC(void);
	static void disable_RTC_mainclk(void);
	static void disable_RTC_subclk(void);
	static void cmt0_wait(unsigned int);
};
#endif  /* R_INIT_CLOCK_H */

/* End of File */

