 Panorama-photo


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






[I'm an inline-style link](https://www.google.com)
