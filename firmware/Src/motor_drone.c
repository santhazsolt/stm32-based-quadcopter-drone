
#include "motor_drone.h"

uint16_t compare_value;
void set_motor_voltage(MotorDroneHandler *motor, uint8_t motor_number, float percentage)
{
	if(percentage > 100.0)
	{
		percentage = 100.0;
	}
	compare_value = ((percentage * 1000) / (100.0));
	compare_value += 1000;
	switch(motor_number)
	{
	case MOTOR_1:
		__HAL_TIM_SET_COMPARE(motor-> timer, motor -> motor1_ch, compare_value);
	case MOTOR_2:
			__HAL_TIM_SET_COMPARE(motor-> timer, motor -> motor2_ch, compare_value);
	case MOTOR_3:
			__HAL_TIM_SET_COMPARE(motor-> timer, motor -> motor3_ch, compare_value);
	case MOTOR_4:
			__HAL_TIM_SET_COMPARE(motor-> timer, motor -> motor4_ch, compare_value);
	}
}
