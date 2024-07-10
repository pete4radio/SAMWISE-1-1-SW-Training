#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // SD card and radio SPI
    { MP_ROM_QSTR(MP_QSTR_SCK),      MP_ROM_PTR(&pin_PA13)  },
    { MP_ROM_QSTR(MP_QSTR_MOSI),     MP_ROM_PTR(&pin_PA12)  },
    { MP_ROM_QSTR(MP_QSTR_MISO),     MP_ROM_PTR(&pin_PA14)  },
    { MP_ROM_QSTR(MP_QSTR_SD_CS),    MP_ROM_PTR(&pin_PA27)  },

    // MPPTs
    { MP_ROM_QSTR(MP_QSTR_MPPT_SHDN_1),    MP_ROM_PTR(&pin_PB01)  },
    { MP_ROM_QSTR(MP_QSTR_MPPT_SHDN_2),    MP_ROM_PTR(&pin_PB23)  },
    { MP_ROM_QSTR(MP_QSTR_MPPT_STAT_1),    MP_ROM_PTR(&pin_PB31)  },
    { MP_ROM_QSTR(MP_QSTR_MPPT_STAT_2),    MP_ROM_PTR(&pin_PB22)  },

    // RPi Temp
    { MP_ROM_QSTR(MP_QSTR_RPI_TEMP),    MP_ROM_PTR(&pin_PA04)  },
    { MP_ROM_QSTR(MP_QSTR_RPI_EN),    MP_ROM_PTR(&pin_PB07)  },
    { MP_ROM_QSTR(MP_QSTR_RPI_RST),    MP_ROM_PTR(&pin_PB09)  },

    // Burn wire
    { MP_ROM_QSTR(MP_QSTR_RELAY_A),  MP_ROM_PTR(&pin_PB15)  },
    { MP_ROM_QSTR(MP_QSTR_BURN1),    MP_ROM_PTR(&pin_PA15)  },
    { MP_ROM_QSTR(MP_QSTR_BURN2),    MP_ROM_PTR(&pin_PA05)  },
    
    // Battery
    { MP_ROM_QSTR(MP_QSTR_BATTERY),  MP_ROM_PTR(&pin_PA06)  },
    { MP_ROM_QSTR(MP_QSTR_BATT_THERM_B),   MP_ROM_PTR(&pin_PA02)  },
    { MP_ROM_QSTR(MP_QSTR_BATT_THERM_A),   MP_ROM_PTR(&pin_PA07)  },
    { MP_ROM_QSTR(MP_QSTR_BATT_HEATER_A),   MP_ROM_PTR(&pin_PB16)  },
    { MP_ROM_QSTR(MP_QSTR_BATT_HEATER_B),   MP_ROM_PTR(&pin_PB17)  },

    { MP_ROM_QSTR(MP_QSTR_CHRG),     MP_ROM_PTR(&pin_PB08)  },
    { MP_ROM_QSTR(MP_QSTR_FAULT),     MP_ROM_PTR(&pin_PB14)  },

    { MP_ROM_QSTR(MP_QSTR_VBUS_RST), MP_ROM_PTR(&pin_PA18)  },

    // Radio power enable
    { MP_ROM_QSTR(MP_QSTR_RF_EN),    MP_ROM_PTR(&pin_PB05)  },

    // Radio communications
    { MP_ROM_QSTR(MP_QSTR_RF1_RST),  MP_ROM_PTR(&pin_PA19)  },
    { MP_ROM_QSTR(MP_QSTR_RF1_CS),   MP_ROM_PTR(&pin_PB30)  },
    { MP_ROM_QSTR(MP_QSTR_RF1_IO4),  MP_ROM_PTR(&pin_PB04)  },
    { MP_ROM_QSTR(MP_QSTR_RF1_IO0),  MP_ROM_PTR(&pin_PB00)  },
    
    // Uart for ADCS
    { MP_ROM_QSTR(MP_QSTR_ADCS_TX),       MP_ROM_PTR(&pin_PB02)  },
    { MP_ROM_QSTR(MP_QSTR_ADCS_RX),       MP_ROM_PTR(&pin_PB03)  },

    // ADCS enables
    { MP_ROM_QSTR(MP_QSTR_ADCS_EN_LP),      MP_ROM_PTR(&pin_PA20)  },
    { MP_ROM_QSTR(MP_QSTR_ADCS_EN),       MP_ROM_PTR(&pin_PA22)  },

    // Uart for RPi
    { MP_ROM_QSTR(MP_QSTR_RPI_TX),      MP_ROM_PTR(&pin_PA16)  },
    { MP_ROM_QSTR(MP_QSTR_RPI_RX),      MP_ROM_PTR(&pin_PA17)  },
    
    // I2C for MPPT/current monitor/USB charger
    { MP_ROM_QSTR(MP_QSTR_SDA),      MP_ROM_PTR(&pin_PB12)  },
    { MP_ROM_QSTR(MP_QSTR_SCL),      MP_ROM_PTR(&pin_PB13)  },

    // Watchdog
    { MP_ROM_QSTR(MP_QSTR_WDT_WDI),  MP_ROM_PTR(&pin_PA23)  },

    // Neopixel
    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PA21)  },

    // Serial Definitions
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C),  MP_ROM_PTR(&board_i2c_obj)  },
    { MP_ROM_QSTR(MP_QSTR_SPI),  MP_ROM_PTR(&board_spi_obj)  },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
