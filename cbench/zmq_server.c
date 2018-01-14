#include <mpi.h>
#include <stdio.h>
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


void bench_zmq_tcp_server(const char *endpoint, unsigned s_s, size_t s_size)
{
    void *data;
    void *ctx = zmq_ctx_new();
    void *socket = zmq_socket(ctx, ZMQ_ROUTER);
    size_t p;
    char c;
    double t_start = 0, t_end = 0;
    if (zmq_bind(socket, endpoint)) {
        printf("Error!\n");
        return;
    }
    zmq_recv(socket,&c,1,0);
    t_start = MPI_Wtime();
    switch (s_s) {
        case S_KB:
            for (p = 0; p < s_size; p += _chunk_kb) {
#ifdef DEBUG
                printf("recving %lu/%lu parts\n", p, s_size);
#endif
                data = malloc(_chunk_kb);
                zmq_recv(socket, data, _chunk_kb, 0);

                printf("recving %lu/%lu parts\n", p, s_size);

                free(data);
            }
            data = malloc(_chunk_kb);
            zmq_recv(socket, data, _chunk_kb, 0);
            free(data);
            break;
        case S_MB:

            for (p = 0; p < s_size; p += _chunk_mb) {
#ifdef DEBUG
                printf("recving %lu/%lu parts\n", p, s_size);
#endif
                data = malloc(_chunk_mb);
                zmq_recv(socket, data, _chunk_mb, 0);
                free(data);
            }
            data = malloc(_chunk_mb);
            zmq_recv(socket, data, _chunk_mb, 0);
            free(data);
            break;
        case S_GB:

            for (p = 0; p < s_size; p += _chunk_gb) {
#ifdef DEBUG
                printf("recving %lu/%lu parts\n", p, s_size);
#endif
                data = malloc(_chunk_gb);
                zmq_recv(socket, data, _chunk_gb, 0);
                free(data);
            }
            data = malloc(_chunk_gb);
            zmq_recv(socket, data, _chunk_gb, 0);
            free(data);
            break;
        default:
            printf("no such format!\n");
            exit(139);
    }
    t_end = MPI_Wtime();
    printf("ZMQ Server:Finished receiving data. WS: %f; WE: %f; WD: %f\n", t_start, t_end,
           t_end - t_start);
    zmq_close(socket);
    zmq_ctx_destroy(ctx);

}

int main(int argc, char ** argv)
{
        bench_zmq_tcp_server("ipc:///tmp/bench", (unsigned int) strtoul(argv[2], NULL, 10),
                             strtoul(argv[3], NULL, 10));
	return 0;
}
