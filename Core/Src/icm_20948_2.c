//
//#include "icm_20948_2.h"
//#include "stdio.h"
//
//volatile uint8_t spi_dma_busy = 0;
//volatile uint32_t dma_callback_count = 0;
//
//
//static void sel_user_bank(user_bank ub);
//
//static void ak09916_mag_init();
//static void ak09916_read_reg(uint8_t onset_reg, uint8_t len);
//static void ak09916_write_reg(uint8_t reg, uint8_t data);
//static void remove_gyro_bias();
//
//static void activate_imu()
//{
//	HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN,
//			GPIO_PIN_RESET);
//}
//static void deactivate_imu()
//{
//	HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN,
//				GPIO_PIN_SET);
//}
//static void sel_user_bank(user_bank ub)
//{
//	uint8_t data = ub;
//	uint8_t reg = REG_BANK_SEL;
//	activate_imu();
//	HAL_SPI_Transmit(&IMU_SPI, &reg, 1, 100);
//	HAL_SPI_Transmit(&IMU_SPI, &data, 1, 100);
//	deactivate_imu();
//}
//void icm_20948_read_reg(user_bank ub, uint8_t address, uint8_t *data)
//{
//	uint8_t temp_data = 0x80|address;
//	sel_user_bank(ub);
//	activate_imu();
//	HAL_SPI_Transmit(&IMU_SPI, &temp_data , 1, 100);
//	HAL_SPI_Receive(&IMU_SPI, data, 1, 100);
//	deactivate_imu();
//}
//
//void icm_20948_write_reg(user_bank ub, uint8_t reg, uint8_t data)
//{
//	sel_user_bank(ub);
//	activate_imu();
//	HAL_SPI_Transmit(&IMU_SPI, &reg, 1, 100);
//	HAL_SPI_Transmit(&IMU_SPI, &data, 1, 100);
//	deactivate_imu();
//}
//void icm_20948_init()
//{
//	uint8_t temp_data;
//	// IMU reset-sleep-clock, page 37, 0xc1 = 0b1100 0001
//	icm_20948_write_reg(_b0, PWR_MGMT_1, 0xc1);
//	HAL_Delay(100);
//	// Exit from sleep mode, selecting the clock page 37, 0x01 = 0b0000 0001
//	icm_20948_write_reg(_b0, PWR_MGMT_1, 0x01);
//
//	// Accelerometer configuration, sample rate divider = 0, page 63
//	icm_20948_write_reg(_b2, ACCEL_SMPLRT_DIV_1, 0x00);
//	icm_20948_write_reg(_b2, ACCEL_SMPLRT_DIV_2, 0x00);
//
//	// Accelerometer configuration, accelerometer range set and enable digital filter, page 64
//	icm_20948_write_reg(_b2, ACCEL_CONFIG, ((ACCEL_RANGE_VALUE << 1)|0x01));
//
//	// Put the serial interface in SPI mode only, page 36, 0x10 = 0b0001 0000
//	temp_data = 0x10;
//	icm_20948_write_reg(_b0, USER_CTRL, temp_data);
//
//	sel_user_bank(_b0);
//	remove_gyro_bias();
//
//	// Gyroscope configuration, sample rate divider = 0, page 59
//	icm_20948_write_reg(_b2, GYRO_SMPLRT_DIV, 0x00);
//
//	// Gyroscope configuration, gyroscope range set and enable digital filter, page 59
//	icm_20948_write_reg(_b2, GYRO_CONFIG_1, ((GYRO_RANGE_VALUE << 1)|0x01));
//
//	ak09916_mag_init();
//	ak09916_read_reg(MAG_DATA_ONSET, 8);
//
//	sel_user_bank(_b0);
//
//}
//
//uint8_t icm_20948_read_data(icm_20948_data* data)
//{
//	static uint8_t data_rx[20];
//	uint8_t temp_data = 0x80|ACCEL_XOUT_H;
//
//	// Don't start new transfer if previous DMA not done
//	if(spi_dma_busy) return 1;
//
//	data ->x_accel = ((int16_t)data_rx[0]<<8)| (int16_t)data_rx[1];
//	data ->y_accel = ((int16_t)data_rx[2]<<8)| (int16_t) data_rx[3];
//	data ->z_accel = ((int16_t)data_rx[4]<<8)| (int16_t) data_rx[5];
//
//	data ->x_gyro = ((int16_t)data_rx[6]<<8) | (int16_t) data_rx[7];
//	data ->y_gyro = ((int16_t)data_rx[8]<<8) | (int16_t)data_rx[9];
//	data ->z_gyro = ((int16_t)data_rx[10]<<8)| (int16_t)data_rx[11];
//
//	data ->x_magnet = (((int16_t)data_rx[15]<<8) | (int16_t) data_rx[14]) - MAG_X_BIAS;
//	data ->y_magnet = (((int16_t)data_rx[17]<<8) | (int16_t)data_rx[16]) - MAG_Y_BIAS;
//	data ->z_magnet = (((int16_t)data_rx[19]<<8)| (int16_t)data_rx[18]) - MAG_Z_BIAS;
//
//	// Start next DMA transfer
//	spi_dma_busy = 1;
//	activate_imu();
//	HAL_SPI_Transmit(&IMU_SPI, &temp_data, 1, 10);
//	HAL_SPI_Receive_DMA(&IMU_SPI, data_rx, 20);
//
//	return 0;
//}
//
//static void ak09916_mag_init()
//{
//	uint8_t temp_data;
//
//	// I2C master reset, page 36
//	icm_20948_read_reg(_b0, USER_CTRL, &temp_data);
//	temp_data |= 0x02;
//	icm_20948_write_reg(_b0, USER_CTRL, temp_data);
//	HAL_Delay(100);
//
//	// I2C Master enable, page 36
//	icm_20948_read_reg(_b0, USER_CTRL, &temp_data);
//	temp_data |= 0x20;
//	icm_20948_write_reg(_b0, USER_CTRL, temp_data);
//	HAL_Delay(10);
//
//	// I2C Master clock: 7 (400 kHz), page 68
//	temp_data = 0x07;
//	icm_20948_write_reg(_b3, I2C_MST_CTRL, temp_data);
//	HAL_Delay(10);
//
//	// LP_CONFIG:ODR is determined by I2C_MST_ODR_CONFIG register,page 37
//	temp_data = 0x40;
//	icm_20948_write_reg(_b0, LP_CONFIG, temp_data);
//	HAL_Delay(10);
//
//	// I2C_MST_ODR_CONFIG: 1.1 kHz/(2^3) = 136 Hz, page 68
//	temp_data = 0x03;
//	icm_20948_write_reg(_b3, I2C_MST_ODR_CONFIG, temp_data);
//	HAL_Delay(10);
//
//	// I2C_MST_DELAY_CTRL: delays shadowing of external sensors, page 69
//	temp_data = 0x80;
//	icm_20948_write_reg(_b3, I2C_MST_DELAY_CTRL, temp_data);
//	HAL_Delay(10);
//
//	// Magnetometer reset, page, page 80
//	ak09916_write_reg(MAG_CTRL3, 0x01);
//	HAL_Delay(100);
//
//	// continuous mode 4: 100 Hz, page 79
//	ak09916_write_reg(MAG_CTRL2, 0x08);
//}
//
//static void ak09916_write_reg(uint8_t reg, uint8_t data)
//{
//	icm_20948_write_reg(_b3, I2C_SLV0_ADDR, AK09916_ADDRESS);
//	icm_20948_write_reg(_b3, I2C_SLV0_REG , reg);
//	icm_20948_write_reg(_b3, I2C_SLV0_DO , data);
//
//	// Enable and single data write
//	HAL_Delay(50);
//	icm_20948_write_reg(_b3, I2C_SLV0_CTRL, 0x80|0x01);
//	HAL_Delay(50);
//}
//
//static void ak09916_read_reg(uint8_t onset_reg, uint8_t len)
//{
//	icm_20948_write_reg(_b3, I2C_SLV0_ADDR, 0x80|AK09916_ADDRESS);
//	icm_20948_write_reg(_b3, I2C_SLV0_REG , onset_reg);
//	HAL_Delay(50);
//	icm_20948_write_reg(_b3, I2C_SLV0_CTRL, 0x80|len);
//	HAL_Delay(50);
//}
//
//void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
//{
//	if(hspi == &IMU_SPI)
//	{
//		deactivate_imu();
//		spi_dma_busy = 0;
//        dma_callback_count++;  // watch this in Live Expressions
//
//	}
//}
//
//static void remove_gyro_bias()
//{
//	int16_t x_gyro_bias, y_gyro_bias, z_gyro_bias;
//	icm_20948_data data;
//	int32_t x_bias = 0, y_bias = 0, z_bias = 0;
//	for(int i = 0; i < 500; i++)
//	{
//		icm_20948_read_data(&data);
//		x_bias += (int32_t)data.x_gyro;
//		y_bias += (int32_t)data.y_gyro;
//		z_bias += (int32_t)data.z_gyro;
//		HAL_Delay(2);
//	}
//	x_gyro_bias = -(int16_t)(x_bias / 2000);
//	y_gyro_bias = -(int16_t)(y_bias / 2000);
//	z_gyro_bias = -(int16_t)(z_bias / 2000);
//
//	printf("x,y,z %d, %d, %d \n", x_gyro_bias, y_gyro_bias, z_gyro_bias);
//	HAL_Delay(100);
//	icm_20948_write_reg(_b2, XG_OFFS_USRH, (uint8_t)(x_gyro_bias >> 8));
//	icm_20948_write_reg(_b2, XG_OFFS_USRL, (uint8_t)(x_gyro_bias));
//	icm_20948_write_reg(_b2, YG_OFFS_USRH, (uint8_t)(y_gyro_bias >> 8));
//	icm_20948_write_reg(_b2, YG_OFFS_USRL, (uint8_t)(y_gyro_bias));
//	icm_20948_write_reg(_b2, ZG_OFFS_USRH, (uint8_t)(z_gyro_bias >> 8));
//	icm_20948_write_reg(_b2, ZG_OFFS_USRL, (uint8_t)(z_gyro_bias));
//}







