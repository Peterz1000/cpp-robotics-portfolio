# 01 — Data Structures in C++
### Edem Uyimeabasi Linus

Core data structures implemented from scratch
in C++ — the foundation every robotics algorithm
is built on.

## Structures Implemented
| Structure | Time Complexity | Used in Robotics |
|---|---|---|
| Dynamic Array (Vector) | push O(1) amortized | Sensor buffers, waypoint lists |
| Singly Linked List | insert O(1) front | Adjacency lists, path nodes |
| Stack (LIFO) | push/pop O(1) | DFS search, undo operations |
| Queue (FIFO) | enqueue/dequeue O(1) | BFS planning, ROS message queues |

## Compile & Run
```bash
g++ -std=c++17 -o ds data_structures.cpp && ./ds
```
## Key Concept
The Vector doubles its capacity each time it
fills up — this is why push_back is O(1)
amortized even though individual resizes are O(n).
