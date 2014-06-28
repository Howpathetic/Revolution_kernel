/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2014 Jaka Valenko <jaka.valenko6@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
 
#include <asm/mach-types.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <mach/board.h>
#include <mach/msm_bus_board.h>
#include <mach/gpiomux.h>
#include <asm/setup.h>
#include "devices-msm8x60.h"
#include "devices.h"
#include "board-shooter_u.h"

#include <linux/spi/spi.h>
#include <mach/rpm-regulator.h>

#ifdef CONFIG_MSM_CAMERA_FLASH
#include <linux/htc_flashlight.h>
#include <linux/leds.h>
#endif

// static int camera_sensor_power_enable(char *power, unsigned volt, struct regulator **sensor_power);
// static int camera_sensor_power_disable(struct regulator *sensor_power);
// static struct platform_device msm_camera_server = {
	// .name = "msm_cam_server",
	// .id = 0,
// };

#ifdef CONFIG_MSM_CAMERA
static struct msm_bus_vectors cam_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_zsl_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 566231040,
		.ib  = 905969664,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 706199040,
		.ib  = 1129918464,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 320864256,
		.ib  = 513382810,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 320864256,
		.ib  = 513382810,
	},
};

static struct msm_bus_vectors cam_stereo_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 212336640,
		.ib  = 339738624,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 25090560,
		.ib  = 40144896,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 239708160,
		.ib  = 383533056,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 79902720,
		.ib  = 127844352,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_stereo_snapshot_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 300902400,
		.ib  = 481443840,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 230307840,
		.ib  = 368492544,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 245113344,
		.ib  = 392181351,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab  = 106536960,
		.ib  = 170459136,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 106536960,
		.ib  = 170459136,
	},
};

static struct msm_bus_paths cam_bus_client_config[] = {
	{
		ARRAY_SIZE(cam_init_vectors),
		cam_init_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_stereo_video_vectors),
		cam_stereo_video_vectors,
	},
	{
		ARRAY_SIZE(cam_stereo_snapshot_vectors),
		cam_stereo_snapshot_vectors,
	},
};

static struct msm_bus_scale_pdata cam_bus_client_pdata = {
		cam_bus_client_config,
		ARRAY_SIZE(cam_bus_client_config),
		.name = "msm_camera",
};

struct msm_camera_device_platform_data msm_camera_csi_device_data[] = {
	{
	.ioext.csiphy = 0x04800000,
	.ioext.csisz  = 0x00000400,
	.ioext.csiirq = CSI_0_IRQ,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 228570000,
	.csid_core = 0,
	.is_csic = 1,
	.is_vpe = 1,
	.cam_bus_scale_table = &cam_bus_client_pdata,
	},
	{
	.ioext.csiphy = 0x04900000,
	.ioext.csisz  = 0x00000400,
	.ioext.csiirq = CSI_1_IRQ,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 228570000,
	.csid_core = 1,
	.is_csic = 1,
	.is_vpe = 1,
	.cam_bus_scale_table = &cam_bus_client_pdata,
	},
};

#ifdef CONFIG_MSM_CAMERA_FLASH
int flashlight_control(int mode)
{
#ifdef CONFIG_FLASHLIGHT_AAT1277
	return aat1277_flashlight_control(mode);
#else
	return 0;
#endif
}

static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_CURRENT_DRIVER,
	.camera_flash = flashlight_control,
};
#endif

static struct regulator *shooter_u_reg_8901_l6 = NULL;
static struct regulator *shooter_u_reg_8058_l8 = NULL;
static struct regulator *shooter_u_reg_8058_l9 = NULL;
static struct regulator *shooter_u_reg_8058_l10 = NULL;
static struct regulator *shooter_u_reg_8058_l15 = NULL;

static int camera_sensor_power_enable(char *power, unsigned volt, struct regulator **sensor_power)
{
	int rc;
	
	if (power == NULL)
		return -ENODEV;
		
	*sensor_power = regulator_get(NULL, power);
	
	if (IS_ERR(*sensor_power)) {
		pr_info("%s: failed to Unable to get %s\n", __func__, power);
		return -ENODEV;
	}
	
	if (volt != 1800000) {
		rc = regulator_set_voltage(*sensor_power, volt, volt);
		if (rc < 0) {
			pr_info("%s: failed to unable to set %s voltage to %d rc:%d\n",
					__func__, power, volt, rc);
			regulator_put(*sensor_power);
			*sensor_power = NULL;
			return -ENODEV;
		}
	}

	rc = regulator_enable(*sensor_power);
	if (rc < 0) {
		pr_info("%s: failed to Enable regulator %s failed\n", __func__, power);
		regulator_put(*sensor_power);
		*sensor_power = NULL;
		return -ENODEV;
	}

	return rc;
}

