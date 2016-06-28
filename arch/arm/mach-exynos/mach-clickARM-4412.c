/*
 * linux/arch/arm/mach-exynos4/mach-clickARM-4412.c
 *
 *
 * Based on mach-hkdh4412.c
 *
 * Copyright (c) 2012 AgreeYa Mobility Co., Ltd.
 *		http://www.agreeyamobility.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/i2c/pca953x.h>
#include <linux/input.h>
#include <linux/io.h>
#include <linux/mfd/max77686.h>
#include <linux/mmc/host.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/pwm_backlight.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>
#include <linux/serial_core.h>
#include <linux/platform_data/s3c-hsotg.h>
#include <linux/platform_data/i2c-s3c2410.h>
#include <linux/platform_data/usb-ehci-s5p.h>
#include <linux/platform_data/usb-exynos.h>
#include <linux/platform_data/usb3503.h>
#include <linux/delay.h>
#include <linux/lcd.h>
#include <linux/clk.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/reboot.h>
#include <linux/i2c/tsc2007.h>

#include <asm/mach/arch.h>
#include <asm/hardware/gic.h>
#include <asm/mach-types.h>

#include <plat/backlight.h>
#include <plat/clock.h>
#include <plat/cpu.h>
#include <plat/devs.h>
#include <plat/gpio-cfg.h>
#include <plat/keypad.h>
#include <plat/mfc.h>
#include <plat/regs-serial.h>
#include <plat/sdhci.h>
#include <plat/fb.h>
#include <plat/hdmi.h>

#include <video/platform_lcd.h>
#include <video/samsung_fimd.h>
#include <linux/platform_data/spi-s3c64xx.h>

#include <mach/gpio.h>
#include <mach/map.h>
#include <mach/regs-pmu.h>
#include <mach/dwmci.h>
#include <drm/exynos_drm.h>

#include "common.h"
#include "pmic-77686.h"

/*VELO INCLUDES*/
#include <linux/pwm_backlight.h>

#include <linux/w1-gpio.h>
#define VELO_FAN_INT    EXYNOS4212_GPM3(0) /*IRQ XEINT8*/


extern void exynos4_setup_dwmci_cfg_gpio(struct platform_device *dev, int width);

/* Following are default values for UCON, ULCON and UFCON UART registers */
#define CLICKARM4412_UCON_DEFAULT	(S3C2410_UCON_TXILEVEL |	\
				 S3C2410_UCON_RXILEVEL |	\
				 S3C2410_UCON_TXIRQMODE |	\
				 S3C2410_UCON_RXIRQMODE |	\
				 S3C2410_UCON_RXFIFO_TOI |	\
				 S3C2443_UCON_RXERR_IRQEN)

#define CLICKARM4412_ULCON_DEFAULT	S3C2410_LCON_CS8

#define CLICKARM4412_UFCON_DEFAULT	(S3C2410_UFCON_FIFOMODE |	\
				 S5PV210_UFCON_TXTRIG4 |	\
				 S5PV210_UFCON_RXTRIG4)

static struct s3c2410_uartcfg clickarm4412_uartcfgs[] __initdata = {
	[0] = {
		.hwport		= 0,
		.flags		= 0,
		.ucon		= CLICKARM4412_UCON_DEFAULT,
		.ulcon		= CLICKARM4412_ULCON_DEFAULT,
		.ufcon		= CLICKARM4412_UFCON_DEFAULT,
	},
	[1] = {
		.hwport		= 1,
		.flags		= 0,
		.ucon		= CLICKARM4412_UCON_DEFAULT,
		.ulcon		= CLICKARM4412_ULCON_DEFAULT,
		.ufcon		= CLICKARM4412_UFCON_DEFAULT,
	},
	[2] = {
		.hwport		= 2,
		.flags		= 0,
		.ucon		= CLICKARM4412_UCON_DEFAULT,
		.ulcon		= CLICKARM4412_ULCON_DEFAULT,
		.ufcon		= CLICKARM4412_UFCON_DEFAULT,
	},
	[3] = {
		.hwport		= 3,
		.flags		= 0,
		.ucon		= CLICKARM4412_UCON_DEFAULT,
		.ulcon		= CLICKARM4412_ULCON_DEFAULT,
		.ufcon		= CLICKARM4412_UFCON_DEFAULT,
	},
};

/*DS2782 BATTERY FUEL GAUGE*/
#if defined(CONFIG_BATTERY_DS2782) || defined(CONFIG_BATTERY_DS2782_MODULE)
#include <linux/ds2782_battery.h>
#define DS2786_RSNS    18 /* Constant sense resistor value */

struct ds278x_platform_data ds278x_pdata = {
	.rsns = DS2786_RSNS,
};
#endif
/*FAN54040 CONFIGURATION PLATDATA*/
/*FAN CHARGER INTERRUPT INIT FAN54:XEINT8*/
//static int fan54_chr_init(void)
//{
//	/* FAN54_INT: XEINT_8 *///EXYNOS4212_GPM3(0)
//	gpio_request(VELO_FAN_INT, "FAN54_INT");
//	s3c_gpio_cfgpin(VELO_FAN_INT, S3C_GPIO_SFN(0xf));
//	s3c_gpio_setpull(VELO_FAN_INT, S3C_GPIO_PULL_NONE);
//}
/*END OF FAN54040 CONFIGURATION PLATDATA*/

/*TMP103 THERMOMETER MODEL A/B PLATDATA*/
//#if defined(CONFIG_TMP103_SENSOR) || defined(CONFIG_TMP103_SENSOR_MODULE)
//static struct temp_sensor_pdata tmp103_sensor_pdata = {
//	.source_domain = "pcb_case",
//	.average_number = 20,
//	.report_delay_ms = 250,
//};
//
//static struct platform_device bowser_case_sensor_device = {
//	.name	= "tmp103_temp_sensor",
//	.id	= -1,
//	.dev	= {
//	.platform_data = &tmp103_sensor_pdata,
//	},
//};
//#endif

