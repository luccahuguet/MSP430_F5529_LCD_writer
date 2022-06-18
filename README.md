# MSP430-F5529 LCD Writer

This project, developed in Code Composer Studio, enables one to write to a LCD-Module (using IC2) using a gyroscope as a controller and an a joystick as a button (solely to press up and down).

Technologies used:
- MSP430-F5529
- mpu6050
- jumpers
- joystick
- LCD 1602A solded with the module that allows for IC2 communication

There are two modes: cursor moving mode, and character selecting mode.

Program already starts in cursor moving mode, you move the cursor around using the gyroscope to select the position you want to write.

Once the position is selected, you press the button to enter char selecting mode.

Now if you point the gyroscope "upwards or down", you will change the char collection being used:
- Uppercase letters
- Lowercase letters
- Numbers

Inclining the gyroscope left or right will traverse said collections.

Now if you click the button again, you will switch back to cursor moving mode, and the character you chose was written successfully!
