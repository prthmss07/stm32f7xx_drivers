/*
 * stm32f7xx_dma.h
 *
 *  Created on: 30-Jul-2026
 *      Author: prathameshs
 */

#ifndef STM32F7XX_DMA_H_
#define STM32F7XX_DMA_H_

#ifndef STM32F4XX_DMA_H
#define STM32F4XX_DMA_H

#include "stm32f7xx.h" // For register map definitions (DMA_RegDef_t, DMA_Stream_RegDef_t)

/*
 * DMA Callback Identification Enums
 */
typedef enum {
    DMA_CB_TRANSFER_CPLT   = 0, // Transfer Complete (TC)
    DMA_CB_HALF_TRANSFER  = 1, // Half Transfer (HT)
    DMA_CB_ERROR          = 2  // Transfer / FIFO / DME Error
} DMA_CallbackID_t;

// Callback function pointer type definition
typedef void (*DMA_Callback_t)(struct __DMA_Handle_t *pDMAHandle);

/*
 * DMA User Configuration Structure
 */
typedef struct {
    uint32_t Channel;       // Channel selection (0 to 7)
    uint32_t Direction;     // Peripheral-to-Memory, Memory-to-Peripheral, Memory-to-Memory
    uint32_t PInc;          // Peripheral Increment (Enable/Disable)
    uint32_t MInc;          // Memory Increment (Enable/Disable)
    uint32_t PSize;         // Peripheral Data Width (8-bit, 16-bit, 32-bit)
    uint32_t MSize;         // Memory Data Width (8-bit, 16-bit, 32-bit)
    uint32_t Priority;      // Stream Priority Level (Low, Medium, High, Very High)
    uint32_t Mode;          // Normal Mode vs. Circular Mode
    uint32_t FIFOMode;      // Direct Mode (FIFO Disabled) vs. FIFO Mode
    uint32_t FIFOThreshold; // FIFO Threshold level (1/4, 1/2, 3/4, Full)
} DMA_Config_t;

/*
 * DMA Handle Structure
 */
typedef struct {
    DMA_RegDef_t        *pDMAx;         // Pointer to base register: DMA1 or DMA2
    DMA_Stream_RegDef_t *pStream;       // Pointer to specific stream: DMA2_Stream0
    DMA_Config_t        DMA_Config;     // Configuration parameters
    uint8_t             StreamNum;      // Stream Index Number (0 to 7)

    // Callbacks for asynchronous interrupt handling
    DMA_Callback_t      XferCpltCallback;
    DMA_Callback_t      HalfXferCpltCallback;
    DMA_Callback_t      ErrorCallback;
} DMA_Handle_t;

/**
 * @DMA_Channel_Selection
 */
#define DMA_CHANNEL_0           0U
#define DMA_CHANNEL_1           1U
#define DMA_CHANNEL_2           2U
#define DMA_CHANNEL_3           3U
#define DMA_CHANNEL_4           4U
#define DMA_CHANNEL_5           5U
#define DMA_CHANNEL_6           6U
#define DMA_CHANNEL_7           7U

/**
 * @DMA_Transfer_Direction
 */
#define DMA_DIR_PERIPH_TO_MEM   0U  /* Bit 7:6 = 00 in SxCR */
#define DMA_DIR_MEM_TO_PERIPH   1U  /* Bit 7:6 = 01 in SxCR */
#define DMA_DIR_MEM_TO_MEM      2U  /* Bit 7:6 = 10 in SxCR */

/**
 * @DMA_Data_Width (PSize / MSize)
 */
#define DMA_DATA_SIZE_8BIT      0U  /* Bit 12:11 or 14:13 = 00 */
#define DMA_DATA_SIZE_16BIT     1U  /* Bit 12:11 or 14:13 = 01 */
#define DMA_DATA_SIZE_32BIT     2U  /* Bit 12:11 or 14:13 = 10 */

/**
 * @DMA_Address_Increment
 */
#define DMA_INC_DISABLE         0U
#define DMA_INC_ENABLE          1U

/**
 * @DMA_Priority_Level
 */
#define DMA_PRIORITY_LOW        0U  /* Bit 17:16 = 00 in SxCR */
#define DMA_PRIORITY_MEDIUM     1U  /* Bit 17:16 = 01 in SxCR */
#define DMA_PRIORITY_HIGH       2U  /* Bit 17:16 = 10 in SxCR */
#define DMA_PRIORITY_VERY_HIGH  3U  /* Bit 17:16 = 11 in SxCR */

/**
 * @DMA_Mode
 */
#define DMA_MODE_NORMAL         0U  /* CIRC bit = 0 */
#define DMA_MODE_CIRCULAR       1U  /* CIRC bit = 1 */

/**
 * @DMA_FIFO_Mode
 */
