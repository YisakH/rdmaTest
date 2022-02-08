#include "rdma.hpp"

class myRDMA : public RDMA
{
public:
    RDMA *rdma;
    struct rdmaBaseData
    {
        RDMA rdma;
  
        struct ibv_context* context;
        struct ibv_pd* protection_domain;
        int cq_size;
        struct ibv_cq* completion_queue;
        struct ibv_qp* qp;

        char send_buffer[1024];
        struct ibv_mr *mr;
        uint16_t lid;
        uint32_t qp_num;
    }rdmaBaseData;

    myRDMA()
    {
    }

    void makeRDMAqp(char * send_buffer, int buffer_size);

    

};