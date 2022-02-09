#include "myrdma.hpp"
#include "thread"



void myRDMA::makeRDMAqp(char * send_buffer, int buffer_size)
{
    rdma = new RDMA;

    rdmaBaseData.context = rdma->createContext();
    rdmaBaseData.protection_domain = ibv_alloc_pd(rdmaBaseData.context);
    rdmaBaseData.cq_size = 0x10;
    rdmaBaseData.completion_queue = ibv_create_cq(rdmaBaseData.context, rdmaBaseData.cq_size, nullptr, nullptr, 0);
    rdmaBaseData.qp = rdma->createQueuePair(rdmaBaseData.protection_domain, rdmaBaseData.completion_queue);

    rdmaBaseData.mr = rdma->registerMemoryRegion(rdmaBaseData.protection_domain, send_buffer, buffer_size);
    rdmaBaseData.lid = rdma->getLocalId(rdmaBaseData.context, PORT);
    rdmaBaseData.qp_num = rdma->getQueuePairNumber(rdmaBaseData.qp);
    
    rdmaBaseData.comp_channel = ibv_create_comp_channel(rdmaBaseData.context);
}

void myRDMA::readRDMAMsg(char **send_buffer, int sizeofNode)
{
    thread t(&readRDMAMsg_t, send_buffer, sizeofNode);

    t.detach();
}

void myRDMA::readRDMAMsg_t(char **send_buffer, int sizeofNode)
{
    while (true)
    {
        ibv_get_cq_event(rdmaBaseData.comp_channel, &rdmaBaseData.completion_queue, (void **) &rdmaBaseData.context);
        
        for(int i=0; i<sizeofNode; i++){
            printf("%s\n", send_buffer[i]);
        }
    }
}