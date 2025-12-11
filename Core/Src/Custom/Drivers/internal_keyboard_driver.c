#include "main.h"
#include "stdint.h"
#include "Custom/Drivers/uart_driver.h"
#include "Custom/Drivers/pca9538_driver.h"
#include "Custom/Drivers/internal_keyboard_driver.h"

#define KBRD_ADDR 0xE2

#define ROW1 0xFE
#define ROW2 0xFD
#define ROW3 0xFB
#define ROW4 0xF7

static HAL_StatusTypeDef Set_Keyboard(void) {
  HAL_StatusTypeDef ret = HAL_OK;
  uint8_t buf;

  buf = 0;
  ret = PCA9538_Write_Register(KBRD_ADDR, POLARITY_INVERSION, &buf);
  if (ret != HAL_OK) {
    uart_send_string("Error write polarity\n");
    goto exit;
  }

  buf = 0;
  ret = PCA9538_Write_Register(KBRD_ADDR, OUTPUT_PORT, &buf);
  if (ret != HAL_OK) {
    uart_send_string("Error write output\n");
  }

exit:
  return ret;
}

static uint8_t Check_Row(uint8_t Nrow) {
  uint8_t Nkey = 0x00;
  HAL_StatusTypeDef ret = HAL_OK;
  uint8_t buf;
  uint8_t kbd_in;

  ret = Set_Keyboard();
  if (ret != HAL_OK) {
    uart_send_string("Error write init\n");
  }

  buf = Nrow;
  ret = PCA9538_Write_Register(KBRD_ADDR, CONFIG, &buf);
  if (ret != HAL_OK) {
    uart_send_string("Error write config\n");
  }

  ret = PCA9538_Read_Inputs(KBRD_ADDR, &buf);
  if (ret != HAL_OK) {
    uart_send_string("Read error\n");
  }

  kbd_in = buf & 0x70;
  Nkey = kbd_in;
  if (kbd_in != 0x70) {
    if (!(kbd_in & 0x10)) {
      switch (Nrow) {
      case ROW1:
        Nkey = 0x04;
        break;
      case ROW2:
        Nkey = 0x04;
        break;
      case ROW3:
        Nkey = 0x04;
        break;
      case ROW4:
        Nkey = 0x04;
        break;
      }
    }
    if (!(kbd_in & 0x20)) {
      switch (Nrow) {
      case ROW1:
        Nkey = 0x02;
        break;
      case ROW2:
        Nkey = 0x02;
        break;
      case ROW3:
        Nkey = 0x02;
        break;
      case ROW4:
        Nkey = 0x02;
        break;
      }
    }
    if (!(kbd_in & 0x40)) {
      switch (Nrow) {
      case ROW1:
        Nkey = 0x01;
        break;
      case ROW2:
        Nkey = 0x01;
        break;
      case ROW3:
        Nkey = 0x01;
        break;
      case ROW4:
        Nkey = 0x01;
        break;
      }
    }
  } else
    Nkey = 0x00;

  return Nkey;
}

int8_t getKeyPressed(void) {
  uint8_t row[4] = {ROW4, ROW3, ROW2, ROW1}, pressed = 0;
  int8_t pressed_key = -1;
  for (int i = 0; i < 4; i++) {
    uint8_t key = Check_Row(row[i]);
    if (key) {
      if (pressed) {
        return -1;
      }

      if (key == 0x01) {
        pressed_key = 2 + 3 * i;
      } else if (key == 0x02) {
        pressed_key = 1 + 3 * i;
      } else if (key == 0x04) {
        pressed_key = 0 + 3 * i;
      }

      pressed = 1;
    }
  }
  return pressed_key;
}
