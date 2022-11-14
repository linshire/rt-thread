# STM32l496-ali-developer的Arduino生态兼容说明

## 1 RTduino - RT-Thread的Arduino生态兼容层

stm32l496-ali-developer已经完整适配了[RTduino软件包](https://github.com/RTduino/RTduino)，即RT-Thread的Arduino生态兼容层。用户可以按照Arduino的编程习惯来操作该BSP，并且可以使用大量Arduino社区丰富的库，是对RT-Thread生态的极大增强。更多信息，请参见[RTduino软件包说明文档](https://github.com/RTduino/RTduino)。

### 1.1 如何开启针对本BSP的Arduino生态兼容层

Env 工具下敲入 menuconfig 命令，或者 RT-Thread Studio IDE 下选择 RT-Thread Settings：

```Kconfig
Hardware Drivers Config --->
    Onboard Peripheral Drivers --->
        [*] Compatible with Arduino Ecosystem (RTduino)
```

## 2 Arduino引脚排布

该BSP遵照Arduino UNO板的引脚排列方式，详见 `pins_arduino.c`

### 2.1 Arduino引脚排布统览

更多引脚布局相关信息参见 [pins_arduino.c](pins_arduino.c) 和 [pins_arduino.h](pins_arduino.h)。

| Arduino引脚编号       | STM32引脚编号 | 5V容忍    | 备注                                           |
| --------------------- | --------- | --------- | -------------------------------------------- |
| 0 (D0)                | PA1       | 是        | Serial-Rx，被RT-Thread的UART设备框架uart1接管         |
| 1 (D1)                | PA0       | 是        | Serial-Tx，被RT-Thread的UART设备框架uart1接管         |
| 2 (D2)                | PD14      | 是        | 普通IO                                         |
| 3 (D3)                | PE6       | 是        | PWM3-CH4，默认被RT-Thread的PWM设备框架pwm4接管          |
| 4 (D4)                | PD11      | 是        | 普通IO                                         |
| 5 (D5)                | PE5       | 是        | PWM3-CH3，默认被RT-Thread的PWM设备框架pwm3接管            |
| 6 (D6)                | PE4       | 是        | PWM3-CH2，默认被RT-Thread的PWM设备框架pwm3接管          |
| 7 (D7)                | PE0       | 是        | 普通IO                                         |
| 8 (D8)                | PC11      | 是        | 普通IO                                         |
| 9 (D9)                | PC10      | 是        | 普通IO       |
| 10 (D10)              | PD0       | 是        | 普通IO        |
| 11 (D11)              | PC1       | 是        | 普通IO        |
| 12 (D12)              | PD3       | 是        | 普通IO                                         |
| 13 (D13)              | PA9       | 是        | 普通IO                                         |
| 14 (D14)              | PC1       | 是        | I2C-SCL                                   |
| 15 (D15)              | PC0       | 是        | I2C-SDA                                     |
| 16 (D16)              | PC7       | 是        | 蜂鸣器                                          |
| 17 (D17)              | PC12      | 是        | 普通IO                                          |
| 18 (D18)              | PD1       | 是        | 普通IO                                          |
| 19 (D19)              | PD9       | 是        | 普通IO                                          |
| A0                    | PC2       | 是（但不建议） | ADC1-CH3，默认被RT-Thread的ADC设备框架adc1管          |
| A1                    | PC3       | 是（但不建议） | ADC1-CH4，默认被RT-Thread的ADC设备框架adc1管         |


> 注意：
> 
> 1. 驱动舵机和analogWrite函数要选择不同定时器发生的PWM信号引脚，由于STM32的定时器4个通道需要保持相同的频率，如果采用相同的定时器发生的PWM分别驱动舵机和analogWrite，可能会导致舵机失效。
