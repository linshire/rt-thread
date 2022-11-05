/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-23     liYony       first version
 */

#include <rtthread.h>
#include <rtdevice.h>

#include "sensor_melexis_mlx9039x.h"

    rt_device_t lishire_dev;

int rt_hw_mlx9039x_port(void)
{
    struct rt_sensor_config cfg;
    
    cfg.intf.dev_name = "i2c1";
    //cfg.intf.user_data = (void *)MLX9039x_ADDR_DEFAULT;
    cfg.irq_pin.pin = RT_PIN_NONE;

    rt_hw_mlx9039x_init("mlx", &cfg);
    return 0;
}
INIT_APP_EXPORT(rt_hw_mlx9039x_port);

#define SPIBUSNAME  "spi1"
char name[RT_NAME_MAX];
rt_uint8_t w25x_read_id = 0x90;
rt_uint8_t id[5] = {0X00,0XAB,0XAA,0XAA,0XAA};
struct rt_spi_device linshire_spi_device;


int main(void)
{
    while (1)
    {
        rt_spi_send(&linshire_spi_device,id,5);
        rt_thread_mdelay(1000);
    }
}


