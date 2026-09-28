
#ifndef INC_DRONE_CONTROL_H_
#define INC_DRONE_CONTROL_H_

#include "motor_drone.h"
#include "pid_control.h"
#include "euler_angles.h"

#define HOVERING_VOLTAGE  45.0

#define MOTOR_MIN 0.0f
#define MOTOR_MAX 100.0f

typedef struct
{
	pid_instance pid_pitch;
	pid_instance pid_roll;
	pid_instance pid_yaw;
//	pid_isntance pid_altitude;
	MotorDroneHandler motor_drone;
	euler_angles angles_estimation;
	uint8_t start_flight;
	float voltage_default;
}DroneTypeDef;

void drone_initialization(DroneTypeDef *drone);
void drone_apply_control(DroneTypeDef *drone, euler_angles reference);
void drone_update_angle(DroneTypeDef *drone, euler_angles angle);
void drone_set_zero(DroneTypeDef *drone);
#endif /* INC_DRONE_CONTROL_H_ */
