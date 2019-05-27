#include "commands.h"
#include "hal.h"
#include "defines.h"

HAL_COMP(ramp);

// input
HAL_PIN(vel_ext_cmd);

HAL_PIN(en);

HAL_PIN(load);

HAL_PIN(max_vel);
HAL_PIN(max_acc);
HAL_PIN(at_speed_th);

// output
HAL_PIN(vel_cmd);

HAL_PIN(at_speed);
HAL_PIN(en_out);


static void nrt_init(void *ctx_ptr, hal_pin_inst_t *pin_ptr) {
  // struct ramp_ctx_t *ctx      = (struct ramp_ctx_t *)ctx_ptr;
  struct ramp_pin_ctx_t *pins = (struct ramp_pin_ctx_t *)pin_ptr;

  PIN(at_speed_th) = 0.01;
}

static void rt_func(float period, void *ctx_ptr, hal_pin_inst_t *pin_ptr) {
  // struct ramp_ctx_t *ctx      = (struct ramp_ctx_t *)ctx_ptr;
  struct ramp_pin_ctx_t *pins = (struct ramp_pin_ctx_t *)pin_ptr;
  
  float vel_ext_cmd = LIMIT(PIN(vel_ext_cmd), PIN(max_vel));

  if(PIN(en) <= 0.0){
    vel_ext_cmd = 0.0;
  }

  float vel_error = vel_ext_cmd - PIN(vel_cmd);

  PIN(vel_cmd) += LIMIT(vel_error, PIN(max_acc) * period * (1 - CLAMP(ABS(PIN(load)), 0, 1)));

  if(ABS(PIN(vel_ext_cmd) - PIN(vel_cmd)) < PIN(max_vel) * PIN(at_speed_th)){
    PIN(at_speed) = 1;
  }
  else{
    PIN(at_speed) = 0;
  }

  if(ABS(PIN(vel_cmd)) > 0.01 | ABS(PIN(vel_ext_cmd)) > 0.01){
    PIN(en_out) = 1;
  }
  else{
    PIN(en_out) = 0;
  }
}

hal_comp_t ramp_comp_struct = {
    .name      = "ramp",
    .nrt       = 0,
    .rt        = rt_func,
    .frt       = 0,
    .nrt_init  = nrt_init,
    .rt_start  = 0,
    .frt_start = 0,
    .rt_stop   = 0,
    .frt_stop  = 0,
    .ctx_size  = 0,
    .pin_count = sizeof(struct ramp_pin_ctx_t) / sizeof(struct hal_pin_inst_t),
};