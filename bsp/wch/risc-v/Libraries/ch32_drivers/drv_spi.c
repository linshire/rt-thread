/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-5      SummerGift   first version
 * 2018-12-11     greedyhao    Porting for stm32f7xx
 * 2019-01-03     zylx         modify DMA initialization and spixfer function
 * 2020-01-15     whj4674672   Porting for stm32h7xx
 * 2020-06-18     thread-liu   Porting for stm32mp1xx
 * 2020-10-14     Dozingfiretruck   Porting for stm32wbxx
 */
/*SPI�豸������*/

#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"


#ifdef RT_USING_SPI

#if defined(BSP_USING_SPI1) || defined(BSP_USING_SPI2) || defined(BSP_USING_SPI3)

#include "drv_spi.h"
#include <string.h>


//#define DRV_DEBUG
#define LOG_TAG              "drv.spi"
#include <drv_log.h>

//3 �� SPI �ӿڣ�SPI2,SPI3 ���� I2S2,I2S3��
enum
{
#ifdef BSP_USING_SPI1
    SPI1_INDEX,
#endif
#ifdef BSP_USING_SPI2
    SPI2_INDEX,
#endif
#ifdef BSP_USING_SPI3
    SPI3_INDEX,
#endif
};

static struct ch32v307_spi_config spi_config[] =
{
#ifdef BSP_USING_SPI1
        {                                                       \
            .Instance = SPI1,                                   \
            .bus_name = "spi1",                                 \
            .irq_type = SPI1_IRQn,                              \
        },
#endif

#ifdef BSP_USING_SPI2
        {                                                       \
            .Instance = SPI2,                                   \
            .bus_name = "spi2",                                 \
            .irq_type = SPI2_IRQn,                              \
        },
#endif

#ifdef BSP_USING_SPI3
        {                                                       \
            .Instance = SPI3,                                   \
            .bus_name = "spi3",                                 \
            .irq_type = SPI3_IRQn,                              \
        }
#endif
};



#endif /* BSP_SPI3_TX_USING_DMA */

static struct ch32v307_spi spi_bus_obj[sizeof(spi_config) / sizeof(spi_config[0])] = {0};

static rt_uint32_t ch32v307_spi_clock_get(SPI_TypeDef *spix);
static void ch32v307_spi_clock_and_io_init(SPI_TypeDef *spix);
static rt_uint8_t SPIx_ReadWriteByte(SPI_TypeDef *Instance, rt_uint8_t TxData);
static rt_err_t SPI_TransmitReceive(SPI_TypeDef *Instance, uint8_t *send_buf, uint8_t *recv_buf, uint16_t send_length);
static rt_err_t SPI_Transmit(SPI_TypeDef *Instance, uint8_t *send_buf, uint16_t send_length);
static rt_err_t SPI_Receive(SPI_TypeDef *Instance, uint8_t *recv_buf,uint16_t send_length);

/*
*SPI����GPIO�ĳ�ʼ������
*/
static void ch32v307_spi_clock_and_io_init(SPI_TypeDef *spix)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    if (spix == SPI1)
    {
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA, ENABLE );
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( GPIOA, &GPIO_InitStructure );

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( GPIOA, &GPIO_InitStructure );

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( GPIOA, &GPIO_InitStructure );
    }

    if (spix == SPI2)
    {
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( GPIOB, &GPIO_InitStructure );

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( GPIOB, &GPIO_InitStructure );

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( GPIOB, &GPIO_InitStructure );
    }

    if (spix == SPI3)
    {
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI3, ENABLE );
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( GPIOB, &GPIO_InitStructure );

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( GPIOB, &GPIO_InitStructure );

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( GPIOB, &GPIO_InitStructure );
    }
}

/*
*SPI���߻�ȡ��Ƭ��Ƶ��-��������SPI��max_hz
*/
static rt_uint32_t ch32v307_spi_clock_get(SPI_TypeDef *spix)
{
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    if (spix == SPI1)
    {
        return RCC_Clocks.PCLK2_Frequency;
    }

    if (spix == SPI2)
    {
        return RCC_Clocks.PCLK1_Frequency;
    }

    if (spix == SPI3)
    {
        return RCC_Clocks.PCLK1_Frequency;
    }

    return RCC_Clocks.PCLK2_Frequency;
}