#include "icm_20948_2.h"
#include "stdio.h"

volatile uint8_t spi_dma_busy = 0;
volatile uint32_t dma_callback_count = 0;

// NEW: self-test readback values. File-scope (global) so Live Expressions
// can always see them, at any pause point, regardless of scope.
uint8_t st_who, st_pm1, st_pm2, st_acc_cfg, st_gyr_cfg;

static void sel_user_bank(user_bank ub);

static void ak09916_mag_init();
static void ak09916_read_reg(uint8_t onset_reg, uint8_t len);
static void ak09916_write_reg(uint8_t reg, uint8_t data);
static void remove_gyro_bias();

// NEW: blocking (non-DMA) burst read, used only during startup calibration.
static void icm_20948_read_data_blocking(icm_20948_data* data);

static void activate_imu()
{
	HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_RESET);
}
static void deactivate_imu()
{
	HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_SET);
}
static void sel_user_bank(user_bank ub)
{
	uint8_t data = ub;
	uint8_t reg = REG_BANK_SEL;
	activate_imu();
	HAL_SPI_Transmit(&IMU_SPI, &reg, 1, 100);
	HAL_SPI_Transmit(&IMU_SPI, &data, 1, 100);
	deactivate_imu();
}
void icm_20948_read_reg(user_bank ub, uint8_t address, uint8_t *data)
{
	uint8_t tx[2] = { 0x80 | address, 0x00 };
	uint8_t rx[2] = { 0, 0 };
	sel_user_bank(ub);
	activate_imu();
	HAL_SPI_TransmitReceive(&IMU_SPI, tx, rx, 2, 100);
	deactivate_imu();
	*data = rx[1];   // second byte is the register value

}

