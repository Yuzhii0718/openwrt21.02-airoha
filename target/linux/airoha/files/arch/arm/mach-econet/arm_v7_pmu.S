/*
 * ------------------------------------------------------------
 * ARMv7-A/R PMU Code
 *
 *------------------------------------------------------------

 * PRESERVE8

 * AREA  v7PMU,CODE,READONLY
  
 * ARM

 * ------------------------------------------------------------
 * Performance Monitor Block
 *------------------------------------------------------------
 */
#include <linux/version.h>
#include <linux/linkage.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
#include <asm-generic/export.h>
#endif
#define ENDECNTPROC(x)			\
	.globl	__ecnt_##x;		\
	.type 	__ecnt_##x, %function;	\
	.set	__ecnt_##x, x;		\
	.size	__ecnt_##x, . - x;	\
	ENDPROC(x)

  /* 
   * Returns the number of progammable counters
   * unsigned int getPMN(void)
   */
ENTRY(getPMN)
  MRC     p15, 0, r0, c9, c12, 0 // Read PMCR
  MOV     r0, r0, LSR #11        // Shift N field down to bit 0
  AND     r0, r0, #0x1F          // Mask to leave just the 5 N bits
  BX      lr
ENDECNTPROC(getPMN)
  
/*
  ; Sets the event for a programmable counter to record
  ; void pmnConfig(unsigned counter, unsigned int event, unsigned filter)
  ; counter = r0 = Which counter to program  (e.g. 0 for PMN0, 1 for PMN1)
  ; event   = r1 = The event code
  ; filter  = r2 = For PMUv2, the filter bits
*/
ENTRY(pmnConfig)
  MCR     p15, 0, r0, c9, c12, 5 // Write PMNXSEL Register
  ISB
  AND     r1, r1, #0xFF          // Mask to leave 8-bit event code
  AND     r2, r2, #0xF8000000    // Mask to leave the 5-bit filter
  ORR     r1, r1, r2             // Combine event code with event filters
  MCR     p15, 0, r1, c9, c13, 1 // Write EVTSELx Register
  ISB
  BX      lr
ENDECNTPROC(pmnConfig)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(pmnConfig)
#endif
/*
  ; Enables/disables the divider (1/64) on CCNT
  ; void ccntDivider(int divider)
  ; divider = r0 = If 0 disable divider, else enable divider
*/
ENTRY(ccntDivider)
  MRC     p15, 0, r1, c9, c12, 0  // Read PMCR

  CMP     r0, #0x0                // IF (r0 == 0)
  BICEQ   r1, r1, #0x08           // THEN: Clear the D bit (disables the divisor)
  ORRNE   r1, r1, #0x08           // ELSE: Set the D bit (enables the divisor)

  MCR     p15, 0, r1, c9, c12, 0  // Write PMCR
  ISB
  BX      lr
ENDECNTPROC(ccntDivider)

  /* ---------------------------------------------------------------
  ; Enable/Disable
  ; ---------------------------------------------------------------
  */

/*
  ; Global PMU enable
  ; void enablePMU(void)
 */
ENTRY(enablePMU)
  MRC     p15, 0, r0, c9, c12, 0  // Read PMCR
  ORR     r0, r0, #0x01           // Set E bit
  MCR     p15, 0, r0, c9, c12, 0  // Write PMCR
  ISB
  BX      lr
ENDECNTPROC(enablePMU)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(enablePMU)
#endif
/*
  ; Global PMU disable
  ; void disablePMU(void)
 */
ENTRY(disablePMU)
  MRC     p15, 0, r0, c9, c12, 0  // Read PMCR
  BIC     r0, r0, #0x01           // Clear E bit
  MCR     p15, 0, r0, c9, c12, 0  // Write PMCR
  ISB
  BX      lr
ENDECNTPROC(disablePMU)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(disablePMU)
#endif
/*
  ; Enable the CCNT
  ; void enableCCNT(void)
*/
ENTRY(enableCCNT)
  MOV     r0, #0x80000000         // Set C bit
  MCR     p15, 0, r0, c9, c12, 1  // Write PMINTENSET Register
  ISB
  BX      lr
