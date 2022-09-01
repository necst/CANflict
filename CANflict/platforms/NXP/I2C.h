#pragma once

// The peripheral interface that is being implemented
#include "../peripherals/I2C.h"

// Platform-specific header
#include <board.h>

// ------------------- I2C Interface Implementation ---------------------------

struct CANflict_I2C_t {
  I2C_ID_T id;
  LPC_I2C_T *i2c;
};

#define I2C_CON_FLAGS (I2C_CON_AA | I2C_CON_SI | I2C_CON_STO | I2C_CON_STA)

static void Init_I2C_PinMux(void) {
#if (defined(BOARD_NXP_XPRESSO_11U14) || defined(BOARD_NGX_BLUEBOARD_11U24))
  Chip_SYSCTL_PeriphReset(RESET_I2C0);
  Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 4, IOCON_FUNC1 | IOCON_FASTI2C_EN);
  Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 5, IOCON_FUNC1 | IOCON_FASTI2C_EN);
#elif (defined(BOARD_NXP_XPRESSO_11C24) || defined(BOARD_MCORE48_1125))
  Chip_SYSCTL_PeriphReset(RESET_I2C0);
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_FUNC1 | IOCON_FASTI2C_EN);
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_FUNC1 | IOCON_FASTI2C_EN);
#else
#error "No Pin Muxing defined for I2C operation"
#endif
}

static inline bool CANflict_I2C_init(struct CANflict_I2C_t *I2C) {
  if (I2C == NULL)
    return false;

  Init_I2C_PinMux();
  Chip_I2C_Init(I2C->id);

  // TODO: might be useful to implement also a non-blocking version
  Chip_I2C_SetMasterEventHandler(I2C->id, Chip_I2C_EventHandlerPolling);

  return true;
}

static inline bool CANflict_I2C_deinit(struct CANflict_I2C_t *I2C) {
  if (I2C == NULL)
    return false;

  Chip_I2C_DeInit(I2C->id);
  return true;
}

static inline bool CANflict_I2C_setBaudrate(struct CANflict_I2C_t *I2C,
                                            uint32_t baudrateHz) {
  if (I2C == NULL)
    return false;

  Chip_I2C_SetClockRate(I2C->id, baudrateHz);
  return true;
}

/* Get current state of the I2C peripheral */
static inline int CANflict_I2C_getstate(LPC_I2C_T *I2C) {
  return (int)(I2C->STAT & I2C_STAT_CODE_BITMASK);
}

static inline uint32_t CANflict_I2C_write(struct CANflict_I2C_t *I2C,
                                          uint8_t *bytes, bool *rwBits,
                                          uint8_t len) {
  if (bytes == NULL || rwBits == NULL)
    return 0U;

  LPC_I2C_T *i2c = I2C->i2c;
  I2C_ID_T id = I2C->id;

  /* Reset STA, STO, SI */
  i2c->CONCLR = I2C_CON_SI | I2C_CON_STO | I2C_CON_STA | I2C_CON_AA;

  // For the moment, the implementation for this target is hardcoded to
  // match a CAN baudrate of 200 kHz.
  // TODO: switch to a more general implementation
  unsigned byteIdx;
  for (byteIdx = 0; byteIdx < len; byteIdx++) {
    /* Enter to Master Transmitter mode */
    i2c->CONSET = I2C_CON_I2EN | I2C_CON_STA;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    bool done = false;
    while (!done) {
      if (Chip_I2C_IsStateChanged(id)) {
        uint32_t cclr = I2C_CON_FLAGS;

        switch (CANflict_I2C_getstate(i2c)) {
        case 0x08: /* Start condition on bus */
        case 0x10: /* Repeated start condition */
          I2C->i2c->DAT = (bytes[byteIdx] << 1) | (rwBits[byteIdx]);
          break;

        case 0x00: /* Bus Error */
        case 0x20: /* SLA+W sent NAK received */
        case 0x30: /* DATA sent NAK received */
        case 0x48: /* SLA+R sent NAK received */
          cclr &= ~I2C_CON_STO;
          done = true;
          break;
        }

        /* Set clear control flags */
        i2c->CONSET = cclr ^ I2C_CON_FLAGS;
        i2c->CONCLR = cclr;
      }
    }
    // This time is hardcoded
    // TODO: switch to a more general implementation
    for (int i = 0; i < 46; ++i)
      ;
  }

  I2C->i2c->CONCLR = I2C_CON_FLAGS;
  return byteIdx;
}