static struct regulator *shooter_u_reg_8901_lvs2 = NULL;
static struct regulator *shooter_u_reg_8901_lvs3 = NULL;

static int camera_sensor_power_enable_8901(char *power, struct regulator **sensor_power)
{
	int rc;
	pr_info("%s %s\n", __func__, power);
	if (power == NULL)
		return -ENODEV;
		
	*sensor_power = regulator_get(NULL, power);
	
	if (IS_ERR(*sensor_power)) {
		pr_info("%s: failed to Unable to get %s\n", __func__, power);
		return -ENODEV;
	}
	
	rc = regulator_enable(*sensor_power);
	if (rc < 0) {
		pr_info("%s: failed to Enable regulator %s failed\n", __func__, power);
		regulator_put(*sensor_power);
		*sensor_power = NULL;
		return -ENODEV;
	}
	
	return rc;
}

static int camera_sensor_power_disable(struct regulator *sensor_power)
{

	int rc;
	if (sensor_power == NULL)
		return -ENODEV;

	if (IS_ERR(sensor_power)) {
		pr_info("%s: failed to Invalid requlator ptr\n", __func__);
		return -ENODEV;
	}
	
	rc = regulator_disable(sensor_power);
	if (rc < 0) {
		pr_info("%s: disable regulator failed\n", __func__);
		regulator_put(sensor_power);
		sensor_power = NULL;
		return -ENODEV;
	}
	regulator_put(sensor_power);
	sensor_power = NULL;
	return rc;
}