/*
 *spix read write byte
 * */
static rt_uint8_t SPIx_ReadWriteByte(SPI_TypeDef *Instance, rt_uint8_t TxData)
{
    uint8_t i=0;
    while (SPI_I2S_GetFlagStatus(Instance, SPI_I2S_FLAG_TXE) == RESET)
    {
        i++;
        if (i > 200) return 0;
    }
    SPI_I2S_SendData(Instance, TxData);

    i=0;
    while (SPI_I2S_GetFlagStatus(Instance, SPI_I2S_FLAG_RXNE) == RESET)
    {
        i++;
        if(i > 200) return 0;
    }
    return SPI_I2S_ReceiveData(Instance);
}

/*
 *spi transmit and receive
 * */
static rt_err_t SPI_TransmitReceive(SPI_TypeDef *Instance, uint8_t *send_buf, uint8_t *recv_buf, uint16_t send_length)
{
    uint16_t i=0;
    for(i = 0; i < send_length; i++)
    {
        recv_buf[i] = SPIx_ReadWriteByte(Instance, send_buf[i]);
    }
    return RT_EOK;
}

/*
 *spi transmit
 * */
static rt_err_t SPI_Transmit(SPI_TypeDef *Instance, uint8_t *send_buf, uint16_t send_length)
{
    uint16_t i=0;
    for(i = 0; i < send_length; i++)
    {
        SPIx_ReadWriteByte(Instance, send_buf[i]);
    }
    return RT_EOK;
}


/*
 *spi  receive
 * */
static rt_err_t SPI_Receive(SPI_TypeDef *Instance, uint8_t *recv_buf,uint16_t send_length)
{
    uint16_t i=0;
    for(i = 0; i < send_length; i++)
    {
        recv_buf[i] = SPIx_ReadWriteByte(Instance, 0xFF);  /*��������Ϊ0xff ��ʱ��ʾΪ������*/
    }
    return RT_EOK;
}