#define DMA_FIFO_DIRECT_DISABLE 0U  /* DMDIS bit = 0 (Direct Mode enabled) */
#define DMA_FIFO_ENABLE         1U  /* DMDIS bit = 1 (FIFO enabled) */

/**
 * @DMA_FIFO_Threshold
 */
#define DMA_FIFO_THRES_1QUARTER 0U  /* FTH bits = 00 */
#define DMA_FIFO_THRES_HALF     1U  /* FTH bits = 01 */
#define DMA_FIFO_THRES_3QUARTER 2U  /* FTH bits = 10 */
#define DMA_FIFO_THRES_FULL     3U  /* FTH bits = 11 */

/* =================================================================================
 * 1. PERIPHERAL CLOCK & INITIALIZATION APIs
 * ================================================================================= */

/**
 * @brief  Enables or disables the peripheral clock for DMA1 or DMA2.
 * @param  pDMAx: Base address of the DMA controller (DMA1 or DMA2).
 * @param  EnOrDis: ENABLE or DISABLE macro.
 */
void DMA_PeriClockControl(DMA_RegDef_t *pDMAx, uint8_t EnorDi);

/**
 * @brief  Initializes a DMA Stream based on the settings in DMA_Handle_t.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 */
void DMA_Init(DMA_Handle_t *pDMAHandle);

/**
 * @brief  Resets a DMA Stream back to its hardware default state.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 */
void DMA_DeInit(DMA_Handle_t *pDMAHandle);


/* =================================================================================
 * 2. DATA TRANSFER EXECUTION APIs
 * ================================================================================= */

/**
 * @brief  Starts a transfer in Polling (Blocking) mode.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 * @param  SrcAddr:    32-bit source address (Peripheral PAR or RAM M0AR).
 * @param  DestAddr:   32-bit destination address (RAM M0AR or Peripheral PAR).
 * @param  Size:       Number of data items (NDTR count) to move.
 */
void DMA_Start(DMA_Handle_t *pDMAHandle, uint32_t SrcAddr, uint32_t DestAddr, uint32_t Size);

/**
 * @brief  Starts a transfer in Interrupt-Driven (Non-Blocking) mode.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 * @param  SrcAddr:    32-bit source address (Peripheral PAR or RAM M0AR).
 * @param  DestAddr:   32-bit destination address (RAM M0AR or Peripheral PAR).
 * @param  Size:       Number of data items (NDTR count) to move.
 */
void DMA_Start_IT(DMA_Handle_t *pDMAHandle, uint32_t SrcAddr, uint32_t DestAddr, uint32_t Size);

/**
 * @brief  Safely disables an active DMA stream and waits for hardware halt.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 */
void DMA_Abort(DMA_Handle_t *pDMAHandle);


/* =================================================================================
 * 3. INTERRUPT & STATUS MANAGEMENT APIs
 * ================================================================================= */

/**
 * @brief  Central Interrupt Service Routine processor for DMA streams.
 *         (Must be called inside DMAx_Streamy_IRQHandler).
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 */
void DMA_IRQHandling(DMA_Handle_t *pDMAHandle);

/**
 * @brief  Checks if a specific status flag (TCIF, HTIF, TEIF, etc.) is active.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 * @param  FlagMask:   Bitmask identifying the target flag.
 * @retval uint8_t:    FLAG_SET or FLAG_RESET.
 */
uint8_t DMA_GetFlagStatus(DMA_Handle_t *pDMAHandle, uint32_t FlagMask);

/**
 * @brief  Clears a specific status flag in LIFCR or HIFCR.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 * @param  FlagMask:   Bitmask identifying the flag to clear.
 */
void DMA_ClearFlag(DMA_Handle_t *pDMAHandle, uint32_t FlagMask);


/* =================================================================================
 * 4. CALLBACK REGISTRATION APIs
 * ================================================================================= */

/**
 * @brief  Registers a client callback function for Transfer Complete, Half-Transfer, or Error.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 * @param  CallbackID: Event type (DMA_CB_TRANSFER_CPLT, DMA_CB_HALF_TRANSFER, DMA_CB_ERROR).
 * @param  pFunc:      Pointer to the user function to attach.
 */
void DMA_RegisterCallback(DMA_Handle_t *pDMAHandle, DMA_CallbackID_t CallbackID, DMA_Callback_t pFunc);

/**
 * @brief  Detaches a client callback function from the DMA Handle.
 * @param  pDMAHandle: Pointer to the DMA Handle structure.
 * @param  CallbackID: Event type to unregister.
 */
void DMA_UnRegisterCallback(DMA_Handle_t *pDMAHandle, DMA_CallbackID_t CallbackID);


#endif

#endif /* STM32F7XX_DMA_H_ */
