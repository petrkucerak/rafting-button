# Scenarios

## B1
```c
game->deadline = 1000000000; // in ns (max value is UINT64_MAX)
game->nodes_count = 3;

A.status = MASTER;
A.time_speed = 250;
A.latency_min = 100;  // 1 μs
A.latency_max = 65535; // 262 μs

B.time = 67189;
B.time_speed = 198;
B.latency_min = 100;  // 1 μs
B.latency_max = 65535; // 262 μs

C.time = 147189;
C.time_speed = 100;
C.latency_min = 100;  // 1 μs
C.latency_max = 65535; // 262 μs
```

#B2
```c
game->deadline = 1000000000; // in ns (max value is UINT64_MAX)

A.status = MASTER;
A.time_speed = 250;
A.latency_min = 50;
A.latency_max = 200;

B.time = 67189;
B.time_speed = 198;
B.latency_min = 10;
B.latency_max = 300;

C.time = 147189;
C.time_speed = 100;
C.latency_min = 1000;
C.latency_max = 60000;
```