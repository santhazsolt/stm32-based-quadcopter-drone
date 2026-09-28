
#ifndef INC_MOTOR_DRONE_H_
#define INC_MOTOR_DRONE_H_

#include "main.h"

typedef struct
{
	TIM_HandleTypeDef *timer;
	uint32_t motor1_ch;
	uint32_t motor2_ch;
	uint32_t motor3_ch;
	uint32_t motor4_ch;
}MotorDroneHandler;

typedef enum
{
	MOTOR_1,
	MOTOR_2,
	MOTOR_3,
	MOTOR_4,
}MOTOR_NUMBER;

void set_motor_voltage(MotorDroneHandler *motor, uint8_t motor_number, float percentage);
#endif /* INC_MOTOR_DRONE_H_ */
