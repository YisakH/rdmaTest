#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cassert>

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePairFactory.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>
#include <infinity/memory/RegionToken.h>
#include <infinity/requests/RequestToken.h>

#define PORT_NUMBER 40201
#define SERVER_IP "192.168.0.100"

// Usage: ./progam -s for server and ./program for client component
int main(int argc, char **argv) {

	bool isServer = false;

	while (argc > 1) {
		if (argv[1][0] == '-') {
			switch (argv[1][1]) {

				case 's': {
					isServer = true;
					break;
				}

			}
		}
		++argv;
		--argc;
	}

	infinity::core::Context *context = new infinity::core::Context();
	infinity::queues::QueuePairFactory *qpFactory = new  infinity::queues::QueuePairFactory(context);
	infinity::queues::QueuePair *qp;

	if(isServer) {
		char input[1024];

		printf("Creating buffers to read from and write to\n");
		infinity::memory::Buffer *bufferToReadWrite = new infinity::memory::Buffer(context, input, sizeof(input));
		infinity::memory::RegionToken *bufferToken = bufferToReadWrite->createRegionToken();

		printf("Creating buffers to receive a message\n");
		infinity::memory::Buffer *bufferToReceive = new infinity::memory::Buffer(context, input, sizeof(input));
		context->postReceiveBuffer(bufferToReceive);

		printf("Setting up connection (blocking)\n");
		qpFactory->bindToPort(PORT_NUMBER);
		qp = qpFactory->acceptIncomingConnection(bufferToken, sizeof(infinity::memory::RegionToken));

		printf("Waiting for message (blocking)\n");
		infinity::core::receive_element_t receiveElement;
		while(!context->receive(&receiveElement));

		printf("receive msg : %s\n", input);
		printf("Message received\n");
		delete bufferToReadWrite;
		delete bufferToReceive;

	} else {
		
		char output[1024];
		scanf("%[^\n]s", output);

		printf("Connecting to remote node\n");
		qp = qpFactory->connectToRemoteHost(SERVER_IP, PORT_NUMBER);
		infinity::memory::RegionToken *remoteBufferToken = (infinity::memory::RegionToken *) qp->getUserData();


		printf("Creating buffers\n");
		infinity::memory::Buffer *buffer1Sided = new infinity::memory::Buffer(context, output, sizeof(output));
		infinity::memory::Buffer *buffer2Sided = new infinity::memory::Buffer(context, output, sizeof(output));

		
		printf("Reading content from remote buffer\n");
		infinity::requests::RequestToken requestToken(context);
		/*
		qp->read(buffer1Sided, remoteBufferToken, &requestToken);
		requestToken.waitUntilCompleted();
		*/
		printf("Writing content to remote buffer\n");
		qp->write(buffer1Sided, remoteBufferToken, &requestToken);
		requestToken.waitUntilCompleted();
		

		printf("Sending message to remote host\n");
		qp->send(buffer2Sided, &requestToken);
		requestToken.waitUntilCompleted();

		delete buffer1Sided;
		delete buffer2Sided;

	}

	delete qp;
	delete qpFactory;
	delete context;

	return 0;

}