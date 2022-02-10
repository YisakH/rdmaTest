#include "myrdma.hpp"
#include "thread"



void myRDMA::makeRDMAqp(char * send_buffer, int buffer_size)
{
    rdma = new RDMA;
    this->send_buffer = send_buffer;
    void *cq_context = NULL;

    rdmaBaseData.context = rdma->createContext();
    rdmaBaseData.protection_domain = ibv_alloc_pd(rdmaBaseData.context);
    rdmaBaseData.cq_size = 0x10;    
    rdmaBaseData.comp_channel = ibv_create_comp_channel(rdmaBaseData.context);


    rdmaBaseData.completion_queue = ibv_create_cq(rdmaBaseData.context, rdmaBaseData.cq_size, cq_context, rdmaBaseData.comp_channel, 1);
    rdmaBaseData.qp = rdma->createQueuePair(rdmaBaseData.protection_domain, rdmaBaseData.completion_queue);

    rdmaBaseData.mr = rdma->registerMemoryRegion(rdmaBaseData.protection_domain, send_buffer, buffer_size);
    rdmaBaseData.lid = rdma->getLocalId(rdmaBaseData.context, PORT);
    rdmaBaseData.qp_num = rdma->getQueuePairNumber(rdmaBaseData.qp);

}

void myRDMA::readRDMAMsg(int sizeofNode)
{
    thread t(&myRDMA::readRDMAMsg_t, this,   sizeofNode);

    t.join();
}

void myRDMA::readRDMAMsg_t(int sizeofNode)
{
    ibv_req_notify_cq(rdmaBaseData.completion_queue, 0);

    while (true)
    {
        void *cq_context;

        printf("수신 실행됨\n");
        tempRecv();

        ibv_get_cq_event(rdmaBaseData.comp_channel, &rdmaBaseData.completion_queue, &cq_context);
        ibv_req_notify_cq(rdmaBaseData.completion_queue, 0);

        pollCompletion(rdmaBaseData.completion_queue);
        
        for(int i=0; i<sizeofNode; i++){
            printf("%s\n", send_buffer);
        }
    }
    ibv_ack_cq_events(rdmaBaseData.completion_queue, 1);
}

int myRDMA::tempRecv()
{
    struct ibv_sge sge = {
        .addr = (uint64_t)(uintptr_t)send_buffer,
        .length = 1024,
        .lkey = rdmaBaseData.mr->lkey,
    };

    struct ibv_recv_wr recv_wr = {
        .wr_id = (uint64_t)(uintptr_t)sge.addr,
        .next = NULL,
        .sg_list = &sge,
        .num_sge = 1,
    };

    struct ibv_recv_wr *bad_wr;

    return ibv_post_recv(rdmaBaseData.qp, &recv_wr, &bad_wr);
}