#include <stdio.h>

#include <mpi.h>
#include <zmq.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#define _KB 1000
#define _MB 1000000
#define _GB 1000000000
#define S_KB 1
#define S_MB 2
#define S_GB 3

#define _P_KB 10
#define _P_MB 10
#define _P_GB (1/5)

const size_t _chunk_kb = _P_KB;
const size_t _chunk_mb = _P_MB;
const size_t _chunk_gb = _P_GB;

double _wstart_time;
double _wfinish_time;
#define DEBUG