static uint32_t camera_off_gpio_table_liteon[] = {
	GPIO_CFG(SHOOTER_U_CAM_I2C_SDA, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_CAM_I2C_SCL, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_SP3D_MCLK, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DO, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DI, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CS, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CLK, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_S5K4E1_VCM_PD, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_S5K4E1_INTB, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_S5K4E1_PD, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_WEBCAM_RST, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_WEBCAM_STB, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_CAM_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static uint32_t camera_on_gpio_table_liteon[] = {
	GPIO_CFG(SHOOTER_U_CAM_I2C_SDA, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_CAM_I2C_SCL, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_SP3D_MCLK, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DO, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DI, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CS, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CLK, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_S5K4E1_VCM_PD, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_S5K4E1_INTB, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_S5K4E1_PD, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_WEBCAM_RST, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_WEBCAM_STB, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_CAM_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static uint32_t camera_off_gpio_table_sp3d[] = {
	GPIO_CFG(SHOOTER_U_CAM_I2C_SDA, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_CAM_I2C_SCL, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_SP3D_MCLK, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DO, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DI, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CS, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CLK, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_GATE, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_CORE_GATE, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SYS_RST, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_PDX, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_WEBCAM_RST, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_WEBCAM_STB, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_CAM_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static uint32_t camera_on_gpio_table_sp3d[] = {
	GPIO_CFG(SHOOTER_U_CAM_I2C_SDA, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_CAM_I2C_SCL, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_SP3D_MCLK, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DO, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DI, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CS, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CLK, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_GATE, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_CORE_GATE, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_SYS_RST, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_SP3D_PDX, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_WEBCAM_RST, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_WEBCAM_STB, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(SHOOTER_U_CAM_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

#ifdef CONFIG_SP3D
static uint32_t sp3d_spi_gpio[] = {
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DO, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_DI, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CS, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA),
	GPIO_CFG(SHOOTER_U_SP3D_SPI_CLK, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),
};
#endif

static void config_gpio_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("[CAM] %s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

static int shooter_u_config_camera_on_gpios(void)
{

	if (system_rev == 0x80) {
		config_gpio_table(camera_on_gpio_table_liteon,
			ARRAY_SIZE(camera_on_gpio_table_liteon));
	} else {
		config_gpio_table(camera_on_gpio_table_sp3d,
			ARRAY_SIZE(camera_on_gpio_table_sp3d));
	}

	return 0;
}

static void shooter_u_config_camera_off_gpios(void)
{
	if (system_rev == 0x80) {
		config_gpio_table(camera_off_gpio_table_liteon,
			ARRAY_SIZE(camera_off_gpio_table_liteon));
	} else {
		config_gpio_table(camera_off_gpio_table_sp3d,
			ARRAY_SIZE(camera_off_gpio_table_sp3d));
	}
	gpio_set_value(SHOOTER_U_SP3D_SPI_DO, 0);
	gpio_set_value(SHOOTER_U_SP3D_SPI_CS, 0);
	gpio_set_value(SHOOTER_U_SP3D_SPI_CLK, 0);
	gpio_set_value(SHOOTER_U_SP3D_MCLK, 0);
	gpio_set_value(SHOOTER_U_CAM_SEL, 0);
}

#ifdef CONFIG_QS_S5K4E1
static int shooter_u_qs_s5k4e1_vreg_on(void)
{
	int rc = 0;
	pr_info("[CAM] %s %x\n", __func__, system_rev);
	mdelay(50);
		
	rc = camera_sensor_power_enable("8058_l15", 2800000, &shooter_u_reg_8058_l15);
	pr_info("[CAM] sensor_power_enable(\"8058_l15\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8058_l15\", 2.8V) FAILED %d\n", rc);
		goto init_fail;
	}
	udelay(50);
		
	if (system_rev == 0x80) {
		rc = camera_sensor_power_enable_8901("8901_lvs3", &shooter_u_reg_8901_lvs3);
		pr_info("[CAM] sensor_power_enable(\"8901_lvs3\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8901_lvs3\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
		mdelay(10);

		rc = camera_sensor_power_enable_8901("8901_lvs2", &shooter_u_reg_8901_lvs2);
		pr_info("[CAM] sensor_power_enable(\"8901_lvs2\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8901_lvs2\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
	} else {
		rc = camera_sensor_power_enable("8058_l9", 1800000, &shooter_u_reg_8058_l9);
		pr_info("[CAM] sensor_power_enable(\"8058_l9\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8058_l9\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
		
		rc = camera_sensor_power_enable("8058_l8", 1800000, &shooter_u_reg_8058_l8);
		pr_info("[CAM] sensor_power_enable(\"8058_l8\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8058_l8\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
	}
	udelay(50);
	
	rc = camera_sensor_power_enable("8058_l10", 2800000, &shooter_u_reg_8058_l10);
	pr_info("[CAM] sensor_power_enable(\"8058_l10\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8058_l10\", 2.8V) FAILED %d\n", rc);
		goto init_fail;
	}
	udelay(50);
	
	gpio_set_value(SHOOTER_U_S5K4E1_INTB, 1);
	gpio_set_value(SHOOTER_U_S5K4E1_PD, 1);
	gpio_set_value(SHOOTER_U_S5K4E1_VCM_PD, 1);
	
	shooter_u_config_camera_on_gpios();
	
	rc = gpio_request(SHOOTER_U_SP3D_MCLK, "CAM_SEL");
	if (rc < 0)
	{
		pr_err("[CAM] GPIO (%d) request fail\n", SHOOTER_U_SP3D_MCLK);
		goto init_fail;
	}
	gpio_direction_output(SHOOTER_U_SP3D_MCLK, 1);
	gpio_free(SHOOTER_U_SP3D_MCLK);

init_fail:
	return rc;
}

static int shooter_u_qs_s5k4e1_vreg_off(void)
{
	int rc = 0;
	pr_info("[CAM] %s\n", __func__, system_rev);
	gpio_set_value(SHOOTER_U_S5K4E1_INTB, 0);
	gpio_set_value(SHOOTER_U_S5K4E1_VCM_PD, 0);
	gpio_set_value(SHOOTER_U_S5K4E1_PD, 0);
	
	rc = camera_sensor_power_disable(shooter_u_reg_8058_l10);
	pr_info("[CAM] camera_sensor_power_disable(\"8058_l10\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_disable(\"8058_l10\") FAILED %d\n", rc);
		goto init_fail;
	}
	udelay(50);

	if (system_rev == 0x80) {
		rc = camera_sensor_power_disable(shooter_u_reg_8901_lvs2);
		pr_info("[CAM] camera_sensor_power_disable(\"8901_lvs2\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8901_lvs2\") FAILED %d\n", rc);
			goto init_fail;
		}
		mdelay(10);

		rc = camera_sensor_power_disable(shooter_u_reg_8901_lvs3);
		pr_info("[CAM] camera_sensor_power_disable(\"8901_lvs3\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8901_lvs3\") FAILED %d\n", rc);
			goto init_fail;
		}
	} else {
		rc = camera_sensor_power_disable(shooter_u_reg_8058_l9);
		pr_info("[CAM] camera_sensor_power_disable(\"8058_l9\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8058_l9\") FAILED %d\n", rc);
			goto init_fail;
		}

		rc = camera_sensor_power_disable(shooter_u_reg_8058_l8);
		pr_info("[CAM] camera_sensor_power_disable(\"8058_l8\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8058_l8\") FAILED %d\n", rc);
			goto init_fail;
		}
	}

	if (!(system_rev == 0x80) {
		rc = camera_sensor_power_disable(shooter_u_reg_8058_l15);
		pr_info("[CAM] camera_sensor_power_disable(\"8058_l15\", 2.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8058_l15\") FAILED %d\n", rc);
			goto init_fail;
		}
		udelay(50);
	}

	shooter_u_config_camera_off_gpios();

	pr_info("[CAM] Doing clk switch to sleep state\n");
	rc = gpio_request(SHOOTER_U_SP3D_MCLK, "CAM_SEL");
	if (rc < 0)
	{
		pr_err("[CAM] GPIO (%d) request fail\n", SHOOTER_U_SP3D_MCLK);
		goto init_fail;
	}
	gpio_direction_output(SHOOTER_U_SP3D_MCLK, 0);
	gpio_free(SHOOTER_U_SP3D_MCLK);

init_fail:
		return rc;
}

#ifdef CONFIG_QS_S5K4E1_ACT
static struct i2c_board_info qs_s5k4e1_actuator_i2c_info = {
	I2C_BOARD_INFO("qs_s5k4e1_act", 0x11),
};

static struct msm_actuator_info qs_s5k4e1_actuator_info = {
	.board_info     = &qs_s5k4e1_actuator_i2c_info,
	.bus_id         = MSM_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = SHOOTER_U_GPIO_CAM_VCM_PD,
};
#endif

static struct msm_camera_sensor_platform_info sensor_qs_s5k4e1_board_info = {
	.mount_angle = 90,
	.mirror_flip = CAMERA_SENSOR_MIRROR_FLIP,
	.sensor_reset_enable = 1,
	.sensor_reset	= SHOOTER_U_S5K4E1_PD,
};

static struct camera_flash_cfg msm_camera_sensor_qs_s5k4e1_flash_cfg = {
	.low_temp_limit		= 10,
	.low_cap_limit		= 15,
};

static struct msm_camera_sensor_flash_data flash_qs_s5k4e1 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src	= &msm_flash_src,
#endif
};

static struct msm_camera_sensor_info msm_camera_sensor_qs_s5k4e1_data = {
	.sensor_name	= "qs_s5k4e1",
	.camera_power_on = shooter_u_qs_s5k4e1_vreg_on,
	.camera_power_off = shooter_u_qs_s5k4e1_vreg_off,
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_qs_s5k4e1,
	.sensor_platform_info = &sensor_qs_s5k4e1_board_info,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
#ifdef CONFIG_QS_S5K4E1_ACT
	.actuator_info = &qs_s5k4e1_actuator_info,
#endif
	.use_rawchip = RAWCHIP_DISABLE,
	.flash_cfg = &msm_camera_sensor_qs_s5k4e1_flash_cfg,
};
#endif

#ifdef CONFIG_SP3D
static int shooter_u_sp3d_vreg_on(void)
{
	int rc = 0;
	pr_info("[CAM] %s %d\n", __func__, system_rev);

	if (system_rev == 0x80) {
		rc = camera_sensor_power_enable_8901("8901_lvs2", &shooter_u_reg_8901_lvs2);
		pr_info("[CAM] sensor_power_enable(\"8901_lvs2\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8901_lvs2\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
		udelay(26);

		rc = camera_sensor_power_enable_8901("8901_lvs3", &shooter_u_reg_8901_lvs3);
		pr_info("[CAM] sensor_power_enable(\"8901_lvs3\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8901_lvs3\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
	} else {
		rc = camera_sensor_power_enable("8058_l8", 1800000, &shooter_u_reg_8058_l8);
		pr_info("[CAM] sensor_power_enable(\"8058_l8\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8058_l8\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
		udelay(26);

		rc = camera_sensor_power_enable("8058_l9", 1800000, &shooter_u_reg_8058_l9);
		pr_info("[CAM] sensor_power_enable(\"8058_l9\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8058_l9\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
	}
	
	gpio_set_value(SHOOTER_U_SP3D_CORE_GATE, 1);
	gpio_set_value(SHOOTER_U_SP3D_SYS_RST, 1);
	gpio_set_value(SHOOTER_U_SP3D_PDX, 1);
	gpio_set_value(SHOOTER_U_SP3D_GATE, 1);
	
	rc = camera_sensor_power_enable("8058_l15", 2800000, &shooter_u_reg_8058_l15);
	pr_info("[CAM] sensor_power_enable(\"8058_l15\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8058_l15\", 2.8V) FAILED %d\n", rc);
		goto init_fail;
	}

	rc = camera_sensor_power_enable("8058_l10", 2800000, &shooter_u_reg_8058_l10);
	pr_info("[CAM] sensor_power_enable(\"8058_l10\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8058_l10\", 2.8V) FAILED %d\n", rc);
		goto init_fail;
	}
	
	gpio_tlmm_config(sp3d_spi_gpio[0], GPIO_CFG_ENABLE);
	gpio_tlmm_config(sp3d_spi_gpio[1], GPIO_CFG_ENABLE);
	gpio_tlmm_config(sp3d_spi_gpio[2], GPIO_CFG_ENABLE);
	gpio_tlmm_config(sp3d_spi_gpio[3], GPIO_CFG_ENABLE);
	
	shooter_u_config_camera_on_gpios();
	
	rc = gpio_request(SHOOTER_U_SP3D_MCLK, "CAM_SEL");
	if (rc < 0)
	{
		pr_err("[CAM] GPIO (%d) request fail\n", SHOOTER_U_SP3D_MCLK);
		goto init_fail;
	}
	gpio_direction_output(SHOOTER_U_SP3D_MCLK, 1);
	gpio_free(SHOOTER_U_SP3D_MCLK);

init_fail:
	return rc;
}

static int shooter_u_sp3d_vreg_off(void)
{
	int rc = 0;
	pr_info("[CAM] %s\n", __func__);
	gpio_set_value(SHOOTER_U_SP3D_PDX, 0);

	rc = camera_sensor_power_disable(shooter_u_reg_8058_l10);
	pr_info("[CAM] camera_sensor_power_disable(\"8058_l10\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_disable(\"8058_l10\") FAILED %d\n", rc);
		goto init_fail;
	}
	udelay(10);
	
	if (!(system_rev == 0x80) {
		rc = camera_sensor_power_disable(shooter_u_reg_8058_l15);
		pr_info("[CAM] camera_sensor_power_disable(\"8058_l15\", 2.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8058_l15\") FAILED %d\n", rc);
			goto init_fail;
		}
		udelay(10);
	}

	if (system_rev == 0x80)
		rc = camera_sensor_power_disable(shooter_u_reg_8901_lvs3);
		pr_info("[CAM] camera_sensor_power_disable(\"8901_lvs3\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8901_lvs3\") FAILED %d\n", rc);
			goto init_fail;
		}
	else
		rc = camera_sensor_power_disable(shooter_u_reg_8058_l9);
		pr_info("[CAM] camera_sensor_power_disable(\"8058_l9\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8058_l9\") FAILED %d\n", rc);
			goto init_fail;
		}
		
	gpio_set_value(SHOOTER_U_SP3D_SYS_RST, 0);
	gpio_set_value(SHOOTER_U_SP3D_CORE_GATE, 0);
	gpio_set_value(SHOOTER_U_SP3D_GATE, 0);
	
	if (system_rev == 0x80)
		rc = camera_sensor_power_disable(shooter_u_reg_8901_lvs2);
		pr_info("[CAM] camera_sensor_power_disable(\"8901_lvs2\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8901_lvs2\") FAILED %d\n", rc);
			goto init_fail;
		}
	else
		rc = camera_sensor_power_disable(shooter_u_reg_8058_l8);
		pr_info("[CAM] camera_sensor_power_disable(\"8058_l8\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8058_l8\") FAILED %d\n", rc);
			goto init_fail;
		}
		
	shooter_u_config_camera_off_gpios();

	pr_info("[CAM] Doing clk switch to sleep state\n");
	rc = gpio_request(SHOOTER_U_SP3D_MCLK, "CAM_SEL");
	if (rc < 0)
	{
		pr_err("[CAM] GPIO (%d) request fail\n", SHOOTER_U_SP3D_MCLK);
		goto init_fail;
	}
	gpio_direction_output(SHOOTER_U_SP3D_MCLK, 0);
	gpio_free(SHOOTER_U_SP3D_MCLK);

init_fail:
		return rc;
}

#ifdef CONFIG_SP3D_ACT
static struct i2c_board_info sp3d_actuator_i2c_info = {
	I2C_BOARD_INFO("sp3d_act", 0x18),
};

static struct msm_actuator_info sp3d_actuator_info = {
	.board_info     = &sp3d_actuator_i2c_info,
	.bus_id         = MSM_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = SHOOTER_U_GPIO_CAM_VCM_PD,
};
#endif

static struct msm_camera_sensor_platform_info sensor_sp3d_board_info = {
	.mount_angle = 90,
	.mirror_flip = CAMERA_SENSOR_MIRROR_FLIP,
	.sensor_reset_enable = 1,
	.sensor_reset	= SHOOTER_U_SP3D_SYS_RST,
};

static struct camera_flash_cfg msm_camera_sensor_sp3d_flash_cfg = {
	.low_temp_limit		= 10,
	.low_cap_limit		= 15,
};

static struct msm_camera_sensor_flash_data flash_sp3d = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src	= &msm_flash_src,
#endif
};

static struct msm_camera_sensor_info msm_camera_sensor_sp3d_data = {
	.sensor_name	= "sp3d",
	.camera_power_on = shooter_u_sp3d_vreg_on,
	.camera_power_off = shooter_u_sp3d_vreg_off,
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_sp3d,
	.sensor_platform_info = &sensor_sp3d_board_info,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_3D,
#ifdef CONFIG_SP3D_ACT
	.actuator_info = &sp3d_actuator_info,
#endif
	.use_rawchip = RAWCHIP_DISABLE,
	.flash_cfg = &msm_camera_sensor_sp3d_flash_cfg,
};

struct platform_device msm_camera_sensor_sp3d = {
	.name	= "msm_camera_sp3d",
	.dev	= {
		.platform_data = &msm_camera_sensor_sp3d_data,
	},
};

#endif

#ifdef CONFIG_S5K6AAFX
static int shooter_u_s5k6aafx_vreg_on(void)
{
	int rc = 0;
	pr_info("[CAM] %s\n", __func__);
	
	rc = camera_sensor_power_enable("8901_l6", 2850000, &shooter_u_reg_8901_l6);
	pr_info("[CAM] sensor_power_enable(\"8901_l6\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8901_l6\", 2.8V) FAILED %d\n", rc);
		goto init_fail;
	}
	mdelay(5);
	
	if (system_rev == 0x80) {
		rc = camera_sensor_power_enable_8901("8901_lvs2", &shooter_u_reg_8901_lvs2);
		pr_info("[CAM] sensor_power_enable(\"8901_lvs2\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8901_lvs2\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
		mdelay(10);

		rc = camera_sensor_power_enable_8901("8901_lvs3", &shooter_u_reg_8901_lvs3);
		pr_info("[CAM] sensor_power_enable(\"8901_lvs3\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8901_lvs3\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
	} else {
		rc = camera_sensor_power_enable("8058_l8", 1800000, &shooter_u_reg_8058_l8);
		pr_info("[CAM] sensor_power_enable(\"8058_l8\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8058_l8\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
		mdelay(5);

		rc = camera_sensor_power_enable("8058_l9", 1800000, &shooter_u_reg_8058_l9);
		pr_info("[CAM] sensor_power_enable(\"8058_l9\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_enable(\"8058_l9\", 1.8V) FAILED %d\n", rc);
			goto init_fail;
		}
		mdelay(1);
	}
	
	shooter_u_config_camera_on_gpios();
	
	rc = gpio_request(SHOOTER_U_CAM_SEL, "CAM_SEL");
	if (rc < 0)
	{
		pr_err("[CAM] GPIO (%d) request fail\n", SHOOTER_U_CAM_SEL);
		goto init_fail;
	}
	gpio_direction_output(SHOOTER_U_CAM_SEL, 1);
	gpio_free(SHOOTER_U_CAM_SEL);

init_fail:
	return rc;
}

static int shooter_u_s5k6aafx_vreg_off(void)
{
	int rc = 0;
	pr_info("[CAM] %s\n", __func__);
	
	if (system_rev == 0x80) {
		rc = camera_sensor_power_disable(shooter_u_reg_8901_lvs3);
		pr_info("[CAM] camera_sensor_power_disable(\"8901_lvs3\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8901_lvs3\") FAILED %d\n", rc);
			goto init_fail;
		}
		mdelay(1);
		
		rc = camera_sensor_power_disable(shooter_u_reg_8901_lvs2);
		pr_info("[CAM] camera_sensor_power_disable(\"8901_lvs2\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8901_lvs2\") FAILED %d\n", rc);
			goto init_fail;
		}
		mdelay(1);
	} else {
		rc = camera_sensor_power_disable(shooter_u_reg_8058_l9);
		pr_info("[CAM] camera_sensor_power_disable(\"8058_l9\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8058_l9\") FAILED %d\n", rc);
			goto init_fail;
		}
		mdelay(1);

		rc = camera_sensor_power_disable(shooter_u_reg_8058_l8);
		pr_info("[CAM] camera_sensor_power_disable(\"8058_l8\", 1.8V) == %d\n", rc);
		if (rc < 0) {
			pr_err("[CAM] sensor_power_disable(\"8058_l8\") FAILED %d\n", rc);
			goto init_fail;
		}
		mdelay(1);
	}
	
	rc = camera_sensor_power_disable(shooter_u_reg_8901_l6);
	pr_info("[CAM] camera_sensor_power_disable(\"8901_l6\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_disable(\"8901_l6\") FAILED %d\n", rc);
		goto init_fail;
	}
	
	shooter_u_config_camera_off_gpios();

	pr_info("[CAM] Doing clk switch to sleep state\n");
	rc = gpio_request(SHOOTER_U_CAM_SEL, "CAM_SEL");
	if (rc < 0)
	{
		pr_err("[CAM] GPIO (%d) request fail\n", SHOOTER_U_CAM_SEL);
		goto init_fail;
	}
	gpio_direction_output(SHOOTER_U_CAM_SEL, 0);
	gpio_free(SHOOTER_U_CAM_SEL);

