Panorama-photo

Target device: **ATmega8**

Functionality:
With this device the **user can create panorama photos** without "fitting problems"  
**or** can create **time-lapse videos with** using (very) slowly **rotating camera**.  
This device can be placed between any standard camera-tripod (or any other camera-mount) and the camera (or any other device what can be placed on a standard tripod (will be called as "camera" below)).
It can rotate the camera to selected direction with selected speed.



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




<a href="http://www.youtube.com/watch?feature=player_embedded&v=i8isvKmdPvM
" target="_blank"><img src="http://img.youtube.com/vi/i8isvKmdPvM/4.jpg"
alt="IMAGE ALT TEXT HERE" width="24" height="18" border="0" /></a>

[I'm an inline-style link](https://www.google.com)