/*ch32v307_spi_init���� ch32v307_spi�ṹ���е�SPI_HandleTypeDef�ͱ���handle���������SPI�ĳ�ʼ��������*/
static rt_err_t ch32v307_spi_init(struct ch32v307_spi *spi_drv, struct rt_spi_configuration *cfg)
{

    RT_ASSERT(spi_drv != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    /*��ȡspix��handle���*/
    //    SPI_HandleTypeDef *spi_handle = &spi_drv->handle;

    SPI_HandleTypeDef *spi_handle = &spi_drv->handle;

    /*����������ģʽ*/
    if (cfg->mode & RT_SPI_SLAVE)
    {
        spi_handle->Init.SPI_Mode = SPI_Mode_Slave;
    }
    else
    {
        spi_handle->Init.SPI_Mode = SPI_Mode_Master;
    }
    /*����SPI���ߵ����ݷ��򣺵��ߡ�˫��*/
    if (cfg->mode & RT_SPI_3WIRE)  //�˴���Щ��һ�� ���ǼĴ����Ĳ�����ѡ��һ�������鿴���̸���
    {
        spi_handle->Init.SPI_Direction = SPI_Direction_1Line_Rx;
    }
    else
    {
        spi_handle->Init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    }

    /*SPI���ߵ����ݿ��*/
    if (cfg->data_width <= 8)
    {
        spi_handle->Init.SPI_DataSize = SPI_DataSize_8b;
        spi_handle->TxXferSize = 8;
        spi_handle->RxXferSize = 8;
    }
    else if (cfg->data_width <= 16)
    {
        spi_handle->Init.SPI_DataSize = SPI_DataSize_16b;
    }
    else
    {
        return RT_EIO;
    }

    /*SPI���ߵ�ģʽ ����ʱ��*/
    if (cfg->mode & RT_SPI_CPHA)
    {
        spi_handle->Init.SPI_CPHA = SPI_CPHA_2Edge;
    }
    else
    {
        spi_handle->Init.SPI_CPHA = SPI_CPHA_1Edge;
    }

    if (cfg->mode & RT_SPI_CPOL)
    {
       spi_handle->Init.SPI_CPOL = SPI_CPOL_High;
    }
    else
    {
        spi_handle->Init.SPI_CPOL = SPI_CPOL_Low;
    }

    spi_handle->Init.SPI_NSS = SPI_NSS_Soft;
    /*SPI���ߵĲ���������*/
    //device is not RT_NULL, so spi_bus not need check
    uint32_t SPI_APB_CLOCK;
    ch32v307_spi_clock_and_io_init(spi_handle->Instance); //spi_drv->config->Instance Ƭ��SPI
    SPI_APB_CLOCK = ch32v307_spi_clock_get(spi_handle->Instance);

    //��ȡSPI���ߵĲ��������ô����

    if (cfg->max_hz >= SPI_APB_CLOCK / 2)
    {
        spi_handle->Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    }
    else if (cfg->max_hz >= SPI_APB_CLOCK / 4)
    {
        spi_handle->Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    }
    else if (cfg->max_hz >= SPI_APB_CLOCK / 8)
    {
        spi_handle->Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    }
    else if (cfg->max_hz >= SPI_APB_CLOCK / 16)
    {
        spi_handle->Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    }
    else if (cfg->max_hz >= SPI_APB_CLOCK / 32)
    {
        spi_handle->Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    }
    else if (cfg->max_hz >= SPI_APB_CLOCK / 64)
    {
       spi_handle->Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    }
    else if (cfg->max_hz >= SPI_APB_CLOCK / 128)
    {
        spi_handle->Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    }
    else
    {
        /*  min prescaler 256 */
        spi_handle->Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    }
    /*�˴�Ϊ��ӡ��Ϣ -ϵͳƵ�ʡ�pclk2Ƶ�ʡ�SPI���Ƶ�Ƶ�ʣ�������Ƶ��-����SPI��ʱû�����ò�������ʱδ���*/
     LOG_D("sys freq: %d, pclk2 freq: %d, SPI limiting freq: %d, BaudRatePrescaler: %d",
         HAL_RCC_GetSysClockFreq(),
         SPI_APB_CLOCK,
         cfg->max_hz,
         spi_handle->Init.SPI_BaudRatePrescaler);
//    rt_kprintf("pclk2 freq: %d, SPI limiting freq: %d, BaudRatePrescaler: %d",
//        // HAL_RCC_GetSysClockFreq(),
//        SPI_APB_CLOCK,
//        cfg->max_hz,
//        spi_handle->Init.SPI_BaudRatePrescaler);
        

       /*SPI���������Ǹ�λ���Ȼ��ǵ�λ����*/
    if (cfg->mode & RT_SPI_MSB)
    {
       spi_handle->Init.SPI_FirstBit = SPI_FirstBit_MSB;
    }
    else
    {
        spi_handle->Init.SPI_FirstBit = SPI_FirstBit_LSB;
    }


    /*����SPI_Init�⺯����ʼ��SPI*/
    SPI_Init(spi_handle->Instance, &spi_handle->Init);
    /* Enable SPI_MASTER */

    SPI_Cmd(spi_handle->Instance, ENABLE);

     LOG_D("%s init done", spi_drv->config->bus_name);
//    rt_kprintf("%s init done", spi_drv->config->bus_name);

    return RT_EOK;
}

/*�˺�������ch32v307_spi_init*/
static rt_err_t spi_configure(struct rt_spi_device *device,
                              struct rt_spi_configuration *configuration)
{
    /*���жϴ�����Ч��*/
    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(configuration != RT_NULL);
    /*��ȡdevice->bus������structure stm32_spi���͵Ľṹ����*/
    /*Ȼ����½ṹ���е�cfg */
    struct ch32v307_spi *spi_drv =  rt_container_of(device->bus, struct ch32v307_spi, spi_bus);
    spi_drv->cfg = configuration;

    return ch32v307_spi_init(spi_drv, configuration);
}


static rt_uint32_t spi_xfer(struct rt_spi_device *device, struct rt_spi_message *message)
{
    // struct rt_spi_bus *spi_bus;
    // struct rt_spi_configuration *config;
    rt_err_t state;  //״̬��

    rt_size_t message_length, already_send_length;
    rt_uint16_t send_length;
    rt_uint8_t *recv_buf;
    const rt_uint8_t *send_buf;

    RT_ASSERT(device != NULL);
    RT_ASSERT(device->bus != RT_NULL);
    RT_ASSERT(device->bus->parent.user_data != RT_NULL);
    RT_ASSERT(message != NULL);

    //device is not RT_NULL, so spi_bus not need check
    // spi_bus = (struct rt_spi_bus *)device->bus;

    struct ch32v307_spi *spi_drv =  rt_container_of(device->bus, struct ch32v307_spi, spi_bus);
    SPI_HandleTypeDef *spi_handle = &spi_drv->handle;

    // config = &device->config;
    struct ch32v307_hw_spi_cs *cs = device->parent.user_data;
    /* take CS */
    if (message->cs_take && !(device->config.mode & RT_SPI_NO_CS))
    {
        if (device->config.mode & RT_SPI_CS_HIGH)
            GPIO_WriteBit(cs->GPIOx, cs->GPIO_Pin, Bit_SET);
        else
            GPIO_WriteBit(cs->GPIOx, cs->GPIO_Pin, Bit_RESET);
    }

     LOG_D("%s transfer prepare and start", spi_drv->config->bus_name);
     LOG_D("%s sendbuf: %X, recvbuf: %X, length: %d",
           spi_drv->config->bus_name,
           (uint32_t)message->send_buf,
           (uint32_t)message->recv_buf, message->length);

//    rt_kprintf("%s transfer prepare and start", spi_drv->config->bus_name);
//    rt_kprintf("%s sendbuf: %X, recvbuf: %X, length: %d",
//          spi_drv->config->bus_name,
//          (uint32_t)message->send_buf,
//          (uint32_t)message->recv_buf, message->length);

    message_length = message->length;
    recv_buf = message->recv_buf;
    send_buf = message->send_buf;

    while (message_length)
    {
        /* the HAL library use uint16 to save the data length */
        if (message_length > 65535)
        {
            send_length = 65535;
            message_length = message_length - 65535;
        }
        else
        {
            send_length = message_length;
            message_length = 0;
        }

        /* calculate the start address */
        already_send_length = message->length - send_length - message_length;
        send_buf = (rt_uint8_t *)message->send_buf + already_send_length;
        recv_buf = (rt_uint8_t *)message->recv_buf + already_send_length;

         /* start once data exchange */
        if (message->send_buf && message->recv_buf)
        {

            state = SPI_TransmitReceive(spi_handle->Instance, (uint8_t *)send_buf, (uint8_t *)recv_buf, send_length);

        }
        else if (message->send_buf)
        {

            state = SPI_Transmit(spi_handle->Instance, (uint8_t *)send_buf, send_length);
            if (message->cs_release && (device->config.mode & RT_SPI_3WIRE))
            {
                /* release the CS by disable SPI when using 3 wires SPI */
                /* ��ʹ��3����SPIʱ��ͨ������SPI�ͷ�CS */
                SPI_Cmd(spi_handle->Instance, DISABLE);
            }
        }
        else
        {
            memset((uint8_t *)recv_buf, 0xff, send_length);
            /* clear the old error flag */
            SPI_I2S_ClearFlag(spi_handle->Instance, SPI_I2S_FLAG_OVR);
            state = SPI_Receive(spi_handle->Instance, (uint8_t *)recv_buf, send_length);
        }

        if (state != RT_EOK)
        {
             LOG_I("spi transfer error : %d", state);
//            rt_kprintf("spi transfer error : %d", state);
            message->length = 0;
        }
        else
        {
             LOG_D("%s transfer done", spi_drv->config->bus_name);
//            rt_kprintf("%s transfer done", spi_drv->config->bus_name);
        }

    }
    /* release CS */
    if (message->cs_release && !(device->config.mode & RT_SPI_NO_CS))
    {
        if (device->config.mode & RT_SPI_CS_HIGH)
            GPIO_WriteBit(cs->GPIOx, cs->GPIO_Pin, Bit_RESET);
        else
            GPIO_WriteBit(cs->GPIOx, cs->GPIO_Pin, Bit_SET);
    }

    return message->length;
}

/*ʼ�����շ������Ѿ����ˣ�����������װ��struct rt_rt_spi_ops�ṹ������*/
static const struct rt_spi_ops ch32v307_spi_ops =
{
    .configure = spi_configure,
    .xfer = spi_xfer,
};


/*SPI���߶���ֵ��������ע�ᵽϵͳ��*/
static int rt_hw_spi_bus_init(void)
{
    rt_err_t result;

    for (rt_size_t i = 0; i < sizeof(spi_config) / sizeof(spi_config[0]); i++)
    {
        spi_bus_obj[i].config = &spi_config[i];
        spi_bus_obj[i].spi_bus.parent.user_data = &spi_config[i];
        spi_bus_obj[i].handle.Instance = spi_config[i].Instance;

        /**
         * DMA��ʱδ����
         * */

        //spi����ע�ᵽspi���������
        result = rt_spi_bus_register(&spi_bus_obj[i].spi_bus, spi_config[i].bus_name, &ch32v307_spi_ops);
        RT_ASSERT(result == RT_EOK);

         LOG_D("%s bus init done", spi_config[i].bus_name);
//        rt_kprintf("%s bus init done", spi_config[i].bus_name);
    }

    return result;
}

/**
  * Attach the spi device to SPI bus, this function must be used after initialization.
  */
/* ע���SPI����֮�󣬾Ϳ����������Ϲ����豸�ˣ�
 * ʹ�ú���rt_hw_spi_device_attach() ��SPI�豸���ص�SPI�����ϣ�
 * ���ҽ�SPI�豸ע�ᵽIO�豸��������*/
rt_err_t rt_hw_spi_device_attach(const char *bus_name, const char *device_name, GPIO_TypeDef *cs_gpiox, uint16_t cs_gpio_pin)
{
    RT_ASSERT(bus_name != RT_NULL);
    RT_ASSERT(device_name != RT_NULL);

    rt_err_t result;
    struct rt_spi_device *spi_device;
    struct ch32v307_hw_spi_cs *cs_pin;

    /* initialize the cs pin && select the slave*/
    GPIO_InitTypeDef GPIO_Initure;
    GPIO_Initure.GPIO_Pin = cs_gpio_pin;
    GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(cs_gpiox, &GPIO_Initure);
    GPIO_WriteBit(cs_gpiox, cs_gpio_pin, Bit_SET);

    /* attach the device to spi bus*/
    spi_device = (struct rt_spi_device *)rt_malloc(sizeof(struct rt_spi_device));
    RT_ASSERT(spi_device != RT_NULL);
    cs_pin = (struct ch32v307_hw_spi_cs *)rt_malloc(sizeof(struct ch32v307_hw_spi_cs));
    RT_ASSERT(cs_pin != RT_NULL);
    cs_pin->GPIOx = cs_gpiox;
    cs_pin->GPIO_Pin = cs_gpio_pin;
    result = rt_spi_bus_attach_device(spi_device, device_name, bus_name, (void *)cs_pin);

    if (result != RT_EOK)
    {
        LOG_E("%s attach to %s faild, %d\n", device_name, bus_name, result);
    }

    RT_ASSERT(result == RT_EOK);

    LOG_D("%s attach to %s done", device_name, bus_name);

    return result;
}


int rt_hw_spi_init(void)
{
    // stm32_get_dma_info();
    return rt_hw_spi_bus_init();
}
INIT_BOARD_EXPORT(rt_hw_spi_init);

#endif
