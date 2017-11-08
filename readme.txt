Use STM32Cube_FW_F4_V1.16.0 as the HAL driver.
This is an test project of OV7670 with AL422B on the STM32F412g-disco board.
connection is:
OV7670            STM32F412g-disco
SCL               PB8
SDA               PB9
D7-D0             PC7-PC0
/WRST             PB11
/RRST             PA13
RCK               PB5
WR                PG10
Vsync             PB7
/OE               GND
PWDN              GND(if exists)
/RST               not connected

Addition, use a rubber band to bind all the control signal wires together, otherwise it may interfere
the data lines which would cause the wrong image displayed.
