#include "math.h"
#include "stdio.h"
#include "tim.h"
#include "Custom/Drivers/sound_driver.h"

void sound_driver_init(void) {
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  sound_stop();
}

void sound_play_frequency(uint32_t freq_hz) {
  if (freq_hz == 0) {
    sound_stop();
    return;
  }

  uint32_t timer_freq = HAL_RCC_GetPCLK2Freq();
  uint32_t target_arr = 1000;

  uint32_t prescaler = (timer_freq / (target_arr * freq_hz)) - 1;
  if (prescaler > 65535)
    prescaler = 65535;

  uint32_t arr_value = (timer_freq / ((prescaler + 1) * freq_hz)) - 1;
  if (arr_value > 65535) {
    arr_value = 65535;
    prescaler = (timer_freq / ((arr_value + 1) * freq_hz)) - 1;
  }

  uint32_t ccr_value = arr_value / 2;

  __HAL_TIM_SET_AUTORELOAD(&htim1, arr_value);
  __HAL_TIM_SET_PRESCALER(&htim1, prescaler);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr_value);
}

void sound_stop(void) {
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
}