ENDECNTPROC(enableCCNT)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(enableCCNT)
#endif
/*
  ; Disable the CCNT
  ; void disableCCNT(void)
*/
ENTRY(disableCCNT)
  MOV     r0, #0x80000000
  MCR     p15, 0, r0, c9, c12, 2  // Write PMINTENCLR Register
  ISB
  BX      lr
ENDECNTPROC(disableCCNT)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(disableCCNT)
#endif
/*
  ; Enable PMN{n}
  ; void enablePMN(unsigned int counter)
  ; counter = r0 = The counter to enable (e.g. 0 for PMN0, 1 for PMN1)
*/
ENTRY(enablePMN)
  MOV     r1, #0x1                // Use arg (r0) to set which counter to disable
  MOV     r1, r1, LSL r0

  MCR     p15, 0, r1, c9, c12, 1  // Write PMINTENSET Register
  ISB
  BX      lr
ENDECNTPROC(enablePMN)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(enablePMN)
#endif

/*
  ; Disable PMN{n}
  ; void disablePMN(unsigned int counter)
  ; counter = r0 = The counter to enable (e.g. 0 for PMN0, 1 for PMN1)
*/
ENTRY(disablePMN)
  MOV     r1, #0x1                // Use arg (r0) to set which counter to disable
  MOV     r1, r1, LSL r0

  MCR     p15, 0, r1, c9, c12, 2  // Write PMINTENCLR Register
  ISB
  BX      lr
ENDECNTPROC(disablePMN)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(disablePMN)
#endif

/*
  ; Enables User mode access to the PMU (must be called in a privileged mode)
  ; void enablePMUUserAccess(void)
ENTRY(enablePMUUserAccess)
  MRC     p15, 0, r0, c9, c14, 0  // Read PMUSERENR Register
  ORR     r0, r0, #0x01           // Set EN bit (bit 0)
  MCR     p15, 0, r0, c9, c14, 0  // Write PMUSERENR Register
  ISB
  BX      lr
ENDECNTPROC(enablePMUUserAccess)


/*
  ; Disables User mode access to the PMU (must be called in a priviledged mode)
  ; void disablePMUUserAccess(void)
*/
ENTRY(disablePMUUserAccess)
  MRC     p15, 0, r0, c9, c14, 0  // Read PMUSERENR Register
  BIC     r0, r0, #0x01           // Clear EN bit (bit 0)
  MCR     p15, 0, r0, c9, c14, 0  // Write PMUSERENR Register
  ISB
  BX      lr
ENDECNTPROC(disablePMUUserAccess)

/*
  ; ---------------------------------------------------------------
  ; Counter read registers
  ; ---------------------------------------------------------------

/*
  ; Returns the value of CCNT
  ; unsigned int readCCNT(void)
*/
ENTRY(readCCNT)
  MRC     p15, 0, r0, c9, c13, 0 // Read PMCCNTR
  BX      lr
ENDECNTPROC(readCCNT)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(readCCNT)
#endif
/*
  ; Returns the value of PMN{n}
  ; unsigned int readPMN(unsigned int counter)
  ; counter = r0 =  The counter to read (e.g. 0 for PMN0, 1 for PMN1)
*/
ENTRY(readPMN)
  AND     r0, r0, #0x1F          // Mask to leave only bits 4:0
  MCR     p15, 0, r0, c9, c12, 5 // Write PMSELR
  MRC     p15, 0, r0, c9, c13, 2 // Read current PMNx Register
  BX      lr
ENDECNTPROC(readPMN)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(readPMN)
#endif  
/*  
  ; ---------------------------------------------------------------
  ; Software Increment
  ; ---------------------------------------------------------------

/*
	; Writes to software increment register
	; void pmuSoftwareIncrement(unsigned int counter)
	; counter = r0 =  The counter to increment (e.g. 0 for PMN0, 1 for PMN1)
*/
ENTRY(pmuSoftwareIncrement)
  MOV     r1, #0x01
  MOV			r1, r1, LSL r0
  MCR     p15, 0, r1, c9, c12, 4 // Write PMSWINC
  BX      lr
