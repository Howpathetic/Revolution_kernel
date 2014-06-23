/* Copyright (C) 2010-2011 HTC Corporation.
 * Copyright (c) 2014 Jaka Valenko <jaka.valenko6@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
 
#include <linux/android_pmem.h>
#include <linux/mfd/pmic8058.h>
#include <linux/delay.h>
#include <linux/pmic8058-othc.h>
#include <linux/spi/spi_aic3254.h>
#include <linux/regulator/consumer.h>

#include <mach/gpio.h>
#include <mach/dal.h>
#include <mach/tpa2051d3.h>
#include "qdsp6v2/snddev_icodec.h"
#include "qdsp6v2/snddev_ecodec.h"
#include "qdsp6v2/snddev_hdmi.h"
#include <mach/htc_acoustic_8x60.h>
#include "board-shooter_u.h"
#include <mach/qdsp6v2/audio_dev_ctl.h>

#define PM8058_GPIO_BASE					NR_MSM_GPIOS
#define PM8058_GPIO_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8058_GPIO_BASE)

#define BIT_SPEAKER		(1 << 0)
#define BIT_HEADSET		(1 << 1)
#define BIT_RECEIVER	(1 << 2)
#define BIT_FM_SPK		(1 << 3)
#define BIT_FM_HS		(1 << 4)

void shooter_u_snddev_bmic_pamp_on(int en);

static uint32_t msm_aic3254_reset_gpio[] = {
	GPIO_CFG(SHOOTER_U_AUD_CODEC_RST, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static struct mutex mic_lock;
static int curr_rx_mode;
static atomic_t aic3254_ctl = ATOMIC_INIT(0);

void shooter_u_snddev_poweramp_on(int en)
{
	pr_aud_info("%s %d\n", __func__, en);
	if (en) {
		msleep(30);
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_SPK_ENO), 1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_SPEAKER;
		msleep(5);
	} else {
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_SPK_ENO), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_SPEAKER;
	}
}

void shooter_u_snddev_usb_headset_pamp_on(int en)
{
	pr_aud_info("%s %d\n", __func__, en);
	if (en) {
	} else {
	}
}

void shooter_u_snddev_hsed_pamp_on(int en)
{
	pr_aud_info("%s %d\n", __func__, en);
	if (en) {
		msleep(30);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_HP_EN), 1);
		set_headset_amp(1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_HEADSET;
	} else {
		set_headset_amp(0);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_HP_EN), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_HEADSET;
	}
}

void shooter_u_snddev_hs_spk_pamp_on(int en)
{
	pr_aud_info("%s %d\n", __func__, en);
	shooter_u_snddev_poweramp_on(en);
	shooter_u_snddev_hsed_pamp_on(en);
}

void shooter_u_snddev_receiver_pamp_on(int en)
{
	pr_aud_info("%s %d\n", __func__, en);
	if (en) {
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_HP_EN), 1);
		set_handset_amp(1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_RECEIVER;
	} else {
		set_handset_amp(0);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_HP_EN), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_RECEIVER;
	}
}

void shooter_u_mic_enable(int en, int shift)
{
	pr_aud_info("%s: %d, shift %d\n", __func__, en, shift);

	mutex_lock(&mic_lock);

	if (en)
		pm8058_micbias_enable(OTHC_MICBIAS_2, OTHC_SIGNAL_ALWAYS_ON);
	else
		pm8058_micbias_enable(OTHC_MICBIAS_2, OTHC_SIGNAL_OFF);

	mutex_unlock(&mic_lock);
}

void shooter_u_snddev_imic_pamp_on(int en)
{
	int ret;

	pr_aud_info("%s %d\n", __func__, en);

	if (en) {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_ALWAYS_ON);
		if (ret)
			pr_aud_err("%s: Enabling int mic power failed\n", __func__);

		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_MIC_SEL), 0);

		shooter_u_snddev_bmic_pamp_on(1);
	} else {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_OFF);
		if (ret)
			pr_aud_err("%s: Enabling int mic power failed\n", __func__);

		shooter_u_snddev_bmic_pamp_on(0);
	}
}

void shooter_u_snddev_bmic_pamp_on(int en)
{
	int ret;

	pr_aud_info("%s %d\n", __func__, en);

	if (en) {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_1, OTHC_SIGNAL_ALWAYS_ON);
		if (ret)
			pr_aud_err("%s: Enabling back mic power failed\n", __func__);

		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_MIC_SEL), 0);

	} else {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_1, OTHC_SIGNAL_OFF);
		if (ret)
			pr_aud_err("%s: Enabling back mic power failed\n", __func__);

	}
}

void shooter_u_snddev_stereo_mic_pamp_on(int en)
{
	int ret;

	pr_aud_info("%s %d\n", __func__, en);

	if (en) {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_ALWAYS_ON);
		if (ret)
			pr_aud_err("%s: Enabling int mic power failed\n", __func__);

		ret = pm8058_micbias_enable(OTHC_MICBIAS_1, OTHC_SIGNAL_ALWAYS_ON);
		if (ret)
			pr_aud_err("%s: Enabling back mic power failed\n", __func__);

		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_MIC_SEL), 0);
	} else {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_OFF);
		if (ret)
			pr_aud_err("%s: Disabling int mic power failed\n", __func__);

		ret = pm8058_micbias_enable(OTHC_MICBIAS_1, OTHC_SIGNAL_OFF);
		if (ret)
			pr_aud_err("%s: Disabling back mic power failed\n", __func__);
	}
}

void shooter_u_snddev_emic_pamp_on(int en)
{
	pr_aud_info("%s %d\n", __func__, en);

	if (en) {
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_MIC_SEL), 1);
	}
}

void shooter_u_snddev_fmspk_pamp_on(int en)
{
	pr_aud_info("%s %d\n", __func__, en);

	if (en) {
		msleep(50);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_SPK_ENO), 1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_FM_SPK;
		msleep(5);
	} else {
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_SPK_ENO), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_FM_SPK;
	}
}

void shooter_u_snddev_fmhs_pamp_on(int en)
{
	pr_aud_info("%s %d\n", __func__, en);

	if (en) {
		msleep(50);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_HP_EN), 1);
		set_headset_amp(1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_FM_HS;
		msleep(5);
	} else {
		set_headset_amp(0);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_HP_EN), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_FM_HS;
	}
}

void shooter_u_voltage_on(int en)
{
}

void shooter_u_rx_amp_enable(int en)
{
	if (curr_rx_mode != 0) {
		atomic_set(&aic3254_ctl, 1);
		pr_aud_info("%s: curr_rx_mode 0x%x, en %d\n",
			__func__, curr_rx_mode, en);
		if (curr_rx_mode & BIT_SPEAKER)
			shooter_u_snddev_poweramp_on(en);
		if (curr_rx_mode & BIT_HEADSET)
			shooter_u_snddev_hsed_pamp_on(en);
		if (curr_rx_mode & BIT_RECEIVER)
			shooter_u_snddev_receiver_pamp_on(en);
		if (curr_rx_mode & BIT_FM_SPK)
			shooter_u_snddev_fmspk_pamp_on(en);
		if (curr_rx_mode & BIT_FM_HS)
			shooter_u_snddev_fmhs_pamp_on(en);
		atomic_set(&aic3254_ctl, 0);;
	}
}

int shooter_u_support_aic3254(void)
{
	return 1;
}

int shooter_u_support_back_mic(void)
{
	return 1;
}

void shooter_u_spibus_enable(int en)
{
	uint32_t msm_spi_gpio_on[] = {
		GPIO_CFG(SHOOTER_U_SPI_DO,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		GPIO_CFG(SHOOTER_U_SPI_DI,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
		GPIO_CFG(SHOOTER_U_SPI_CS,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		GPIO_CFG(SHOOTER_U_SPI_CLK, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	};

	uint32_t msm_spi_gpio_off[] = {
		GPIO_CFG(SHOOTER_U_SPI_DO,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		GPIO_CFG(SHOOTER_U_SPI_DI,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
		GPIO_CFG(SHOOTER_U_SPI_CS,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		GPIO_CFG(SHOOTER_U_SPI_CLK, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	};

	pr_debug("%s %d\n", __func__, en);
	if (en) {
		gpio_tlmm_config(msm_spi_gpio_on[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(msm_spi_gpio_on[1], GPIO_CFG_ENABLE);
		gpio_tlmm_config(msm_spi_gpio_on[2], GPIO_CFG_ENABLE);
		gpio_tlmm_config(msm_spi_gpio_on[3], GPIO_CFG_ENABLE);
	} else {
		gpio_tlmm_config(msm_spi_gpio_off[0], GPIO_CFG_DISABLE);
		gpio_tlmm_config(msm_spi_gpio_off[1], GPIO_CFG_DISABLE);
		gpio_tlmm_config(msm_spi_gpio_off[2], GPIO_CFG_DISABLE);
		gpio_tlmm_config(msm_spi_gpio_off[3], GPIO_CFG_DISABLE);
	}
	mdelay(1);
}

void shooter_u_reset_3254(void)
{
	gpio_tlmm_config(msm_aic3254_reset_gpio[0], GPIO_CFG_ENABLE);
	gpio_set_value(SHOOTER_U_AUD_CODEC_RST, 0);
	mdelay(1);
	gpio_set_value(SHOOTER_U_AUD_CODEC_RST, 1);
}

static struct q6v2audio_analog_ops ops = {
	.speaker_enable	        = shooter_u_snddev_poweramp_on,
	.headset_enable	        = shooter_u_snddev_hsed_pamp_on,
	.handset_enable	        = shooter_u_snddev_receiver_pamp_on,
	.headset_speaker_enable	= shooter_u_snddev_hs_spk_pamp_on,
	.int_mic_enable         = shooter_u_snddev_imic_pamp_on,
	.back_mic_enable        = shooter_u_snddev_bmic_pamp_on,
	.ext_mic_enable         = shooter_u_snddev_emic_pamp_on,
	.stereo_mic_enable      = shooter_u_snddev_stereo_mic_pamp_on,
	.fm_headset_enable      = shooter_u_snddev_fmhs_pamp_on,
	.fm_speaker_enable      = shooter_u_snddev_fmspk_pamp_on,
	.usb_headset_enable     = shooter_u_snddev_usb_headset_pamp_on,
	.voltage_on				= shooter_u_voltage_on,
};

static struct aic3254_ctl_ops cops = {
	.rx_amp_enable        = shooter_u_rx_amp_enable,
	.reset_3254           = shooter_u_reset_3254,
	.spibus_enable        = shooter_u_spibus_enable,
};

static struct acoustic_ops acoustic = {
	.enable_mic_bias = shooter_u_mic_enable,
	.support_aic3254 = shooter_u_support_aic3254,
	.support_back_mic = shooter_u_support_back_mic,
};

void shooter_u_aic3254_set_mode(int config, int mode)
{
	aic3254_set_mode(config, mode);
}

static struct q6v2audio_aic3254_ops aops = {
       .aic3254_set_mode = shooter_u_aic3254_set_mode,
};

void shooter_u_audio_gpios_init(void)
{
	pr_aud_info("%s\n", __func__);
	gpio_request(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_SPK_ENO), "AUD_SPK_ENO");
	gpio_request(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_HP_EN), "AUD_HP_EN");
	gpio_request(PM8058_GPIO_PM_TO_SYS(SHOOTER_U_AUD_MIC_SEL) , "AUD_MIC_SEL");
}

void __init shooter_u_audio_init(void)
{
	mutex_init(&mic_lock);

	pr_aud_info("%s\n", __func__);
	htc_8x60_register_analog_ops(&ops);
	acoustic_register_ops(&acoustic);
	htc_8x60_register_aic3254_ops(&aops);
	msm_set_voc_freq(8000, 8000);	
	aic3254_register_ctl_ops(&cops);

	shooter_u_reset_3254();
    gpio_tlmm_config(
        GPIO_CFG(SHOOTER_U_AUD_CDC_LDO_SEL, 0, GPIO_CFG_OUTPUT,
            GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_DISABLE);
			
	shooter_u_audio_gpios_init();
}