void icm_20948_write_reg(user_bank ub, uint8_t reg, uint8_t data)
{
	sel_user_bank(ub);
	activate_imu();
	HAL_SPI_Transmit(&IMU_SPI, &reg, 1, 100);
	HAL_SPI_Transmit(&IMU_SPI, &data, 1, 100);
	deactivate_imu();
}

void icm_20948_init()
{
	uint8_t temp_data;

	// IMU reset-sleep-clock, page 37, 0xc1 = 0b1100 0001 (DEVICE_RESET)
	icm_20948_write_reg(_b0, PWR_MGMT_1, 0xc1);
	HAL_Delay(100);

	// Exit sleep, auto clock select, page 37, 0x01 = 0b0000 0001
	icm_20948_write_reg(_b0, PWR_MGMT_1, 0x01);
	HAL_Delay(10);

	// FIX: explicitly enable all accel + gyro axes.
	// After a DEVICE_RESET the part can come up with axes disabled, which
	// makes accel/gyro output registers read exactly 0x0000 while the
	// I2C-master (magnetometer) path keeps working. 0x00 = all axes ON.
	icm_20948_write_reg(_b0, PWR_MGMT_2, 0x00);
	HAL_Delay(10);

	// Put the serial interface in SPI mode only, page 36, 0x10 = 0b0001 0000
	// FIX: moved up so USER_CTRL is set before any I2C-master / bias work.
	temp_data = 0x10;
	icm_20948_write_reg(_b0, USER_CTRL, temp_data);
	HAL_Delay(10);

	// ---- Accelerometer configuration ----
	// Sample rate divider = 0, page 63
	icm_20948_write_reg(_b2, ACCEL_SMPLRT_DIV_1, 0x00);
	icm_20948_write_reg(_b2, ACCEL_SMPLRT_DIV_2, 0x00);
	// Range + enable digital LPF, page 64
	icm_20948_write_reg(_b2, ACCEL_CONFIG, ((ACCEL_RANGE_VALUE << 1) | 0x01));

	// ---- Gyroscope configuration ----
	// Sample rate divider = 0, page 59
	icm_20948_write_reg(_b2, GYRO_SMPLRT_DIV, 0x00);
	// Range + enable digital LPF, page 59
	icm_20948_write_reg(_b2, GYRO_CONFIG_1, ((GYRO_RANGE_VALUE << 1) | 0x01));

	sel_user_bank(_b0);
	HAL_Delay(50);   // let accel/gyro settle before sampling

	// NEW: read back key registers BEFORE calibration, so the values are
	// captured even if something later stalls. Watch st_who / st_pm2 / etc.
	icm_20948_selftest_dump();

	// FIX: bias calibration now runs AFTER gyro is fully configured and awake,
	// and uses the BLOCKING read (no DMA busy-wait to deadlock on).
	remove_gyro_bias();

	// ---- Magnetometer ----
	ak09916_mag_init();
	ak09916_read_reg(MAG_DATA_ONSET, 8);

	sel_user_bank(_b0);
}