ENDECNTPROC(pmuSoftwareIncrement)

/*
  ; ---------------------------------------------------------------
  ; Overflow & Interrupt Generation
  ; ---------------------------------------------------------------
*/
/*
  ; Returns the value of the overflow flags
  ; unsigned int readFlags(void)
*/
ENTRY(readFlags)
  MRC     p15, 0, r0, c9, c12, 3 // Read PMOVSR
  BX      lr
ENDECNTPROC(readFlags)


/*
  ; Writes the overflow flags
  ; void writeFlags(unsigned int flags)
*/
ENTRY(writeFlags)
  MCR     p15, 0, r0, c9, c12, 3 // Write PMOVSR
  ISB
  BX      lr
ENDECNTPROC(writeFlags)
  

/*
  ; Enables interrupt generation on overflow of the cycle counter
  ; void enableCCNTIrq(void)
*/
ENTRY(enableCCNTIrq)
  MOV     r0, #0x80000000
  MCR     p15, 0, r0, c9, c14, 1  // Write PMINTENSET
  ISB
  BX      lr
ENDECNTPROC(enableCCNTIrq)


/*
  ; Disables interrupt generation on overflow of the cycle counter
  ; void disableCCNTIrq(void)
*/
ENTRY(disableCCNTIrq)
  MOV     r0, #0x80000000
  MCR     p15, 0, r0, c9, c14, 2   // Write PMINTENCLR
  ISB
  BX      lr
ENDECNTPROC(disableCCNTIrq)


/*
  ; Enables interrupt generation on overflow of PMN{x}
  ; void enablePMNIrq(unsigned int counter)
  ; counter = r0 =  The counter to enable the interrupt for (e.g. 0 for PMN0, 1 for PMN1)
*/
ENTRY(enablePMNIrq)
  MOV     r1, #0x1                 // Use arg (r0) to set which counter to disable
  MOV     r0, r1, LSL r0
  MCR     p15, 0, r0, c9, c14, 1   // Write PMINTENCLR
  ISB
  BX      lr
ENDECNTPROC(enablePMNIrq)


/*
  ; Disables interrupt generation on overflow of PMN{x}
  ; void disablePMNIrq(unsigned int counter)
  ; counter = r0 =  The counter to disable the interrupt for (e.g. 0 for PMN0, 1 for PMN1)
*/
ENTRY(disablePMNIrq)
  MOV     r1, #0x1                // Use arg (r0) to set which counter to disable
  MOV     r0, r1, LSL r0
  MCR     p15, 0, r0, c9, c14, 2  // Write PMINTENCLR
  ISB
  BX      lr
ENDECNTPROC(disablePMNIrq)

/*
  ; ---------------------------------------------------------------
  ; Reset Functions
  ; ---------------------------------------------------------------
*/
/*
  ; Resets the programmable counters
  ; void resetPMN(void)
*/
ENTRY(resetPMN)
  MRC     p15, 0, r0, c9, c12, 0  // Read PMCR
  ORR     r0, r0, #0x02           // Set P bit (Event Counter Reset)
  MCR     p15, 0, r0, c9, c12, 0  // Write PMCR
  ISB
  BX      lr
ENDECNTPROC(resetPMN)
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
EXPORT_SYMBOL(resetPMN)
#endif

/*
  ; Resets the CCNT
  ; void resetCCNT(void)
*/
ENTRY(resetCCNT)
  MRC     p15, 0, r0, c9, c12, 0  // Read PMCR
  ORR     r0, r0, #0x04           // Set C bit (Event Counter Reset)
  MCR     p15, 0, r0, c9, c12, 0  // Write PMCR
  ISB
  BX      lr
ENDECNTPROC(resetCCNT)

