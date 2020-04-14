#include "i2c_slave_init.h"

void i2c2__slave_init(uint8_t slave_address_to_respond_to) {
  // set adress register for slave mode
  // LPC_I2C2->ADR0 = (slave_address_to_respond_to << 1);
  // LPC_I2C2->ADR1 |= (slave_address_to_respond_to << 1);
  // LPC_I2C2->ADR2 |= (slave_address_to_respond_to << 1);
  LPC_I2C2->ADR0 = (slave_address_to_respond_to << 0);
  /*
  LPC_I2C2->MASK0 &= ~(0x7F << 1);
  LPC_I2C2->MASK1 &= ~(0x7F << 1);
  LPC_I2C2->MASK2 &= ~(0x7F << 1);
  LPC_I2C2->MASK3 &= ~(0x7F << 1);
  */
  // enable for slave functions
  LPC_I2C2->CONSET = 0x44;
}