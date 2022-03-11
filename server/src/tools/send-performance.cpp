/**
 * Examples - Send Performance
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */
#include <tools/send-performance.hpp>

uint64_t timeDiff(struct timeval stop, struct timeval start);

// Usage: ./progam -s for server and ./program for client component
void runBench(bool isServer) {

	infinity::core::Context *context = new infinity::core::Context();
	infinity::queues::QueuePairFactory *qpFactory = new infinity::queues::QueuePairFactory(context);
	infinity::queues::QueuePair *qp;

	if (isServer) {

		printf("Creating buffers to receive a messages\n");
		infinity::memory::Buffer **receiveBuffers = new infinity::memory::Buffer *[BUFFER_COUNT];
		for (uint32_t i = 0; i < BUFFER_COUNT; ++i) {
			receiveBuffers[i] = new infinity::memory::Buffer(context, MAX_BUFFER_SIZE * sizeof(char));
			context->postReceiveBuffer(receiveBuffers[i]);
		}

		printf("Waiting for incoming connection\n");
		qpFactory->bindToPort(PORT_NUMBER);
		qp = qpFactory->acceptIncomingConnection();

		printf("Waiting for first message (first message has additional setup costs)\n");
		infinity::core::receive_element_t receiveElement;
		while (!context->receive(&receiveElement));
		context->postReceiveBuffer(receiveElement.buffer);

		printf("Performing measurement\n");

		uint32_t messageSize = 1;
		uint32_t rounds = (uint32_t) log2(MAX_BUFFER_SIZE);

		for(uint32_t sizeIndex = 0; sizeIndex <= rounds; ++sizeIndex) {

			printf("Receiving messages of size %d bytes\n", messageSize);
			fflush(stdout);

			uint32_t numberOfReceivedMessages = 0;
			while (numberOfReceivedMessages < OPERATIONS_COUNT) {
				while (!context->receive(&receiveElement));
				++numberOfReceivedMessages;
				context->postReceiveBuffer(receiveElement.buffer);
			}

			messageSize *= 2;
		}

		printf("All messages received\n");

		printf("Sending notification to client\n");
		infinity::memory::Buffer *sendBuffer = new infinity::memory::Buffer(context, sizeof(char));
		qp->send(sendBuffer, context->defaultRequestToken);
		context->defaultRequestToken->waitUntilCompleted();

		printf("Clean up\n");
		for (uint32_t i = 0; i < BUFFER_COUNT; ++i) {
			delete receiveBuffers[i];
		}
		delete receiveBuffers;
		delete sendBuffer;

	} else {
		struct timespec begin, end;


		printf("Connecting to remote node\n");
		qp = qpFactory->connectToRemoteHost(SERVER_IP, PORT_NUMBER);

		printf("Creating buffers\n");
		infinity::memory::Buffer *sendBuffer = new infinity::memory::Buffer(context, MAX_BUFFER_SIZE * sizeof(char));
		infinity::memory::Buffer *receiveBuffer = new infinity::memory::Buffer(context, sizeof(char));
		context->postReceiveBuffer(receiveBuffer);

		printf("Sending first message\n");
		qp->send(sendBuffer, sizeof(char), context->defaultRequestToken);
		context->defaultRequestToken->waitUntilCompleted();

		printf("Performing measurement\n");
		uint32_t rounds = (uint32_t) log2(MAX_BUFFER_SIZE);
		uint32_t messageSize = 1;

		for(uint32_t sizeIndex = 0; sizeIndex <= rounds; ++sizeIndex) {

			printf("Sending messages of size %d bytes\t", messageSize);
			fflush(stdout);

			struct timeval start;
			gettimeofday(&start, NULL);

			for(uint32_t i=0; i<OPERATIONS_COUNT; ++i) {
				if(i %BUFFER_COUNT == 0 || i == OPERATIONS_COUNT) {

					infinity::requests::RequestToken requestToken(context);
					qp->send(sendBuffer, messageSize, &requestToken);
					requestToken.waitUntilCompleted();

				} else {

					qp->send(sendBuffer, messageSize, NULL);

				}
			}

			struct timeval stop;
			gettimeofday(&stop, NULL);

			uint64_t time = timeDiff(stop, start);
			printf("total time : %ld\n", time);
			double msec = ((double)time) / 1000000L * 1000;

			double msgRate = ((double)(OPERATIONS_COUNT * 1000000L)) / time;
			double bandwidth = ((double) (OPERATIONS_COUNT * messageSize)) / (1024*1024) / (((double) time) / 1000000L);
			double latency = ((double) msec) / OPERATIONS_COUNT;
			printf("%.3f msg/sec\t%.3f MB/sec\n", msgRate, bandwidth);
			printf("latency : %.3fms\n", latency);
			fflush(stdout);

			messageSize *= 2;

		}

		printf("Waiting for notification from server\n");
		infinity::core::receive_element_t receiveElement;
		while (!context->receive(&receiveElement));

		delete receiveBuffer;
		delete sendBuffer;
	}

	delete qp;
	delete qpFactory;
	delete context;

}

uint64_t timeDiff(struct timeval stop, struct timeval start) {
	return (stop.tv_sec * 1000000L + stop.tv_usec) - (start.tv_sec * 1000000L + start.tv_usec);
}