/* MPU9250 CONFIG */
#if defined(CONFIG_SENSOR_MPU9250) || defined(CONFIG_SENSOR_MPU9250_MODULE) /*GPA1CON5 // XEINT30 */
static struct mpu_platform_data gyro_platform_data = {
        .int_config  = 0x00,
        .level_shifter = 0,
        .orientation = {   1,  0,  0,
                           0,  1,  0,
                           0,  0, 1 },
        .sec_slave_type = SECONDARY_SLAVE_TYPE_COMPASS,
        .sec_slave_id   = COMPASS_ID_AK8963,
        .secondary_i2c_addr = 0x0C,
        .secondary_orientation = { 0,  1, 0,
                                   -1, 0,  0,
                                   0,  0,  1 },
};
#endif

/*USB 3505 SMC CCOnfiguration*/
#if defined(CONFIG_USB_HSIC_USB3503)
static struct usb3503_platform_data usb3503_pdata = {
	.initial_mode	= 	USB3503_MODE_HUB,
	.ref_clk		= 	USB3503_REFCLK_26M,
	.gpio_intn		= EXYNOS4_GPX3(0),
	.gpio_connect	= EXYNOS4_GPX3(4),
	.gpio_reset		= EXYNOS4_GPX3(5),
};
#endif
/*END OF USB 3505 SMC CCOnfiguration*/

/*touchscreen config tsc2007 XE_INT22*/
#if defined(CONFIG_TOUCHSCREEN_TSC2007) || defined(CONFIG_TOUCHSCREEN_TSC2007_MODULE)
#include <linux/i2c/tsc2007.h>
#define VELO_TS_INT      EXYNOS4_GPX2(6) /*IRQ_EINT22*/
static int TSC2007_get_pendown_state(void)
{
	int val = 0;
	gpio_free(VELO_TS_INT);
	gpio_request(VELO_TS_INT, NULL);
	gpio_direction_input(VELO_TS_INT);
       
	val = gpio_get_value(VELO_TS_INT);
       
	gpio_free(VELO_TS_INT);
	gpio_request(VELO_TS_INT, NULL);
       
	return val ? 0 : 1;
	/*return !gpio_get_value(VELO_TS_INT);*/
}
/*TOUCHSCREEN INTERRUPT INIT TOUCH_INT:XEINT22*/
static int TSC2007_tsp_init(void)
{
	/* TOUCH_INT: XEINT_22 */
	gpio_request(VELO_TS_INT, "TOUCH_INT");
	s3c_gpio_cfgpin(VELO_TS_INT, S3C_GPIO_SFN(0xf));
	s3c_gpio_setpull(VELO_TS_INT, S3C_GPIO_PULL_UP);

	return 0;
}

struct tsc2007_platform_data tsc2007_info = {
	.get_pendown_state	= TSC2007_get_pendown_state,
	.init_platform_hw	= TSC2007_tsp_init,
	.model 		= 2007,	/* 2007. */
	.x_plate_ohms	= 300, /* must be non-zero value */
	.max_rt		= 1<<12, /* max. resistance above which samples are ignored */
	.poll_delay	= 5, /* delay (in ms) after pen-down event
				     before polling starts */
	.poll_period 	= 5,/* time (in ms) between samples */
	.fuzzx		= 64, 	/* fuzz factor for X, Y and pressure axes */
	.fuzzy		= 64,
	.fuzzz		= 64,
};
#endif
/*END OF touchscreen config tsc2007 XE_INT22*/
/*Audio Codec MAX98090 Configuration*/
#if defined(CONFIG_SND_SOC_MAX98090)
#include <sound/max98090.h>
static struct max98090_pdata max98090 = {
	.digmic_left_mode	= 0,
	.digmic_right_mode	= 0,
	.digmic_3_mode		= 0,
	.digmic_4_mode		= 0,
};
#endif

/*END OF Audio Codec MAX98090 Configuration*/
/*Devices Conected on I2C BUS 0 LISTED ABOVE*/
static struct i2c_board_info clickarm4412_i2c_devs0[] __initdata = {
	{
		I2C_BOARD_INFO("max77686", (0x12 >> 1)),
		.platform_data	= &exynos4_max77686_info,
	},
#if defined(CONFIG_USB_HSIC_USB3503)
	{
		I2C_BOARD_INFO("usb3503", (0x08)),
		.platform_data  = &usb3503_pdata,
	},
#endif
};
/*END OF Devices Conected on I2C BUS 0 LISTED ABOVE*/

static struct i2c_board_info clickarm4412_i2c_devs1[] __initdata = {
#if defined(CONFIG_TOUCHSCREEN_TSC2007)
        {
                I2C_BOARD_INFO("tsc2007", 0x48),
                .platform_data  = &tsc2007_info,
                .irq            = IRQ_EINT(22),
        },
#endif

#if defined(CONFIG_SND_SOC_MAX98090)
	{
		I2C_BOARD_INFO("max98090", (0x20>>1)),
		.platform_data  = &max98090,
		.irq		= IRQ_EINT(0),
	},
#endif

	/*UNCOMMENT WHEN READY*/
//#if defined(CONFIG_TMP103_SENSOR)
//	{
//		I2C_BOARD_INFO("tmp103_temp_sensor", 0x71),
//		.platform_data = &tmp103_sensor_pdata,
//	},
//#endif

//#if defined(CONFIG_LPS25H)
//	{
//		I2C_BOARD_INFO("lps25h", ????),/*CONFIG DIRECCTION*/
//		.platform_data  = &?????????,/*CONFIG PDATA*/
//		.irq		= ????????, /*xe.int25*/
//	},
//#endif
	/*UNCOMMENT WHEN READY*/
#if defined(CONFIG_MAX44005)
	{
		I2C_BOARD_INFO("max44005", 0x44), /*CONFIG DIRECCTION SET DEFAULT*/
		.platform_data  = &????????, /*CONFIG PDATA*/

	},
#endif
};
/*END OF Devices Conected on I2C BUS 1 LISTED ABOVE*/

