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

#B3
```c
A.status = MASTER;
A.time_speed = 250;
A.latency_min = 2500;
A.latency_max = 10000;

B.time = 67189;
B.time_speed = 240;
B.latency_min = 2500;
B.latency_max = 10000;

C.time = 147189;
C.time_speed = 255;
C.latency_min = 2500;
C.latency_max = 10000;
```

#B4
```c
A.status = MASTER;
A.time_speed = 250;
A.latency_min = 1;
A.latency_max = 10;

B.time = 67189;
B.time_speed = 240;
B.latency_min = 1;
B.latency_max = 10;

C.time = 147189;
C.time_speed = 255;
C.latency_min = 1;
C.latency_max = 10;
```

#B5
```c
A.status = MASTER;
A.time_speed = 250;
A.latency_min = 9500;
A.latency_max = 10000;

B.time = 67189;
B.time_speed = 240;
B.latency_min = 9500;
B.latency_max = 10000;

C.time = 147189;
C.time_speed = 255;
C.latency_min = 9500;
C.latency_max = 10000;
```

#B6
```c
A.status = MASTER;
A.time_speed = 250;
A.latency_min = 100;
A.latency_max = 150;

B.time = 67189;
B.time_speed = 240;
B.latency_min = 100;
B.latency_max = 150;

C.time = 147189;
C.time_speed = 255;
C.latency_min = 100;
C.latency_max = 150;
```