// Reads key config registers into file-scope globals for inspection.
// Expected after init:
//   st_who     = 0xEA (234)  -> SPI + chip OK
//   st_pm1     = 0x01 (1)    -> awake
//   st_pm2     = 0x00 (0)    -> accel+gyro axes enabled  (0x3F/63 = still off)
//   st_acc_cfg = (ACCEL_RANGE_VALUE<<1)|1
//   st_gyr_cfg = (GYRO_RANGE_VALUE<<1)|1
void icm_20948_selftest_dump(void)
{
	icm_20948_read_reg(_b0, WHO_AM_I,      &st_who);
	icm_20948_read_reg(_b0, PWR_MGMT_1,    &st_pm1);
	icm_20948_read_reg(_b0, PWR_MGMT_2,    &st_pm2);
	icm_20948_read_reg(_b2, ACCEL_CONFIG,  &st_acc_cfg);
	icm_20948_read_reg(_b2, GYRO_CONFIG_1, &st_gyr_cfg);
	sel_user_bank(_b0);

	printf("WHO=0x%02X PM1=0x%02X PM2=0x%02X ACC=0x%02X GYR=0x%02X\n",
	       st_who, st_pm1, st_pm2, st_acc_cfg, st_gyr_cfg);
}

// NEW: blocking burst read of accel+gyro data registers (no DMA).
// Used only during startup calibration so it can't deadlock on spi_dma_busy.
static void icm_20948_read_data_blocking(icm_20948_data* data)
{
	uint8_t rx[12];
	uint8_t reg = 0x80 | ACCEL_XOUT_H;

	sel_user_bank(_b0);
	activate_imu();
	HAL_SPI_Transmit(&IMU_SPI, &reg, 1, 10);
	HAL_SPI_Receive(&IMU_SPI, rx, 12, 100);   // blocking, 12 bytes = accel+gyro
	deactivate_imu();

	data->x_accel = ((int16_t)rx[0]  << 8) | rx[1];
	data->y_accel = ((int16_t)rx[2]  << 8) | rx[3];
	data->z_accel = ((int16_t)rx[4]  << 8) | rx[5];
	data->x_gyro  = ((int16_t)rx[6]  << 8) | rx[7];
	data->y_gyro  = ((int16_t)rx[8]  << 8) | rx[9];
	data->z_gyro  = ((int16_t)rx[10] << 8) | rx[11];
}

