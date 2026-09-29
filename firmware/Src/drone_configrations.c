
#include "drone_control.h"

extern TIM_HandleTypeDef htim1;

DroneTypeDef drone = {
		.motor_drone = {
			.motor1_ch = TIM_CHANNEL_1,
			.motor2_ch = TIM_CHANNEL_2,
			.motor3_ch = TIM_CHANNEL_3,
			.motor4_ch = TIM_CHANNEL_4,
			.timer = &htim1,
		},
		.pid_pitch = {
			.d_gain = 20,
			.error_integral = 0,
			.i_gain = 20,
			.integral_max = 80 * SAMPLE_RATE / 10,
			.last_error = 0,
			.output = 0,
			.p_gain = 50,
			.pid_max = 90,
			.sam_rate = SAMPLE_RATE
		},
		.pid_roll = {
			.d_gain = 20,
			.error_integral = 0,
			.i_gain = 20.0,
			.integral_max = 80 * SAMPLE_RATE / 10,
			.last_error = 0,
			.output = 0,
			.p_gain = 50,
			.pid_max = 90,
			.sam_rate = SAMPLE_RATE
		},
		.pid_yaw = {
			.d_gain = 4,
			.error_integral = 0,
			.i_gain = 2,
			.integral_max = 40, // * SAMPLE_RATE / 6,
			.last_error = 0,
			.output = 0,
			.p_gain = 10,
			.pid_max = 50,
			.sam_rate = SAMPLE_RATE
		},
//		.pid_altitude = {
//			.d_gain = 0.0,
//			.error_integral = 0,
//			.i_gain = 1,
//			.integral_max = 40 * SAMPLE_RATE / 1,
//			.last_error = 0,
//			.output = 0,
//			.p_gain = 10,
//			.pid_max = 80,
//			.sam_rate = SAMPLE_RATE
//		},

};
