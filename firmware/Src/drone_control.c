#include "drone_control.h"

//#define MOTOR_MIN    1000.0f   // ESC minimum pulse
//#define MOTOR_MAX    2000.0f   // ESC maximum pulse
//#define MOTOR_HOVER  1300.0f   // starting hover estimate

float motor1_command;
float motor2_command;
float motor3_command;
float motor4_command;


void drone_initialization(DroneTypeDef *drone)
{
	reset_pid(&drone->pid_pitch);
	reset_pid(&drone->pid_roll);
	reset_pid(&drone->pid_yaw);
}

void drone_update_angle(DroneTypeDef *drone, euler_angles angle)
{
	drone->angles_estimation = angle;
}

void drone_set_zero(DroneTypeDef *drone)
{
	set_motor_voltage(&drone->motor_drone, MOTOR_1, 0);
	set_motor_voltage(&drone->motor_drone, MOTOR_2, 0);
	set_motor_voltage(&drone->motor_drone, MOTOR_3, 0);
	set_motor_voltage(&drone->motor_drone, MOTOR_4, 0);
}

void drone_apply_control(DroneTypeDef *drone, euler_angles reference)
{
//	if(drone->start_flight == 0)
//	{
//		drone_set_zero(drone);
//		return;  // exit immediately, no motor commands
//	}

	float yaw_error;

	if(drone->angles_estimation.pitch > 0.8f || drone->angles_estimation.pitch < -0.8f ||
	   drone->angles_estimation.roll > 0.8f || drone->angles_estimation.roll < -0.8f)
	{
		set_motor_voltage(&drone->motor_drone, MOTOR_1, 0);
		set_motor_voltage(&drone->motor_drone, MOTOR_2, 0);
		set_motor_voltage(&drone->motor_drone, MOTOR_3, 0);
		set_motor_voltage(&drone->motor_drone, MOTOR_4, 0);
		drone->start_flight = 0;
	}
	else
	{
		yaw_error = reference.yaw - drone->angles_estimation.yaw;
		if(yaw_error > PI)
		{
			yaw_error -= 2 * PI;
		}
		if(yaw_error < -PI)
		{
			yaw_error += 2 * PI;
		}

		apply_pid(&drone->pid_pitch, reference.pitch - drone->angles_estimation.pitch);
		apply_pid(&drone->pid_roll, reference.roll - drone->angles_estimation.roll);
		apply_pid(&drone->pid_yaw, yaw_error);

		// Motor layout:
		// motor1 = FL (front-left)
		// motor2 = RL (rear-left)
		// motor3 = RR (rear-right)
		// motor4 = FR (front-right)

		motor1_command = drone->voltage_default; // FL
		motor2_command = drone->voltage_default; // RL
		motor3_command = drone->voltage_default; // RR
		motor4_command = drone->voltage_default; // FR

		// Pitch: front motors vs rear motors
		motor1_command += drone->pid_pitch.output; // FL
		motor2_command += drone->pid_pitch.output; // RL
		motor3_command -= drone->pid_pitch.output; // RR
		motor4_command -= drone->pid_pitch.output; // FR

		// Roll: left side vs right side
		motor1_command += drone->pid_roll.output; // FL (left)
		motor2_command -= drone->pid_roll.output; // RL (left)
		motor3_command -= drone->pid_roll.output; // RR (right)
		motor4_command += drone->pid_roll.output; // FR (right)

		// Yaw: diagonal pairs (FL/RR vs RL/FR)
		motor1_command += drone->pid_yaw.output; // FL
		motor2_command -= drone->pid_yaw.output; // RL
		motor3_command += drone->pid_yaw.output; // RR
		motor4_command -= drone->pid_yaw.output; // FR

//		if(motor1_command < MOTOR_MIN) motor1_command = MOTOR_MIN;
//		if(motor2_command < MOTOR_MIN) motor2_command = MOTOR_MIN;
//		if(motor3_command < MOTOR_MIN) motor3_command = MOTOR_MIN;
//		if(motor4_command < MOTOR_MIN) motor4_command = MOTOR_MIN;
//
//		if(motor1_command > MOTOR_MAX) motor1_command = MOTOR_MAX;
//		if(motor2_command > MOTOR_MAX) motor2_command = MOTOR_MAX;
//		if(motor3_command > MOTOR_MAX) motor3_command = MOTOR_MAX;
//		if(motor4_command > MOTOR_MAX) motor4_command = MOTOR_MAX;

		if(motor1_command < 0)
		{
			motor1_command = 0;
		}
		if(motor2_command < 0)
				{
					motor2_command = 0;
				}
		if(motor3_command < 0)
				{
					motor3_command = 0;
				}
		if(motor4_command < 0)
				{
					motor4_command = 0;
				}
		set_motor_voltage(&drone->motor_drone, MOTOR_1, motor1_command);
		set_motor_voltage(&drone->motor_drone, MOTOR_2, motor2_command);
		set_motor_voltage(&drone->motor_drone, MOTOR_3, motor3_command);
		set_motor_voltage(&drone->motor_drone, MOTOR_4, motor4_command);
	}
}