uint8_t icm_20948_read_data(icm_20948_data* data)
{
	static uint8_t data_rx[20];
	uint8_t temp_data = 0x80 | ACCEL_XOUT_H;

	// Don't start a new transfer if the previous DMA isn't done
	if (spi_dma_busy) return 1;

	// Parse the bytes captured by the PREVIOUS DMA transfer
	data->x_accel = ((int16_t)data_rx[0] << 8) | (int16_t)data_rx[1];
	data->y_accel = ((int16_t)data_rx[2] << 8) | (int16_t)data_rx[3];
	data->z_accel = ((int16_t)data_rx[4] << 8) | (int16_t)data_rx[5];

	data->x_gyro  = ((int16_t)data_rx[6]  << 8) | (int16_t)data_rx[7];
	data->y_gyro  = ((int16_t)data_rx[8]  << 8) | (int16_t)data_rx[9];
	data->z_gyro  = ((int16_t)data_rx[10] << 8) | (int16_t)data_rx[11];

	data->x_magnet = (((int16_t)data_rx[15] << 8) | (int16_t)data_rx[14]) - MAG_X_BIAS;
	data->y_magnet = (((int16_t)data_rx[17] << 8) | (int16_t)data_rx[16]) - MAG_Y_BIAS;
	data->z_magnet = (((int16_t)data_rx[19] << 8) | (int16_t)data_rx[18]) - MAG_Z_BIAS;

	// FIX: guarantee we read the data registers from bank 0.
	sel_user_bank(_b0);

	// Start next DMA transfer
	spi_dma_busy = 1;
	activate_imu();
	HAL_SPI_Transmit(&IMU_SPI, &temp_data, 1, 10);
	HAL_SPI_Receive_DMA(&IMU_SPI, data_rx, 20);

	return 0;
}

