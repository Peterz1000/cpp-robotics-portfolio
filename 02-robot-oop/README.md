# 02 — OOP Robot Class System
### Edem Uyimeabasi Linus

A robot fleet management system demonstrating
core C++ OOP concepts through realistic robotics design.

## Class Architecture
## C++ Concepts Demonstrated
| Concept | Where used |
|---|---|
| Classes & encapsulation | Sensor, Motor, Robot |
| Inheritance | GroundRobot, UAV extend Robot |
| Pure virtual functions | move(), sense(), type() |
| Polymorphism | Fleet vector holds mixed types |
| Smart pointers | unique_ptr auto-manages memory |
| Constructor initialiser lists | All classes |
| Const correctness | Read-only methods marked const |

## Compile & Run
```bash
g++ -std=c++17 -o robot robot_system.cpp && ./robot
```

## Sample Output
