/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-5      SummerGift   first version
 */

#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

/* SPI设备驱动层结构体
*/

#include <rtthread.h>
#include "rtdevice.h"
#include <rthw.h>

#include "ch32v30x_rcc.h"
#include "ch32v30x_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

rt_err_t rt_hw_spi_device_attach(const char *bus_name, const char *device_name, GPIO_TypeDef* cs_gpiox, uint16_t cs_gpio_pin);

#ifdef __cplusplus
}
#endif

struct ch32v307_hw_spi_cs
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
};
/*片上SPI和自定义SPI总线名称*/
struct ch32v307_spi_config
{
    SPI_TypeDef *Instance;
    char *bus_name;
    IRQn_Type irq_type;
//    struct dma_config *dma_rx, *dma_tx;  //暂时先不加上dma
};
/*SPI设备的引脚号、总线名称以及设备名称。一般规范的写法是，总线名称是spix，挂载在总线上的设备名称为spixy*/
struct ch32v307_spi_device
{
    rt_uint32_t pin;
    char *bus_name;//总线名称
    char *device_name;//设备名称
};

//spi总线关于dma的配置暂时不使用
//#define SPI_USING_RX_DMA_FLAG   (1<<0)
//#define SPI_USING_TX_DMA_FLAG   (1<<1)

typedef struct __SPI_HandleTypeDef
{

  SPI_TypeDef                *Instance;      /*!< SPI registers base address               */

  SPI_InitTypeDef            Init;           /*!< SPI communication parameters             */

  uint8_t                    *pTxBuffPtr;    /*!< Pointer to SPI Tx transfer Buffer        */

  uint16_t                   TxXferSize;     /*!< SPI Tx Transfer size                     */

  volatile uint16_t          TxXferCount;    /*!< SPI Tx Transfer Counter                  */

  uint8_t                    *pRxBuffPtr;    /*!< Pointer to SPI Rx transfer Buffer        */

  uint16_t                   RxXferSize;     /*!< SPI Rx Transfer size                     */

  volatile uint16_t          RxXferCount;    /*!< SPI Rx Transfer Counter                  */


} SPI_HandleTypeDef;

/* ch32v307 spi dirver class */
struct ch32v307_spi
{
    SPI_HandleTypeDef handle;
    
    struct ch32v307_spi_config *config;
    struct rt_spi_configuration *cfg;   /*结构体是设备框架驱动层的*/
    struct rt_spi_bus spi_bus;          /*结构体是设备框架驱动层的*/
/*暂时不使用dma
 *   struct
 *   {
 *       DMA_HandleTypeDef handle_rx;
 *       DMA_HandleTypeDef handle_tx;
 *   } dma;
*/
//   rt_uint8_t spi_dma_flag;
    
};
rt_err_t rt_hw_spi_device_attach(const char *bus_name, const char *device_name, GPIO_TypeDef* cs_gpiox, uint16_t cs_gpio_pin);

#endif /*__DRV_SPI_H__ */