static void ak09916_mag_init()
{
	uint8_t temp_data;

	// I2C master reset, page 36
	icm_20948_read_reg(_b0, USER_CTRL, &temp_data);
	temp_data |= 0x02;
	icm_20948_write_reg(_b0, USER_CTRL, temp_data);
	HAL_Delay(100);

	// I2C Master enable, page 36
	icm_20948_read_reg(_b0, USER_CTRL, &temp_data);
	temp_data |= 0x20;
	icm_20948_write_reg(_b0, USER_CTRL, temp_data);
	HAL_Delay(10);

	// I2C Master clock: 7 (400 kHz), page 68
	temp_data = 0x07;
	icm_20948_write_reg(_b3, I2C_MST_CTRL, temp_data);
	HAL_Delay(10);

	// LP_CONFIG: ODR determined by I2C_MST_ODR_CONFIG, page 37
	temp_data = 0x40;
	icm_20948_write_reg(_b0, LP_CONFIG, temp_data);
	HAL_Delay(10);

	// I2C_MST_ODR_CONFIG: 1.1 kHz / (2^3) = 136 Hz, page 68
	temp_data = 0x03;
	icm_20948_write_reg(_b3, I2C_MST_ODR_CONFIG, temp_data);
	HAL_Delay(10);

	// I2C_MST_DELAY_CTRL: delays shadowing of external sensors, page 69
	temp_data = 0x80;
	icm_20948_write_reg(_b3, I2C_MST_DELAY_CTRL, temp_data);
	HAL_Delay(10);

	// Magnetometer reset, page 80
	ak09916_write_reg(MAG_CTRL3, 0x01);
	HAL_Delay(100);

	// Continuous mode 4: 100 Hz, page 79
	ak09916_write_reg(MAG_CTRL2, 0x08);
}

static void ak09916_write_reg(uint8_t reg, uint8_t data)
{
	icm_20948_write_reg(_b3, I2C_SLV0_ADDR, AK09916_ADDRESS);
	icm_20948_write_reg(_b3, I2C_SLV0_REG,  reg);
	icm_20948_write_reg(_b3, I2C_SLV0_DO,   data);

	HAL_Delay(50);
	icm_20948_write_reg(_b3, I2C_SLV0_CTRL, 0x80 | 0x01);  // enable, 1 byte
	HAL_Delay(50);
}

static void ak09916_read_reg(uint8_t onset_reg, uint8_t len)
{
	icm_20948_write_reg(_b3, I2C_SLV0_ADDR, 0x80 | AK09916_ADDRESS);  // read bit
	icm_20948_write_reg(_b3, I2C_SLV0_REG,  onset_reg);
	HAL_Delay(50);
	icm_20948_write_reg(_b3, I2C_SLV0_CTRL, 0x80 | len);
	HAL_Delay(50);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &IMU_SPI)
	{
		deactivate_imu();
		spi_dma_busy = 0;
		dma_callback_count++;   // watch this in Live Expressions
	}
}

static void remove_gyro_bias()
{
	int16_t x_gyro_bias, y_gyro_bias, z_gyro_bias;
	icm_20948_data data;
	int32_t x_bias = 0, y_bias = 0, z_bias = 0;

	// FIX: use the BLOCKING read here — no DMA, no spi_dma_busy busy-wait,
	// so this can't deadlock during init.
	for (int i = 0; i < 500; i++)
	{
		icm_20948_read_data_blocking(&data);
		x_bias += (int32_t)data.x_gyro;
		y_bias += (int32_t)data.y_gyro;
		z_bias += (int32_t)data.z_gyro;
		HAL_Delay(2);
	}

	// NOTE: sum 500 samples, divide by 2000 -> scales offset by 1/4 to match
	// the XG_OFFS_USR register LSB format. Deliberate; matches your setup.
	x_gyro_bias = -(int16_t)(x_bias / 2000);
	y_gyro_bias = -(int16_t)(y_bias / 2000);
	z_gyro_bias = -(int16_t)(z_bias / 2000);

	printf("gyro bias x,y,z = %d, %d, %d\n", x_gyro_bias, y_gyro_bias, z_gyro_bias);
	HAL_Delay(100);

	icm_20948_write_reg(_b2, XG_OFFS_USRH, (uint8_t)(x_gyro_bias >> 8));
	icm_20948_write_reg(_b2, XG_OFFS_USRL, (uint8_t)(x_gyro_bias));
	icm_20948_write_reg(_b2, YG_OFFS_USRH, (uint8_t)(y_gyro_bias >> 8));
	icm_20948_write_reg(_b2, YG_OFFS_USRL, (uint8_t)(y_gyro_bias));
	icm_20948_write_reg(_b2, ZG_OFFS_USRH, (uint8_t)(z_gyro_bias >> 8));
	icm_20948_write_reg(_b2, ZG_OFFS_USRL, (uint8_t)(z_gyro_bias));
}
