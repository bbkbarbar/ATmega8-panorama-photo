# Panorama-photo


## Project goal:
Create a device what can rotate camera (or camera-mounted phone) around Z axis to create perfect panorama photos or time-lapse videos.


### Functionality:
With this device the **user can create panorama photos** without "fitting problems"  
**or** can create **time-lapse videos with** using (very) slowly **rotating camera**.  
This device can be placed between any standard camera-tripod (or any other camera-mount) and the camera (or any other device what can be placed on a standard tripod (will be called as "camera" below)).
It can rotate the camera to selected direction with selected speed.
The device works from standard lithium battery with built-in battery charger.

### Features:
 - User can select the direction of rotating
 - User can select the speed of rotating
 - Device can be mounted in any standard tripod (or any other device what uses standard mount-screw)
 - Any camera, cellphone mount or any other devices can be mounted on the top of this device which can be mounted on a standard tripod
 - User can set the position of mounted device in 2 axis to reach the optimal position of camera (where the camera's lens is exactly above the center point of rotation)
 - Works with using a replaceable Li-ion battery (in size of 18650)
 - Contains built-in battery charger module (charging current max 1A from micro-usb port)

### MCU: 
**Required features:**
 - PWM output (~50Hz) (1)
 - Analog input (at least 2 channel)
 - I/O input (at least 2)
 - I/O output (at least 2)
 - Low energy consumption
 
**Selected device** (MCU): Atmel **AVR ATmega8** ([DataSheet](https://github.com/bbkbarbar/ATmega8-panorama-photo/blob/master/Docs/ATmega8_datasheet.pdf))


###Sequence diagram of states (v1.1)
![alt text](https://github.com/bbkbarbar/ATmega8-panorama-photo/blob/master/Docs/sequence_v1_1_.png "Basic sequence diagram")


#### First "mainly working" version on testboard:

<a href="http://www.youtube.com/watch?i8isvKmdPvMfeature=player_embedded&v=33iNxKAsr9w
" target="_blank"><img src="http://img.youtube.com/vi/33iNxKAsr9w/2.jpg"
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="1" /></a>

#### First test device (mechanism and box built from Lego):
![alt text](https://raw.githubusercontent.com/bbkbarbar/ATmega8-panorama-photo/master/Docs/Test_on_lego_v1_1.jpg " ")
![alt text](https://raw.githubusercontent.com/bbkbarbar/ATmega8-panorama-photo/master/Docs/Test_on_lego_v1_2.jpg " ")

**UseCase diagram**
(..first plan..)
![alt text](https://github.com/bbkbarbar/ATmega8-panorama-photo/blob/master/Docs/useCase_plan_1.png "UseCase diagram")

[Previous test #1](https://youtu.be/i8isvKmdPvM)
