#include "myrdma.hpp"



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
}