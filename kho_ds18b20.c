#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "kho_ds18b20.h"
#include "kho_1wire.h"
#include "sensorsFunctions.h"
#include "system.h"

void DS18B20_initSensors(void)
{
    uint8_t loop;
    uint8_t sensor_id;

    for(sensor_id = 0; sensor_id < MAX_DS18D20_SENSORS; sensor_id++)
    {
        temperature_ds18d20[sensor_id][DS18D20_1WIRE_ID] = 0xFFFFFFFF;
    }
    sensor_id = 0;
    for(loop = 0; loop < MAX_1WIRE_IDS; loop++)
    {
        if(0x28 == OneWireDevice_list[loop][0])
        {
            temperature_ds18d20[sensor_id][DS18D20_1WIRE_ID] = loop;
            if(MAX_DS18D20_SENSORS == sensor_id++)
                break;
        }
    }
}

void DS18B20_matchDevice(uint8_t id)
{
    uint8_t temp;
    
    oneWire_write_byte(MATCH_ROM_DS18B20);
    for(temp = 0; temp<8; temp++)
        oneWire_write_byte(OneWireDevice_list[temperature_ds18d20[id][DS18D20_1WIRE_ID]][temp]);
}

void DS18B20_calculateTemperature(void)
{
    uint8_t temp;
    uint8_t data_read[9];
    int32_t real_temperature_temp;
    uint8_t sensor_id = 0;

    sensor_id = 0;
    while(temperature_ds18d20[sensor_id][DS18D20_1WIRE_ID]!=0xFFFFFFF && sensor_id < MAX_DS18D20_SENSORS)
    {
        if(oneWire_reset_and_detect())
        {
            //call device
            DS18B20_matchDevice(sensor_id);
            //start convert
            oneWire_write_byte(CONVERT_T_DS18B20);
        }
        else
        {
            // nothing
        }

        delay_ms(sensor_readout_delay_seconds/2);

        if(oneWire_reset_and_detect())
        {
            //call device
            DS18B20_matchDevice(sensor_id);
            //read data
            oneWire_write_byte(READ_SCRATCH_DS18B20);

            for(temp = 0; temp<9; temp++)
                data_read[temp] = oneWire_read_byte();
        }
        else
        {
            // nothing
        }

        real_temperature_temp = 0x0000 | (data_read[1] << 8) | data_read[0];
        real_temperature_temp = (real_temperature_temp * 100)/16;

        temperature_ds18d20[sensor_id][DS18D20_REAL_VALUE] = real_temperature_temp;
        temperature_ds18d20[sensor_id][DS18D20_BCD_VALUE] = convertToBCD(real_temperature_temp);

        sensor_id++;
    }
}
