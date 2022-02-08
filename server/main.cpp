#include "include/my/rdmaTcp.hpp"
#include "include/my/myrdma.hpp"

#define NUM_DEST 1

int main(int argc, char *argv[])
{
    if(argc <= 1){
        printf("argument must be required\n");
        exit(1);
    }

    printf("%s\n", argv[1]);
    rdmaTcp myrdmaTcp(argv[1]);

    myrdmaTcp.server();
    sleep(1);
    myrdmaTcp.client(NoOfNode);
    sleep(1);

    //-------------------------------------------------------------------------


    myRDMA myrdma[NUM_DEST];
    vector<int> sockList = myrdmaTcp.getValidSock();
    vector<map<string,string>> rdmaInfo;
    char send_buffer[4][1024];

    for(int i=0; i<NUM_DEST; i++){
        //char send_buffer[1024];
        myrdma[i].makeRDMAqp(send_buffer[i], sizeof(send_buffer));

        std::ostringstream oss;
        oss << &send_buffer[i];
        myrdmaTcp.send_msg(i, oss.str()+"\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.mr->length)+"\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.mr->lkey)+"\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.mr->rkey)+"\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.lid)+"\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.qp_num)+"\n");
    }
    

    //Send RDMA info

    //Read RDMA info

    for(int i=0; i<sockList.size(); i++){
        map<string,string> returnVal = myrdmaTcp.readRDMAInfo(sockList[i]);
        rdmaInfo.push_back(returnVal);
    }
    //map<string, string> rdmaInfo = myrdmaTcp.readRDMAInfo();

    //Exchange queue pair state
    for(int i=0; i<sockList.size(); i++){
        myrdma[i].changeQueuePairStateToInit(myrdma[i].rdmaBaseData.qp);
        myrdma[i].changeQueuePairStateToRTR(myrdma[i].rdmaBaseData.qp, PORT, stoi(rdmaInfo[i].find("qp_num")->second), stoi(rdmaInfo[i].find("lid")->second));
        myrdma[i].changeQueuePairStateToRTS(myrdma[i].rdmaBaseData.qp);
    }

    sleep(10);

    for(int i=0; i<sockList.size(); i++){
        cout << send_buffer[i] << endl;
    }

    
    for(int i=0; i<sockList.size(); i++){
        ibv_destroy_qp(myrdma[i].rdmaBaseData.qp);
        ibv_destroy_cq(myrdma[i].rdmaBaseData.completion_queue);
        ibv_dereg_mr(myrdma[i].rdmaBaseData.mr);
        ibv_dealloc_pd(myrdma[i].rdmaBaseData.protection_domain);
    }

    return 0;
}