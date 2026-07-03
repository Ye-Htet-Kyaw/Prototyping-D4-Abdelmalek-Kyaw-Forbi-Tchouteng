<img width="1049" height="459" alt="image" src="https://github.com/user-attachments/assets/cc48060b-8aaa-4441-ad6b-fe9735b4832b" />

# Module Definitions

| Module | Responsibility |
| :--- | :--- |
| **`RobotCar.ino`** | Main entry point; acts as the high-level task scheduler. |
| **`LineFollow.cpp`** | Contains the state machine logic for track navigation (Rules 1-4). |
| **`Obstacle.cpp`** | Manages complex maneuver sequences and obstacle avoidance states. |
| **`Motors.cpp`** | Low-level hardware driver; handles PWM and H-bridge signals. |
| **`Sensors.cpp`** | Interface for Ultrasonic and IR sensor data acquisition. |
| **`Config.h`** | Centralized constants (pins, speeds, thresholds, timing). |