/* I2C2 bus GPIO-Bitbanging */
#define		GPIO_I2C2_SDA	EXYNOS4_GPA0(6)
#define		GPIO_I2C2_SCL	EXYNOS4_GPA0(7)
static struct 	i2c_gpio_platform_data 	i2c2_gpio_platdata = {
	.sda_pin = GPIO_I2C2_SDA,
	.scl_pin = GPIO_I2C2_SCL,
	.udelay  = 5,
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.scl_is_output_only = 0
};

static struct 	platform_device 	gpio_device_i2c2 = {
	.name 	= "i2c-gpio",
	.id  	= 2,    // adepter number
	.dev.platform_data = &i2c2_gpio_platdata,
};

/* Odroid-O2 schematics show the DDC of the remote HDMI device connected to
 * I2C2. HDMI specs state that DDC always sits at bus address 0x50. */
static struct i2c_board_info clickarm4412_i2c_devs2[] __initdata = {
	{
			/* nothing here yet */
		//I2C_BOARD_INFO("s5p_ddc", 0x50),
	},
};

static struct i2c_board_info clickarm4412_i2c_devs3[] __initdata = {
#if defined(CONFIG_DS2782)
	{
		I2C_BOARD_INFO("ds2782", 0x34),
		.platform_data  = &ds278x_pdata,
	},
#endif
#if defined(CONFIG_FAN54040)
/**/
	{
		I2C_BOARD_INFO("fan54040", 0x6B),
		.platform_data  = &tsc2007_info,
		.irq		= VELO_FAN_INT,/*xeint8 // GPM3CON0 CHAGE STATUS // GPM3CON1 DISABLE CHARGE*/
	},
#endif
#if defined(CONFIG_SENSOR_MPU9250)
	{
	     I2C_BOARD_INFO("mpu9250", 0x68),
         .irq = (IH_GPIO_BASE + MPUIRQ_GPIO),
	     .platform_data = &gyro_platform_data,
	 },
#endif
};

#if 0
static struct i2c_board_info clickarm4412_i2c_devs7[] __initdata = {
		{
	/* nothing here yet */
		}
};
#endif

/* for u3 I/O shield board */
#define		GPIO_I2C4_SDA	EXYNOS4_GPB(0) /* GPIO-PIN 200 */
#define		GPIO_I2C4_SCL	EXYNOS4_GPB(1) /* GPIO-PIN 199 */

static struct 	i2c_gpio_platform_data 	i2c4_gpio_platdata = {
	.sda_pin = GPIO_I2C4_SDA,
	.scl_pin = GPIO_I2C4_SCL,
	.udelay  = 0,
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.scl_is_output_only = 0
};

static struct 	platform_device 	gpio_device_i2c4 = {
	.name 	= "i2c-gpio",
	.id  	= 4,    // adepter number
	.dev.platform_data = &i2c4_gpio_platdata,
};

#if defined(CONFIG_W1_MASTER_GPIO) || defined(CONFIG_W1_MASTER_GPIO_MODULE)
/* Enables W1 on Pin 6 of the I/-Header of U3 */
/* Breaks support for I/O shield board */
#define               GPIO_W1         EXYNOS4_GPX1(5) /* GPIO-PIN 204 */

static struct w1_gpio_platform_data w1_gpio_pdata = {
       .pin = GPIO_W1,
       .is_open_drain = 0,
};

static struct platform_device clickarm_w1_device = {
   .name  = "w1-gpio",
   .id    = -1,
   .dev.platform_data      = &w1_gpio_pdata,
};
#endif

#if defined(CONFIG_GPIO_PCA953X)
static struct pca953x_platform_data clickarm_gpio_expander_pdata = {
	.gpio_base	= EXYNOS4_GPIO_END,
};
#endif

static struct i2c_board_info clickarm4412_i2c_devs4[] __initdata = {
#if defined(CONFIG_SENSORS_BH1780)
	{
		I2C_BOARD_INFO("bh1780", 0x29),
	},
#endif
#if defined(CONFIG_GPIO_PCA953X)
	{
		I2C_BOARD_INFO("tca6416", 0x20),
		.platform_data 	= &clickarm_gpio_expander_pdata,
	},
#endif
};

//#if defined(CONFIG_CLICKARM_OTHERS)
//static struct gpio_led clickarm4412_gpio_config[] = {
//        {
//                .name			= "led1",
//                .default_trigger	= "heartbeat",
//                .gpio			= EXYNOS4_GPC1(0),
//                .active_low		= 1,
//        },
//};
//#else
//static struct gpio_config clickarm4412_gpio_config[] = {
//	{
//		/* TL_VELO button */
//		gpio_request_one(EXYNOS4_GPF2(5), GPIOF_INIT_HIGH	, "GPX1");
//		gpio_free(EXYNOS4_GPF2(5));
//
//		/* BR_VELO button */
//		gpio_free(EXYNOS4_GPJ0(1));
//		gpio_request_one(EXYNOS4_GPJ0(1), GPIOF_INIT_HIGH	, "GPX1");
//		/* BL_VELO button */
//		gpio_free(EXYNOS4_GPJ1(1));
//		gpio_request_one(EXYNOS4_GPJ1(1), GPIOF_INIT_HIGH	, "GPX1");
//		/* GPS PowerON/OFF */
//		gpio_free(EXYNOS4212_GPM4(2));
//		gpio_request_one(EXYNOS4212_GPM4(2), GPIOF_OUT_INIT_LOW	, "GPX1");
//		/*GPS_ON*/
//		gpio_free(EXYNOS4212_GPM4(2));
//		gpio_set_value(EXYNOS4212_GPM4(2), 1);
//		mdelay(100);
//		gpio_set_value(EXYNOS4212_GPM4(2), 0);
//		/* GPRS POWER */
//		gpio_free(EXYNOS4212_GPM1(1));
//		gpio_request_one(EXYNOS4212_GPM1(1), GPIOF_INIT_HIGH	, "GPX1");
//		/*GPRS_ON*/
//		gpio_free(EXYNOS4212_GPM0(4));
//		gpio_request_one(EXYNOS4212_GPM0(4), GPIOF_OUT_INIT_LOW	, "GPX1");
//		/* to poweron gprs need to drive EXYNOS4212_GPM0(4) to HIG (1) over 1500ms*/
//
//	},
//
//};
//#endif
//
//static struct gpio_led_platform_data clickarm4412_gpio_config_info = {
//	.leds		= clickarm4412_gpio_config,
//	.num_leds	= ARRAY_SIZE(clickarm4412_gpio_config),
//};
//
//static struct platform_device clickarm4412_config_gpio = {
//	.name	= "config-gpio",
//	.id	= -1,
//	.dev	= {
//		.platform_data	= &clickarm4412_gpio_config_info,
//	},
//};
/* LCD Backlight data tps611xx PWM_platform_data*/
/*BACKLIGTH INIT*/

