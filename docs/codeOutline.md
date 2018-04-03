********************************************************************************
                    CODE OUTLINE FOR GAME CONTROLLER
********************************************************************************
@author:  Chris Porras
@date:    04/03/18
@brief:   Notes and outline of the game controller to run the pinball machine
          Game controller found in src/pinball.ino

********************************************************************************
                    Need elements for game controller
********************************************************************************

1)  Setup

2)  Run/loop

3)  Flipper controller
    - Responsible for firing the solenoids of the flippers on button press

4)  Sensor controller
    - Responsible for gathering sensor data and updating score based on data

5)  FSM with different game states
    ** TODO: Determine all necessary game states to include

6)  Score to Display Function
    - Responsible for updating the score based on the inputs from sensors and
      bumpers

7)  fireSolenoidNow && unFireSolenoidNow
    ** TODO: Figure out what these are for