init_fail:
		return rc;
}

static struct msm_camera_sensor_platform_info sensor_s5k6aafx_board_info = {
	.mount_angle = 270,
	.mirror_flip = CAMERA_SENSOR_NONE,
	.sensor_reset_enable = 1,
	.sensor_reset = SHOOTER_U_WEBCAM_RST,
};

static struct msm_camera_sensor_flash_data flash_s5k6aafx = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k6aafx_data = {
	.sensor_name	= "s5k6aafx",
	.sensor_reset	= SHOOTER_U_WEBCAM_RST,
	.camera_power_on = shooter_u_s5k6aafx_vreg_on,
	.camera_power_off = shooter_u_s5k6aafx_vreg_off,
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_s5k6aafx,
	.sensor_platform_info = &sensor_s5k6aafx_board_info,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.use_rawchip = RAWCHIP_DISABLE, 
};
#endif

#ifdef CONFIG_I2C
static struct i2c_board_info msm_camera_boardinfo[] = {
#ifdef CONFIG_S5K3H1GX
	{
	I2C_BOARD_INFO("qs_s5k4e1", 0x20),
	.platform_data = &msm_camera_sensor_qs_s5k4e1_data,
	},
#endif
#ifdef CONFIG_MT9V113
	{
	I2C_BOARD_INFO("s5k6aafx", 0x78 >> 1),
	.platform_data = &msm_camera_sensor_s5k6aafx_data,
	},
	{
	I2C_BOARD_INFO("s5k6aafx", 0x5a >> 1),
	},
#endif
};
#endif

void __init msm8x60_init_cam(void)
{
	pr_info("%s", __func__);

	i2c_register_board_info(MSM_GSBI4_QUP_I2C_BUS_ID,
		msm_camera_boardinfo,
		ARRAY_SIZE(msm_camera_boardinfo));

	platform_device_register(&msm_camera_server);

	platform_device_register(&msm_device_csic0);
	platform_device_register(&msm_device_csic1);
	platform_device_register(&msm_device_vfe);
	platform_device_register(&msm_device_vpe);
}
#endif