static struct samsung_bl_gpio_info clickarm4412_bl_gpio_info = {
	.no = EXYNOS4X12_GPM1(5),
	.func = S3C_GPIO_SFN(2),
};

static struct platform_pwm_backlight_data clickarm4412_bl_data = {
	.pwm_id = 1,
	.pwm_period_ns  = 1000,
};

static struct pwm_lookup clickarm4412_pwm_lookup[] = {
	PWM_LOOKUP("s3c24xx-pwm.1", 0, "pwm-backlight.0", NULL),
};
/* END OF LCD Backlight data tps611xx PWM_platform_data*/
/*Define VELO display with DRM */
#if defined(CONFIG_LCD_T55149GD030J) && defined(CONFIG_DRM_EXYNOS_FIMD)
static struct exynos_drm_fimd_pdata drm_fimd_pdata = {
	.panel = {
		.timing = {
			.left_margin 	= 9,
			.right_margin 	= 9,
			.upper_margin 	= 5,
			.lower_margin 	= 5,
			.hsync_len 	= 2,
			.vsync_len 	= 2,
			.xres 		= 240,
			.yres 		= 400,
		},
	},
	.vidcon0	= VIDCON0_VIDOUT_RGB | VIDCON0_PNRMODE_RGB,
	.vidcon1	= VIDCON1_INV_HSYNC | VIDCON1_INV_VSYNC | 
				  VIDCON1_INV_VCLK | VIDCON1_INV_VDEN,
	.default_win 	= 0,
	.bpp 		= 32,
};
	
static void lcd_t55149gd030j_set_power(struct plat_lcd_data *pd,
				   unsigned int power)
{

	if (power) {
		gpio_set_value(EXYNOS4X12_GPM1(5),1);
	} else {
		gpio_set_value(EXYNOS4X12_GPM1(5),0);
	}
	gpio_free(EXYNOS4X12_GPM1(5));

}

static struct plat_lcd_data clickarm4412_lcd_t55149gd030j_data = {
	.set_power	= lcd_t55149gd030j_set_power,
	
};

static struct platform_device clickarm4412_lcd_t55149gd030j = {
	.name	= "platform-lcd",
	.dev	= {
		.parent		= &s5p_device_fimd0.dev,
		.platform_data	= &clickarm4412_lcd_t55149gd030j_data,
	},
};
#endif
/*END OF Define VELO display with DRM */

/* GPIO KEYS KEYBOARD*/
static struct gpio_keys_button clickarm4412_gpio_keys_tables[] = {
	{
		.code			= BTN_A,
		.gpio			= EXYNOS4X12_GPM3(7),	/* VELO SIDE BUTTON TR POWERON */
		.desc			= "KEY_POWER",
		.type			= EV_SW,
		.active_low		= 1,
		.wakeup			= 1,
	},
	{
		.code			= BTN_B,
		.gpio			= EXYNOS4_GPF2(5), /* VELO SIDE BUTTON TL */
		.desc			= "TL_BUTTON",
		.type			= EV_SW,
		.active_low		= 1,
		.wakeup			= 1,
	},
	{
		.code			= BTN_C,
		.gpio			= EXYNOS4_GPJ0(1), /* VELO FRONT BUTTON BR */
		.desc			= "BR_BUTTON",
		.type			= EV_SW,
		.active_low		= 1,
		.wakeup			= 1,
	},
	{
		.code			= BTN_X,
		.gpio			= EXYNOS4_GPJ1(1), /* VELO FRONT BUTTON BL */
		.desc			= "BL_BUTTON",
		.type			= EV_SW,
		.active_low		= 1,
		.wakeup			= 1,
	},
};

static struct gpio_keys_platform_data clickarm4412_gpio_keys_data = {
	.buttons	= clickarm4412_gpio_keys_tables,
	.nbuttons	= ARRAY_SIZE(clickarm4412_gpio_keys_tables),
};

static struct platform_device clickarm4412_gpio_keys = {
	.name	= "gpio-keys",
	.dev	= {
		.platform_data	= &clickarm4412_gpio_keys_data,
	},
};
/*END OF GPIO KEYS KEYBOARD*/

#if defined(CONFIG_SND_SOC_HKDK_MAX98090)
static struct platform_device hardkernel_audio_device = {
	.name	= "hkdk-snd-max98090",
	.id	= -1,
};
#endif

/* USB EHCI */
static struct s5p_ehci_platdata clickarm4412_ehci_pdata;

static void __init clickarm4412_ehci_init(void)
{
	struct s5p_ehci_platdata *pdata = &clickarm4412_ehci_pdata;

	s5p_ehci_set_platdata(pdata);
}

/* USB OHCI */
static struct exynos4_ohci_platdata clickarm4412_ohci_pdata;

static void __init clickarm4412_ohci_init(void)
{
	struct exynos4_ohci_platdata *pdata = &clickarm4412_ohci_pdata;

	exynos4_ohci_set_platdata(pdata);
}

