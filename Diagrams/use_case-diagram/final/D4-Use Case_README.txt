OUR USE CASE DIAGRAM
==================================

Purpose
-------
The use case diagram represents the functional requirements and high-level goals of the autonomous vehicle system from the perspective of the operator.

The diagram intentionally focuses on WHAT the system must achieve rather than HOW the implementation is performed.

ACTOR
------
Operator(D4 members in this case)
The operator is responsible for interacting with the system by:

- Starting the vehicle
- Stopping the vehicle
- Calibrating the sensors and hardware
- Running test scenarios
- Initiating autonomous driving mode

USE CASES
----------

1. Start Vehicle
The operator powers on and initializes the robot before autonomous operation begins.

2. Stop Vehicle
The operator manually stops the robot after testing or mission completion.

3. Drive Autonomously
The primary functional goal of the system. The vehicle follows a predefined path while responding to environmental conditions without further operator intervention.

4. Calibrate Vehicle
Calibration includes:

- Verifying IR sensor alignment
- Checking ultrasonic sensor positioning
- Testing wheel movement
- Confirming correct sensor thresholds
- Ensuring reliable operation under current lighting conditions

5. Run Test Scenario
Allows controlled experiments and validation before full deployment.

6. Follow Lane
The robot continuously tracks the black guide line using two infrared sensors.

<<include>> Detect Obstacle

<<include>> Detect Final Stop Line

Both behaviours are mandatory components of lane following and therefore are represented using include relationships.

7. Detect Obstacle
The ultrasonic sensor continuously monitors the environment ahead of the vehicle.

<<include>> Respond to Obstacle Pattern

Obstacle detection cannot exist independently because every detected obstacle must trigger a behavioural response.

8. Respond to Obstacle Pattern
The system determines the appropriate action according to the internal is180 flag.

<<extend>> Dodge Obstacle [is180 == false]

<<extend>> Perform 180° Turn [is180 == true]

Only one alternative behaviour is executed at a time, making extend relationships appropriate.

9. Dodge Obstacle
When is180 is false, the robot:

- Avoids the obstacle
- Rejoins the original track
- Sets is180 to true for the next obstacle encounter

10. Perform 180° Turn
When is180 is true, the robot:

- Executes a complete 180-degree rotation
- Continues in the opposite direction
- Sets is180 back to false

11. Reacquire Line
After any obstacle manoeuvre, the robot searches for and reconnects with the guide line before resuming normal operation.

12. Detect Final Stop Line
The mission endpoint is represented by both IR sensors continuously detecting black for more than 350 milliseconds.

13. Complete Mission
After detecting the final stop line, the motors stop and the program remains halted until the system is manually reset.

RELATIONSHIPS
-------------

Association:
Operator -> Use Cases

Generalization:
Follow Lane -> Drive Autonomously

Include Relationships:
- Follow Lane -> Detect Obstacle
- Follow Lane -> Detect Final Stop Line
- Detect Obstacle -> Respond to Obstacle Pattern
- Dodge Obstacle -> Reacquire Line
- Perform 180° Turn -> Reacquire Line
- Detect Final Stop Line -> Complete Mission

Extend Relationships:
- Dodge Obstacle -> Respond to Obstacle Pattern [is180 == false]
- Perform 180° Turn -> Respond to Obstacle Pattern [is180 == true]

OUR DESIGN PHILOSOPHY
-----------------
The use case diagram intentionally abstracts implementation details.

Actions such as:
- Turning left 45 degrees
- Moving around an obstacle
- Timing delays
- Motor PWM control
- Sensor threshold calculations

are operational mechanisms rather than system goals and therefore have been represented in our state machine and implementation code rather than the use case model.