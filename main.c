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
#define CLIENT
//#define DEBUG
void bench_zmq_tcp_client(const char *endpoint, unsigned s_s, size_t s_size)
{

    void *data;
    void *ctx = zmq_ctx_new();
    void *socket = zmq_socket(ctx, ZMQ_PUB);
    size_t p;
    _wstart_time = MPI_Wtime();
    printf("Connecting to %s\n", endpoint);
    if (zmq_bind(socket, endpoint)) {
        printf("Error!\n");
        return;
    }
    sleep(10);
    zmq_send(socket, "A",1,ZMQ_SNDMORE);
    zmq_send(socket, "S",1,0);
    _wstart_time=MPI_Wtime();

	while(1) {
		data = malloc(_chunk_mb);
		zmq_send(socket, "A", 1, ZMQ_SNDMORE);
		zmq_send(socket, data, _chunk_mb, 0);
		free(data);
	}
	return ;
	switch (s_s) {
        case S_KB:
            for (p = 0; p < s_size; p += _chunk_kb) {
#ifdef DEBUG
                printf("sending %lu/%lu parts\n", p, s_size);
#endif
		zmq_send(socket, "A", 1, ZMQ_SNDMORE);
                data = malloc(_chunk_kb);
                zmq_send(socket, data, _chunk_kb, 0);
                free(data);
            }
            break;
        case S_MB:
            for (p = 0; p < s_size; p += _chunk_mb) {
#ifdef DEBUG
                printf("sending %lu/%lu parts\n", p, s_size);
#endif
                zmq_send(socket, "A", 1, ZMQ_SNDMORE); 
	       data = malloc(_chunk_mb);
                zmq_send(socket, data, _chunk_mb,0);
                free(data);
            }
            break;
        case S_GB:
            for (p = 0; p < s_size; p += _chunk_gb) {
#ifdef DEBUG
                printf("sending %lu/%lu parts\n", p, s_size);
#endif
                data = malloc(_chunk_gb);
                zmq_send(socket, data, _chunk_gb, ZMQ_SNDMORE);
                free(data);
            }
            data = malloc(_chunk_gb);
            zmq_send(socket, data, _chunk_gb, 0);
            free(data);
            break;
        default:
            printf("No such format!\n");
            exit(139);
    }
    _wfinish_time = MPI_Wtime();
#ifdef CLIENT
    printf("ZMQ Client:Finished sending data. WS: %f; WE: %f; WD: %f\n", _wstart_time, _wfinish_time,
           _wfinish_time - _wstart_time);
#endif
    zmq_close(socket);
    zmq_ctx_destroy(ctx);
}

