# EmbeddedAF
This repository contains the source code and hardware design files for a indoor automated urban farming system. 

The system has a master/slave architecture, where the master is refered to as the _master_, while the slaves as the _nodes_.

### **_The Concept:_**
The idea was to use a wireless sensor network system as a case study in an embedded systems development process.

Due to the master/slave architecture of the system, the project covers both embedded C development on the node devices and embedded Linux development on the master device, as well as communication over ZigBee between the two.

All in all; a good variation of challanges to learn more about embedded systems.

### **_The Node:_**
![Node Structure](Docs/Diagrams/Node/node_hardware.png?raw=true "Node Structure")

The node features a 16-bit MCU and a XBee 3 ZigBee module on an in-house developed and etched PCB. An OLED display and buttons are included on the board such that the node device can be used in an _offline mode_ independetly of the master device.

The node device measures temperature on the PCB and interfaces with 1) external digital temperature sensors, 2) an external analog soil moisture sensor and 3) an external relay that controls a grow-light.

The idea is to control the grow-light on a day/night-like cycle and record and publish the temperature and soil moisture measurements to the master device.

* The firmware is written in C, and can be found in [/src/node/](/src/node).
* The schematic and PCB is designed in Eagle, and can be found in [/Eagle/node/](/Eagle/node/).



![Master PCB](Docs/Photos/master_photo.png?raw=true "Master PCB") 

![Node  PCB](Docs/Photos/node_photo.png?raw=true "Node PCB")


![Master Structure](Docs/Diagrams/Master/master_SystemStructure.png?raw=true "Master Structure")
This repository contains stuff for a project on embedded automatic farming.

That includes circuit schematics, PCB designs and code to run on the two systems:

1. **The node:** A board with a 16-bit MCU and a XBee 3 micro module. It features two temperature sensors and one analog soil moisture sensor as well as I/O for offline availiability. The source found in src/node is written C and compiles with XC16.
2. **The master:** A Raspberry Pi 2B mounted with the same XB3 modules as the nodes. It runs Raspbian Stretch lite, a MQTT broker, a Node.js server that provides a web ui and finally a MQTT-SN gateway that is implemented in python and who's source code is found in src/master.

In addition there are some random python scripts, diagrams in the DrawIO folder, prototype schematics and board designs.

### The concept:

The idea here is to use a wireless sensor network system inteded to be used in indoor automatic farming as a case study in a development process.

I like watching things grow, and I think indoor has interesting benefits over traditional outdoor farming such as reduced water consumption, seasonal independence and a shorter logistic travel distance so perhaps some day all this might turn into something.

To do automated farming, it controls a LED lamp on a day/night-like cycle which can be configured through the on-board user interface or wirelessly.

The lamp and power distribution needs some work and will be put up here once it is up to spec. 

Further the node measures water content in soil which is intended to be used with automated watering. It has a Dallas 1-wire bus on one of it connectors which currently run both an on-board temp sensor and an external waterproof one that I planned on sticking in the soil.

The MCU does everything mentioned above, and UARTs with the XB3 which handle MQTT over ZigBee.

With many of these and an online master device from which you can monitor and control the whole setup it should be possible to realize indoor automated farming.

### Technical details:

#### The node:
* Microchip PIC24FJ128GA202 MCU. 16-bit.
* Digi XB3-24Z8UM RF module running ZigBee 3.0 API as a router.
* Maxim DS18B20 temperature sensors.
* Analog capacative soil moisture sensor.
* SSD1306 OLED display driver.
* 





