# Raincube-v0.4
Decentralized IoT Water Management System

All hardware design files relate to an IoT based water management system prototype designed for Raincube LLC as Raincube v0.4. The single board microcontroller is an early model designed to test all active features, current operational concepts, and a series of new design ideas for future iterations. 

Main board based on Microchip ATmega328p microcontroller with 4 built-in AC solid state relays, 5 pins for SIM900 GSM module, 4 pins for a HC-SR04 ultrasonic distance sensor, and 6 pins for an external FTDI cable/board. All different power requirements (120VAC, 24VAC, 5VDC) are met by external components.

The system provides remote access to 4 built-in SSRs and an external HC-SR04 ultrasonic ranging sensor. SSRs 1-3 manage 24VAC solenoids. SSR 4 controls a 120VAC water pump. Remote access via TCP connection over GPRS using SIM900 module.

Designed by Carlos V. @IOTE

Creative Commons Attribution
