#include "rdma.hpp"

class myRDMA : public RDMA
{
public:
    RDMA *rdma;
    char * send_buffer;
    char * recv_buffer;

    struct rdmaBaseData
    {
        RDMA rdma;
  
        struct ibv_context* context;
        struct ibv_pd* protection_domain;
        int cq_size;
        struct ibv_cq* send_cq;
        struct ibv_cq* recv_cq;
        struct ibv_qp* qp;

        char send_buffer[1024];
        char recv_buffer[1024];
        struct ibv_mr *mr;
        uint16_t lid;
        uint32_t qp_num;

        struct ibv_comp_channel* comp_channel;
    }rdmaBaseData;

    myRDMA()
    {
    }

    void makeRDMAqp(char * send_buffer, char *recv_buffer, int buffer_size);

    void readRDMAMsg(int sizeofNode);

    void readRDMAMsg_t(int sizeofNode);    

    int tempRecv();
};