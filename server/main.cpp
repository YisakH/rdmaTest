#include "include/my/rdmaTcp.hpp"
#include "include/my/rdma.hpp"

#define NUM_HOST 2

int main(int argc, char *argv[])
{
    if(argc <= 1){
        printf("argument must be required\n");
        exit(1);
    }

    rdmaTcp myrdmaTcp(argv[1]);

    printf("%s\n", argv[1]);

    myrdmaTcp.server();
    sleep(1);
    myrdmaTcp.client(NoOfNode);


    string input = "";


    sleep(2);

    //-------------------------------------------------------------------------


    RDMA rdma;
  
    struct ibv_context* context = rdma.createContext();
    struct ibv_pd* protection_domain = ibv_alloc_pd(context);
    int cq_size = 0x10;
    struct ibv_cq* completion_queue = ibv_create_cq(context, cq_size, nullptr, nullptr, 0);
    struct ibv_qp* qp = rdma.createQueuePair(protection_domain, completion_queue);

    char send_buffer[1024];
    struct ibv_mr *mr = rdma.registerMemoryRegion(protection_domain, send_buffer, sizeof(send_buffer));
    uint16_t lid = rdma.getLocalId(context, PORT);
    uint32_t qp_num = rdma.getQueuePairNumber(qp);

    //Send RDMA info
    std::ostringstream oss;
    oss << &send_buffer;
    myrdmaTcp.send_msg(oss.str()+"\n");
    myrdmaTcp.send_msg(to_string(mr->length)+"\n");
    myrdmaTcp.send_msg(to_string(mr->lkey)+"\n");
    myrdmaTcp.send_msg(to_string(mr->rkey)+"\n");
    myrdmaTcp.send_msg(to_string(lid)+"\n");
    myrdmaTcp.send_msg(to_string(qp_num)+"\n");

    //Read RDMA info
    vector<int> sockList = myrdmaTcp.getValidSock();
    
    vector<map<string,string>> rdmaInfo;

    for(int i=0; i<sockList.size(); i++){
        map<string,string> returnVal = myrdmaTcp.readRDMAInfo(sockList[i]);
        rdmaInfo.push_back(returnVal);
    }
    //map<string, string> rdmaInfo = myrdmaTcp.readRDMAInfo();

    //Exchange queue pair state
    for(int i=0; i<sockList.size(); i++){
        rdma.changeQueuePairStateToInit(qp);
        rdma.changeQueuePairStateToRTR(qp, PORT, stoi(rdmaInfo[i].find("qp_num")->second), stoi(rdmaInfo[i].find("lid")->second));
        rdma.changeQueuePairStateToRTS(qp);
    }

    sleep(10);
    cout << send_buffer << endl;
    
    ibv_destroy_qp(qp);
    ibv_destroy_cq(completion_queue);
    ibv_dereg_mr(mr);
    ibv_dealloc_pd(protection_domain);
    

    return 0;
}