void bench_zmq_tcp_server(const char *endpoint, unsigned s_s, size_t s_size)
{
    void *data;
    void *ctx = zmq_ctx_new();
    void *socket = zmq_socket(ctx, ZMQ_SUB);
    size_t p;
    char c;
    double t_start = 0, t_end = 0;
    printf("Binding to %s\n", endpoint);
    if (zmq_connect(socket, endpoint)) {
        printf("Error!\n");
        return;
    }
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "A", 1);
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "A", 1);
    char buf;
    zmq_recv(socket, &buf, 1, 0);
    zmq_recv(socket, &c, 1, 0);
    unsigned i;
    t_start = MPI_Wtime();
    switch (s_s) {
        case S_KB:
            for (p = 0; p < s_size; p += _chunk_kb) {
#ifdef DEBUG
                printf("recving %lu/%lu parts\n", p, s_size);
#endif
                data = malloc(_chunk_kb);
		zmq_recv(socket, &buf, 1, 0);
                zmq_recv(socket, data, _chunk_kb, 0);
                free(data);
            }
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


void bench_mpi_tcp(unsigned s_s, size_t s_size)
{
    void *data;
    MPI_Status status;
    size_t p;
    double t_start, t_end;
    int rank;
    t_start = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#ifdef DEBUG
    printf("Rank: %d \n", rank);
#endif
    if (rank)   /* client */
        switch (s_s) {
            case S_KB:

                for (p = 0; p < s_size; p += _chunk_kb) {
#ifdef DEBUG
                    printf("sending %lu/%lu parts\n", p, s_size);
#endif
                    data = malloc(_chunk_kb);
                    MPI_Send(data, _chunk_kb, MPI_BYTE, 0, 3, MPI_COMM_WORLD);
                    free(data);
                }
#ifdef DEBUG
                printf("sending one more part...\n");
#endif
                data = malloc(_chunk_kb);
                MPI_Send(data, _chunk_kb, MPI_BYTE, 0, 3, MPI_COMM_WORLD);
                free(data);
                break;
            case S_MB:

                for (p = 0; p < s_size; p += _chunk_mb) {
#ifdef DEBUG
                    printf("sending %lu/%lu parts\n", p, s_size);
#endif
                    data = malloc(_chunk_mb);
                    MPI_Send(data, _chunk_mb, MPI_BYTE, 0, 3, MPI_COMM_WORLD);
                    free(data);
                }
                data = malloc(_chunk_mb);
                MPI_Send(data, _chunk_mb, MPI_BYTE, 0, 3, MPI_COMM_WORLD);
                free(data);
                break;
            case S_GB:

                for (p = 0; p < s_size; p += _chunk_gb) {
#ifdef DEBUG
                    printf("sending %lu/%lu parts\n", p, s_size);
#endif
                    data = malloc(_chunk_gb);
                    MPI_Send(data, _chunk_gb, MPI_BYTE, 0, 3, MPI_COMM_WORLD);
                    free(data);
                }
                data = malloc(_chunk_gb);
                MPI_Send(data, _chunk_gb, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
                free(data);
                break;
            default:
                printf("no such format!: %u\n", s_s);
                exit(139);
        }
    else    /* server */
        switch (s_s) {

            case S_KB:
                for (p = 0; p < s_size; p += _chunk_kb) {
#ifdef DEBUG
                    printf("recving %lu/%lu parts\n", p, s_size);
#endif
                    data = malloc(_chunk_kb);
                    MPI_Recv(data, _chunk_kb, MPI_BYTE, 1, 3, MPI_COMM_WORLD, &status);
                    free(data);
                }
                data = malloc(_chunk_kb);
                MPI_Recv(data, _chunk_kb, MPI_BYTE, 1, 3, MPI_COMM_WORLD, &status);
                free(data);

                break;
            case S_MB:

                for (p = 0; p < s_size; p += _chunk_mb) {
#ifdef DEBUG
                    printf("recving %lu/%lu parts\n", p, s_size);
#endif
                    data = malloc(_chunk_mb);
                    MPI_Recv(data, _chunk_mb, MPI_BYTE, 1, 3, MPI_COMM_WORLD, &status);
                    free(data);
                }
                data = malloc(_chunk_mb);
                MPI_Recv(data, _chunk_mb, MPI_BYTE, 1, 3, MPI_COMM_WORLD, &status);
                free(data);

                break;
            case S_GB:

                for (p = 0; p < s_size; p += _chunk_gb) {
#ifdef DEBUG
                    printf("recving %lu/%lu parts\n", p, s_size);
#endif
                    data = malloc(_chunk_gb);
                    MPI_Recv(data, _chunk_gb, MPI_BYTE, 1, 3, MPI_COMM_WORLD, &status);
                    free(data);
                }
                data = malloc(_chunk_gb);
                MPI_Recv(data, _chunk_gb, MPI_BYTE, 1, 3, MPI_COMM_WORLD, &status);
                free(data);

                break;
            default:
                printf("no such format: %u\n", s_s);
                exit(139);
        }
    /* we finished! */
    t_end = MPI_Wtime();

    if (rank) {
#ifdef CLIENT
       printf("MPI Client:Finished sending data. WS: %f; WE: %f; WD: %f\n", t_start, t_end,
               t_end - t_start);
#endif
    } else
        printf("MPI Server:Finished sending data. WS: %f; WE: %f; WD: %f\n", t_start, t_end,
               t_end - t_start);;
}


int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
#ifdef DEBUG
    printf("Your arguments: %s, Mode %u, Quantity %lu\n", argv[1], (unsigned int) strtoul(argv[2], NULL, 10),
           strtoul(argv[3], NULL, 10));
#endif
    if (!strcmp(argv[1], "zmq-server"))
        bench_zmq_tcp_server("tcp://127.0.0.1:3993", (unsigned int) strtoul(argv[2], NULL, 10),
                             strtoul(argv[3], NULL, 10));
    else if (!strcmp(argv[1], "zmq-client"))
        bench_zmq_tcp_client("tcp://127.0.0.1:3993", (unsigned int) strtoul(argv[2], NULL, 10),
                             strtoul(argv[3], NULL, 10));
    else
        bench_mpi_tcp((unsigned int) strtoul(argv[2], NULL, 10), strtoul(argv[3], NULL, 10));

    MPI_Finalize();
    return 0;
}