/* USB OTG */
static struct s3c_hsotg_plat clickarm4412_hsotg_pdata;

#ifdef CONFIG_USB_EXYNOS_SWITCH
static struct s5p_usbswitch_platdata clickarm4412_usbswitch_pdata;

static void __init clickarm4412_usbswitch_init(void)
{
	struct s5p_usbswitch_platdata *pdata = &clickarm4412_usbswitch_pdata;
	int err;

	pdata->gpio_host_detect = EXYNOS4_GPX3(1); /* low active */
	err = gpio_request_one(pdata->gpio_host_detect, GPIOF_IN, "HOST_DETECT");
	if (err) {
		printk(KERN_ERR "failed to request gpio_host_detect\n");
		return;
	}

	s3c_gpio_cfgpin(pdata->gpio_host_detect, S3C_GPIO_SFN(0xF));
	s3c_gpio_setpull(pdata->gpio_host_detect, S3C_GPIO_PULL_UP);
	gpio_free(pdata->gpio_host_detect);

	pdata->gpio_device_detect = EXYNOS4_GPX1(6); /* high active */
	err = gpio_request_one(pdata->gpio_device_detect, GPIOF_IN, "DEVICE_DETECT");
	if (err) {
		printk(KERN_ERR "failed to request gpio_host_detect for\n");
		return;
	}

	s3c_gpio_cfgpin(pdata->gpio_device_detect, S3C_GPIO_SFN(0xF));
	s3c_gpio_setpull(pdata->gpio_device_detect, S3C_GPIO_PULL_DOWN);
	gpio_free(pdata->gpio_device_detect);

	pdata->gpio_host_vbus = EXYNOS4_GPL2(0);
	err = gpio_request_one(pdata->gpio_host_vbus, GPIOF_OUT_INIT_LOW, "HOST_VBUS_CONTROL");
	if (err) {
		printk(KERN_ERR "failed to request gpio_host_vbus\n");
		return;
	}

	s3c_gpio_setpull(pdata->gpio_host_vbus, S3C_GPIO_PULL_NONE);
	gpio_free(pdata->gpio_host_vbus);

	pdata->ohci_dev = &exynos4_device_ohci.dev;
	pdata->ehci_dev = &s5p_device_ehci.dev;
	pdata->s3c_hsotg_dev = &s3c_device_usb_hsotg.dev;

	s5p_usbswitch_set_platdata(pdata);
}
#endif

/* SDCARD */
static struct s3c_sdhci_platdata clickarm4412_hsmmc2_pdata __initdata = {
	.max_width	= 4,
	.host_caps	= MMC_CAP_4_BIT_DATA |
			MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED,
	.cd_type	= S3C_SDHCI_CD_NONE,
};
/* WIFI SDIO */
static struct s3c_sdhci_platdata clickarm4412_hsmmc3_pdata __initdata = {
		.max_width		= 4,
		.host_caps		= MMC_CAP_4_BIT_DATA |
					MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED,
		.cd_type		= S3C_SDHCI_CD_EXTERNAL,
		};
/* DWMMC */
static int clickarm4412_dwmci_get_bus_wd(u32 slot_id)
{
       return 8;
}

static int clickarm4412_dwmci_init(u32 slot_id, irq_handler_t handler, void *data)
{
       return 0;
}

static struct dw_mci_board clickarm4412_dwmci_pdata = {
	.num_slots			= 1,
	.quirks				= DW_MCI_QUIRK_BROKEN_CARD_DETECTION | DW_MCI_QUIRK_HIGHSPEED,
	.caps				= MMC_CAP_UHS_DDR50 | MMC_CAP_1_8V_DDR | MMC_CAP_8_BIT_DATA | MMC_CAP_CMD23,
	.fifo_depth			= 0x80,
	.bus_hz				= 104 * 1000 * 1000,
	.detect_delay_ms	= 200,
	.init				= clickarm4412_dwmci_init,
	.get_bus_wd			= clickarm4412_dwmci_get_bus_wd,
//	.cfg_gpio			= exynos4_setup_dwmci_cfg_gpio,
};

