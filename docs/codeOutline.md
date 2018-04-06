********************************************************************************
                    CODE OUTLINE FOR GAME CONTROLLER
********************************************************************************
author:  Chris Porras

date:    04/03/18

brief:   Notes and outline of the game controller to run the pinball machine
          Game controller found in src/pinball.ino

********************************************************************************
                    Need elements for game controller
********************************************************************************

1)  Setup

2)  Run/loop

    **THIS WILL BE HANDLED IN THE FSM**

3)  Flipper controller
    - Responsible for firing the solenoids of the flippers on button press

4)  Sensor controller
    - Responsible for gathering sensor data and updating score based on data

5)  FSM with different game states

    **TODO: Determine all necessary game states to include**

    - IDLE
      - Default mode when the machine is not in use. Could omit this mode if we don't feel it is necessary
    - START
      - Game mode for when the player is ready to play the game
      - Make sure all components setup
      - Put ball in ball shoot and change to PLAY mode
    - PLAY (BALLONE)
      - Game loop that handles the playing of the game
      - Monitor sensor/update scoring
      - Check inputs to fire solenoids if warranted
      - Keep track of how many balls have been used
      - Once max ball limit has been reached, switch to GAMEOVER
    - GAMEOVER
      - Turn off all components
      - Switch to default mode: either IDLE or START


6)  Score to Display Function
    - Responsible for updating the score based on the inputs from sensors and
      bumpers

7)  fireSolenoidNow && unFireSolenoidNow

    **TODO: Figure out what these are for**
