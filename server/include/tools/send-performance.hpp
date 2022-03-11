/**
 * Examples - Send Performance
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <time.h>

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePairFactory.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>
#include <infinity/memory/RegionToken.h>
#include <infinity/requests/RequestToken.h>

#define PORT_NUMBER 40201
#define SERVER_IP "192.168.0.100"
#define BUFFER_COUNT 128
#define MAX_BUFFER_SIZE 8192
#define OPERATIONS_COUNT 4096

uint64_t timeDiff(struct timeval stop, struct timeval start);

// Usage: ./progam -s for server and ./program for client component
void runBench(bool isServer);
uint64_t timeDiff(struct timeval stop, struct timeval start);
