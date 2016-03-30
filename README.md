# Panorama-photo


## Project goal:
Create a device what can rotate camera (or camera-mounted phone) around Z axis to create perfect panorama photos or time-lapse videos.


### Functionality:
With this device the **user can create panorama photos** without "fitting problems"  
**or** can create **time-lapse videos with** using (very) slowly **rotating camera**.  
This device can be placed between any standard camera-tripod (or any other camera-mount) and the camera (or any other device what can be placed on a standard tripod (will be called as "camera" below)).
It can rotate the camera to selected direction with selected speed.
The device works from standard lithium battery with built-in battery charger.

### Target MCU: **ATmega8**

### Features:
 - User can select the direction of rotating
 - User can select the speed of rotating
 - Device can be mounted in any standard tripod (or any other device what uses standard mount-screw)
 - Any camera, cellphone mount or any other devices can be mounted on the top of this device which can be mounted on a standard tripod
 - User can set the position of mounted device in 2 axis to reach the optimal position of camera (where the camera's lens is exactly above the center point of rotation)
 - Works with using a replaceable Li-ion battery (in size of 18650)
 - Contains built-in battery charger module (charging current max 1A from micro-usb port)

**Sequence diagram of states**
```sequence
READY-->READY: User can set the initial direction
Note right of READY: Servo continuously follow the adjusted position.
READY-->READY: User can set the speed of rotation
READY->ROTATION_IN_PROGRESS: User press a button (for selected end)
ROTATION_IN_PROGRESS->ROTATION_DONE: Rotation finished.
Note right of ROTATION_DONE: System is waiting for user input (btn) to get "READY" state.
ROTATION_DONE->READY: Button pressed (any)
```

#### Last hw-connected test:

<a href="http://www.youtube.com/watch?feature=player_embedded&v=i8isvKmdPvM
" target="_blank"><img src="http://img.youtube.com/vi/i8isvKmdPvM/1.jpg"
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="1" /></a>

[I'm an inline-style link](https://www.google.com)
