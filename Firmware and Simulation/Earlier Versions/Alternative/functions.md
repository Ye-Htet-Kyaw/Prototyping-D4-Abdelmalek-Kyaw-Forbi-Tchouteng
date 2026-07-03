Function -> Description
setup()	-> Initializes pin modes for motors, IR sensors, and ultrasonic triggers/echoes.
loop()	-> The main execution block that continuously checks for obstacles and selects the movement state.
checkobstacle() ->	Updates the obstacleDetected boolean by polling the ultrasonic sensors.
avoidObstacle() ->	The complex sequence of maneuvers (reverse, pivot, sweep) performed when an obstacle is detected.
actOnState() -> Handles the logic for the current movement state (Stop, Forward, Search).
stopMotors() -> Sets all motor pins to LOW and PWM to 0 to safely halt the robot.
setMotorSpeed()	-> Sets the direction and velocity (via PWM) for both motors.
getDistance()	-> Triggers the ultrasonic sensors and calculates the distance in centimeters.
searchForLineDuringWindow() -> A timed function that searches for the line while moving forward.