static struct resource tmu_resource[] = {
	[0] = {
		.start = EXYNOS4_PA_TMU,
		.end = EXYNOS4_PA_TMU + 0x0100,
		.flags = IORESOURCE_MEM,
	},
	[1] = { 
		.start = EXYNOS4_IRQ_TMU_TRIG0,
		.end = EXYNOS4_IRQ_TMU_TRIG0,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device clickarm4412_tmu = {
	.id = -1,
	.name = "exynos5250-tmu",
	.num_resources = ARRAY_SIZE(tmu_resource),
	.resource = tmu_resource,
};

#if defined(CONFIG_CLICKARM_OTHERS_FAN)
#include	<linux/platform_data/clickarm_others_fan.h>
struct clickarm_fan_platform_data clickarm_fan_pdata = {
        .pwm_gpio = EXYNOS4_GPD0(0),
        .pwm_func = S3C_GPIO_SFN(2),

        .pwm_id = 0,
        .pwm_periode_ns = 20972,        // Freq 22KHz,
        .pwm_duty = 255,                // max=255,
        .pwm_start_temp = 50,           // default 50,
};

static struct platform_device clickarm_fan = {
        .name   = "clickarm-fan",
        .id     = -1,  
        .dev.platform_data = &clickarm_fan_pdata,
};
#endif


#if defined(CONFIG_LCD_T55149GD030J)


static int lcd_power_on(struct lcd_device *ld, int enable)
{	

	if (enable) {
		gpio_set_value(EXYNOS4X12_GPM1(5),1);
	} else {
		gpio_set_value(EXYNOS4X12_GPM1(5),0);
	}

	return 1;
}


static int lcd_cfg_gpio(void)
{
	int err = 0;
	
	printk("lcd_cfg_gpio()***!!!!**********\n");	
	/*Power control*/
//	gpio_free(EXYNOS4X12_GPM1(5));
//	gpio_request_one(EXYNOS4X12_GPM1(5), GPIOF_OUT_INIT_HIGH, "GPM1");
//	gpio_free(EXYNOS4X12_GPM1(5));

	/* LCD _CS */
	gpio_free(EXYNOS4_GPB(5));
	err=gpio_request_one(EXYNOS4_GPB(5), GPIOF_OUT_INIT_HIGH, "GPB");	
	if (err) {
		printk(KERN_ERR "failed to request GPC1 for "
				"lcd SPI CS control\n");
		return err;
	}
	s3c_gpio_cfgpin(EXYNOS4_GPB(5), S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(EXYNOS4_GPB(5), S3C_GPIO_PULL_NONE);
	gpio_free(EXYNOS4_GPB(5));
	gpio_set_value(EXYNOS4_GPB(5), 1);
	
	
	
	/* LCD_SCLK */
	gpio_free(EXYNOS4_GPB(4));
	err=gpio_request_one(EXYNOS4_GPB(4), GPIOF_OUT_INIT_HIGH, "GPB");	
	if (err) {
		printk(KERN_ERR "failed to request GPC1 for "
				"lcd SPI CLK control\n");
		return err;
	}
	s3c_gpio_cfgpin(EXYNOS4_GPB(4), S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(EXYNOS4_GPB(4), S3C_GPIO_PULL_NONE);
	gpio_free(EXYNOS4_GPB(4));
	gpio_set_value(EXYNOS4_GPB(4), 1);
	
	
	/* LCD_SDI */
	gpio_free(EXYNOS4_GPB(7));
	err=gpio_request_one(EXYNOS4_GPB(7), GPIOF_OUT_INIT_HIGH, "GPB");	
	if (err) {
		printk(KERN_ERR "failed to request GPC1 for "
				"lcd SPI SDI control\n");
		return err;
	}
	s3c_gpio_cfgpin(EXYNOS4_GPB(7), S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(EXYNOS4_GPB(7), S3C_GPIO_PULL_NONE);
	gpio_free(EXYNOS4_GPB(7));
	gpio_set_value(EXYNOS4_GPB(7), 1);
	

	gpio_free(EXYNOS4_GPC1(2));
	err = gpio_request_one(EXYNOS4_GPC1(2), GPIOF_OUT_INIT_HIGH, "GPC1");
	if (err) {
		printk(KERN_ERR "failed to request GPC1 for "
				"lcd reset control\n");
		return err;
	}
	
	s3c_gpio_cfgpin(EXYNOS4_GPC1(2), S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(EXYNOS4_GPC1(2), S3C_GPIO_PULL_NONE);
	//gpio_free(EXYNOS4_GPC1(2));

	/* LCD FMARK */
	gpio_free(EXYNOS4_GPX1(5));
	gpio_request_one(EXYNOS4_GPX1(5), GPIOF_OUT_INIT_LOW, "GPX1");	
	//s3c_gpio_cfgpin(EXYNOS4_GPX1(5), S3C_GPIO_OUTPUT);
	s3c_gpio_cfgpin(EXYNOS4_GPX1(5), S3C_GPIO_INPUT);
	s3c_gpio_setpull(EXYNOS4_GPX1(5), S3C_GPIO_PULL_NONE);
	gpio_free(EXYNOS4_GPX1(5));
	//gpio_set_value(EXYNOS4_GPX1(5), 0);

	/* LCD IM1 */
	gpio_free(EXYNOS4_GPX1(7));
	gpio_request_one(EXYNOS4_GPX1(7), GPIOF_OUT_INIT_HIGH, "GPX1");	
	s3c_gpio_cfgpin(EXYNOS4_GPX1(7), S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(EXYNOS4_GPX1(7), S3C_GPIO_PULL_NONE);
	gpio_free(EXYNOS4_GPX1(7));
	gpio_set_value(EXYNOS4_GPX1(7), 0); //SPI mode after reset

	/* LCD IM0 */
	gpio_free(EXYNOS4_GPX2(0));
	gpio_request_one(EXYNOS4_GPX2(0), GPIOF_OUT_INIT_HIGH, "GPX2");	
	s3c_gpio_cfgpin(EXYNOS4_GPX2(0), S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(EXYNOS4_GPX2(0), S3C_GPIO_PULL_NONE);
	gpio_free(EXYNOS4_GPX2(0));
	gpio_set_value(EXYNOS4_GPX2(0), 0); //SPI ID
	
	return 1;
}

static int reset_lcd(struct lcd_device *ld)
{
	int err = 0;
	
	lcd_cfg_gpio();
	
	printk("LCD reset***!!!!**********\n");
	
	gpio_set_value(EXYNOS4_GPC1(2), 1);
	mdelay(10);

	gpio_set_value(EXYNOS4_GPC1(2), 0);
	mdelay(10);

	gpio_set_value(EXYNOS4_GPC1(2), 1);

	//gpio_free(EXYNOS4_GPC1(2));

	return err;
}

static struct lcd_platform_data t55149gd030j_platform_data = {
	.reset			= reset_lcd,
	.power_on		= lcd_power_on,
	.lcd_enabled		= 0,
	.reset_delay		= 100,	/* 100ms */
};

#define		LCD_BUS_NUM	1
#define		DISPLAY_CS	EXYNOS4_GPB(5)
#define		DISPLAY_CLK	EXYNOS4_GPB(4)
#define		DISPLAY_SI	EXYNOS4_GPB(7)

// SPI1
#if 0
static struct s3c64xx_spi_csinfo spi1_csi = {
		.fb_delay = 0x2,
		.line = EXYNOS4_GPB(5),
};
#endif

static struct spi_board_info spi1_board_info[] __initdata = {
	{
		.modalias = "t55149gd030j-spi",
		.platform_data		= (void *)&t55149gd030j_platform_data,
		.max_speed_hz = 1200000, // 1.2 mhz
		.bus_num = LCD_BUS_NUM,
		.chip_select = 0,
		.mode = SPI_MODE_3,
		.controller_data =(void *)DISPLAY_CS,// &spi1_csi,
	},
};

static struct spi_gpio_platform_data t55149gd030j_spi_gpio_data = {
	.sck	= DISPLAY_CLK,
	.mosi	= DISPLAY_SI,
	.miso	= -1,
	.num_chipselect = 1,
};

static struct platform_device clickarm_lcd_spi = {
	.name			= "spi_gpio",
	.id 			= LCD_BUS_NUM,
	.dev	= {
		.parent		= &s5p_device_fimd0.dev,
		.platform_data	= &t55149gd030j_spi_gpio_data,//&spi1_board_info,//
	},
};
#endif

static struct platform_device *clickarm4412_devices[] __initdata = {
	&s3c_device_hsmmc2,
	&s3c_device_hsmmc3,
	&s3c_device_i2c0,
	&s3c_device_i2c1,
	&gpio_device_i2c2,
	&s3c_device_i2c3,
	&s3c_device_i2c4,
#if defined(CONFIG_W1_MASTER_GPIO) || defined(CONFIG_W1_MASTER_GPIO_MODULE)
        &clickarm_w1_device,
#endif
	&s3c_device_i2c7,
	&s3c_device_rtc,
	&s3c_device_usb_hsotg,
	&s3c_device_wdt,
	&s5p_device_ehci,
#ifdef CONFIG_SND_SAMSUNG_I2S
	&exynos4_device_i2s0,
#endif
	&s5p_device_fimc0,
	&s5p_device_fimc1,
	&s5p_device_fimc2,
	&s5p_device_fimc3,
	&s5p_device_fimc_md,
	&s5p_device_fimd0,
	&s5p_device_mfc,
	&s5p_device_mfc_l,
	&s5p_device_mfc_r,
	&s5p_device_g2d,
	&s5p_device_jpeg,
	&mali_gpu_device,
#if defined(CONFIG_S5P_DEV_TV)
	&s5p_device_hdmi,
	&s5p_device_cec,
	&s5p_device_i2c_hdmiphy,
	&s5p_device_mixer,
	&hdmi_fixed_voltage,
#endif
	&exynos4_device_ohci,
	&exynos_device_dwmci,
//	&clickarm4412_leds_gpio,
#if defined(CONFIG_LCD_T55149GD030J) && !defined(CONFIG_CLICKARM_OTHERS) && defined(CONFIG_DRM_EXYNOS_FIMD)
	&clickarm4412_lcd_t55149gd030j,
#endif
	&clickarm4412_gpio_keys,
	&samsung_asoc_idma,
#if defined(CONFIG_SND_SOC_HKDK_MAX98090)
	&hardkernel_audio_device,
#endif
#if defined(CONFIG_EXYNOS_THERMAL)
	&clickarm4412_tmu,
#endif
#if defined(CONFIG_CLICKARM_OTHERS_PWM_BL)
	&s3c_device_timer[1],
	&clickarm_pwm_bl,
#endif

#if defined(CONFIG_LCD_T55149GD030J)
	&clickarm_lcd_spi,
#else
	&s3c64xx_device_spi1,
#endif
#if defined(CONFIG_USB_EXYNOS_SWITCH)
	&s5p_device_usbswitch,
#endif
};

#if defined(CONFIG_S5P_DEV_TV)
static struct s5p_platform_cec hdmi_cec_data __initdata = {

};
#endif

static void __init clickarm4412_map_io(void)
{
	clk_xusbxti.rate = 24000000;

	exynos_init_io(NULL, 0);
	s3c24xx_init_clocks(clk_xusbxti.rate);
	s3c24xx_init_uarts(clickarm4412_uartcfgs, ARRAY_SIZE(clickarm4412_uartcfgs));
}

static void __init clickarm4412_reserve(void)
{
	s5p_mfc_reserve_mem(0x43000000, 64 << 20, 0x51000000, 64 << 20);
}

#if defined(CONFIG_S5P_DEV_TV)
/* I2C module and id for HDMIPHY */
static struct i2c_board_info hdmiphy_info = {
	I2C_BOARD_INFO("s5p_hdmiphy", 0x38),
};
#endif



static void __init clickarm4412_gpio_init(void)
{
	/* Peripheral power enable (P3V3) */
	gpio_request_one(EXYNOS4_GPA1(1), GPIOF_OUT_INIT_HIGH, "p3v3_en");

	/* Power on/off button */
	s3c_gpio_cfgpin(EXYNOS4X12_GPM3(7), S3C_GPIO_SFN(0xF));	/* VELO SIDE BUTTON TR POWERON */
	s3c_gpio_setpull(EXYNOS4X12_GPM3(7), S3C_GPIO_PULL_NONE);
	
	/* TR/TL */
	gpio_request_one(EXYNOS4_GPF2(5), GPIOF_IN, "TL");
        s3c_gpio_cfgpin(EXYNOS4_GPF2(5), S3C_GPIO_INPUT );
        s3c_gpio_setpull(EXYNOS4_GPF2(5), S3C_GPIO_PULL_UP);
	gpio_free(EXYNOS4_GPF2(5));

	gpio_request_one(EXYNOS4X12_GPM3(7), GPIOF_IN, "TR");
        s3c_gpio_cfgpin(EXYNOS4X12_GPM3(7), S3C_GPIO_INPUT );
        s3c_gpio_setpull(EXYNOS4X12_GPM3(7), S3C_GPIO_PULL_UP);
        gpio_free(EXYNOS4X12_GPM3(7));

	/* BR/BL */
        gpio_request_one(EXYNOS4_GPJ0(1), GPIOF_IN, "BR");
        s3c_gpio_cfgpin(EXYNOS4_GPJ0(1), S3C_GPIO_INPUT );
        s3c_gpio_setpull(EXYNOS4_GPJ0(1), S3C_GPIO_PULL_UP);
        gpio_free(EXYNOS4_GPJ0(1));

	gpio_request_one(EXYNOS4_GPJ1(1), GPIOF_IN, "BL");
        s3c_gpio_cfgpin(EXYNOS4_GPJ1(1), S3C_GPIO_INPUT );
        s3c_gpio_setpull(EXYNOS4_GPJ1(1), S3C_GPIO_PULL_UP);
        gpio_free(EXYNOS4_GPJ1(1));

}

static void clickarm4412_power_off(void)
{
	pr_emerg("Bye...\n");

	writel(0x5200, S5P_PS_HOLD_CONTROL);
	while (1) {
		pr_emerg("%s : should not reach here!\n", __func__);
		msleep(1000);
	}
}

static int clickarm4412_reboot_notifier(struct notifier_block *this, unsigned long code, void *_cmd) {
	pr_emerg("exynos4-reboot: Notifier called\n");

	__raw_writel(0, S5P_INFORM4);

        // eMMC HW_RST  
        gpio_request(EXYNOS4_GPK1(2), "GPK1");
        gpio_direction_output(EXYNOS4_GPK1(2), 0);
        msleep(150);
        gpio_direction_output(EXYNOS4_GPK1(2), 1);
        gpio_free(EXYNOS4_GPK1(2));
	msleep(500);
        return NOTIFY_DONE;
}	


static struct notifier_block clickarm4412_reboot_notifier_nb = {
	.notifier_call = clickarm4412_reboot_notifier,
};

static void __init clickarm4412_machine_init(void)
{

	clickarm4412_gpio_init();

	/* Register power off function */
	pm_power_off = clickarm4412_power_off;

	s3c_i2c0_set_platdata(NULL);
	i2c_register_board_info(0, clickarm4412_i2c_devs0,
				ARRAY_SIZE(clickarm4412_i2c_devs0));

	s3c_i2c1_set_platdata(NULL);

	i2c_register_board_info(1, clickarm4412_i2c_devs1,
				ARRAY_SIZE(clickarm4412_i2c_devs1));

	i2c_register_board_info(2, clickarm4412_i2c_devs2,
				ARRAY_SIZE(clickarm4412_i2c_devs2));

	s3c_i2c3_set_platdata(NULL);
	i2c_register_board_info(3, clickarm4412_i2c_devs3,
				ARRAY_SIZE(clickarm4412_i2c_devs3));

	s3c_i2c4_set_platdata(NULL);
	i2c_register_board_info(4, clickarm4412_i2c_devs4,
				ARRAY_SIZE(clickarm4412_i2c_devs4));

#if defined(CONFIG_CLICKARM_OTHERS)
//	i2c_register_board_info(4, clickarm4412_i2c_devs4,
//				ARRAY_SIZE(clickarm4412_i2c_devs4));
#endif

	s3c_i2c7_set_platdata(NULL);
//	i2c_register_board_info(7, clickarm4412_i2c_devs7,
//				ARRAY_SIZE(clickarm4412_i2c_devs7));
/*BACKLIGTH CONFIGURATION GPIO ARRAY*/
	samsung_bl_set(&clickarm4412_bl_gpio_info, &clickarm4412_bl_data);
	pwm_add_table(clickarm4412_pwm_lookup, ARRAY_SIZE(clickarm4412_pwm_lookup));
/*SDIO_HCI CONFIGURATION ARRAY*/
	s3c_sdhci2_set_platdata(&clickarm4412_hsmmc2_pdata);
	s3c_sdhci3_set_platdata(&clickarm4412_hsmmc3_pdata);

	exynos4_setup_dwmci_cfg_gpio(NULL, MMC_BUS_WIDTH_8);
	exynos_dwmci_set_platdata(&clickarm4412_dwmci_pdata);

	clickarm4412_ehci_init();
	clickarm4412_ohci_init();
	s3c_hsotg_set_platdata(&clickarm4412_hsotg_pdata);

#ifdef CONFIG_USB_EXYNOS_SWITCH
	clickarm4412_usbswitch_init();
#endif

	//s3c64xx_spi1_set_platdata(NULL, 0, 1);
	spi_register_board_info(spi1_board_info, ARRAY_SIZE(spi1_board_info));

#if defined(CONFIG_S5P_DEV_TV)
	s5p_i2c_hdmiphy_set_platdata(NULL);
	s5p_hdmi_set_platdata(&hdmiphy_info, NULL, 0, EXYNOS4_GPX3(7));
	s5p_hdmi_cec_set_platdata(&hdmi_cec_data);
	/* FIXME: hdmiphy i2c adapter has dynamic ID, and setting it to 8 causes
	 * a failure to initialize (can't find clock?). so for now we are relying
	 * on the hdmiphy i2c adapter being dynamically assigned address 8. */
	i2c_register_board_info(8, &hdmiphy_info, 1);
#endif

#if defined(CONFIG_LCD_T55149GD030J) && !defined(CONFIG_CLICKARM_OTHERS) && defined(CONFIG_DRM_EXYNOS_FIMD)
	s5p_device_fimd0.dev.platform_data = &drm_fimd_pdata;
	exynos4_fimd0_gpio_setup_24bpp();
#endif
	platform_add_devices(clickarm4412_devices, ARRAY_SIZE(clickarm4412_devices));

	register_reboot_notifier(&clickarm4412_reboot_notifier_nb);
}

MACHINE_START(CLICKARM4412, "ClickArm4412")
	/* Maintainer: Dongjin Kim <dongjin.kim@agreeyamobiity.net> */
	.atag_offset	= 0x100,
	.smp		= smp_ops(exynos_smp_ops),
	.init_irq	= exynos4_init_irq,
	.init_early	= exynos_firmware_init,
	.map_io		= clickarm4412_map_io,
	.handle_irq	= gic_handle_irq,
	.init_machine	= clickarm4412_machine_init,
	.init_late	= exynos_init_late,
	.timer		= &exynos4_timer,
	.restart	= exynos4_restart,
	.reserve	= &clickarm4412_reserve,
MACHINE_END
