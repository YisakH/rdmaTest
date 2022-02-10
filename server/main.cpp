#include "include/my/rdmaTcp.hpp"
#include "include/my/myrdma.hpp"
#include "ThreadPool.hpp"

#define NUM_DEST 3

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("argument must be required\n");
        exit(1);
    }

    printf("%s\n", argv[1]);
    printf("%s mode run\n", argv[2]);

    rdmaTcp myrdmaTcp(argv[1]);

    bool serverMode = false;
    bool clientMode = false;
    if(strcmp(argv[2], "-s") == 0)
    {
        serverMode = true;
    }
    else if(strcmp(argv[2], "-sc") == 0)
    {
        serverMode = true;
        clientMode = true;
    }
    else if(strcmp(argv[2], "-c") == 0)
    {
        clientMode = true;
    }

    printf("Server Mode : %s\n", (serverMode) ? "ON" : "OFF");
    printf("Client Mode : %s\n", (clientMode) ? "ON" : "OFF");
    

    myrdmaTcp.server();
    sleep(1);
    myrdmaTcp.client(NoOfNode);
    sleep(1);

    //-------------------------------------------------------------------------

    myRDMA myrdma[NUM_DEST];
    vector<int> sockList = myrdmaTcp.getValidSock();
    vector<map<string, string>> rdmaInfo;
    char send_buffer[4][1024];
    // char recv_buffer[4][1024];

    // char** send_buffer = new char*[4];
    // for(int i=0; i<4; i++)
    //     send_buffer[i] = new char[1024];

    for (int i = 0; i < sockList.size(); i++)
    {
        // char send_buffer[1024];
        myrdma[i].makeRDMAqp(send_buffer[i], sizeof(send_buffer));

        std::ostringstream oss;
        oss << &send_buffer[i];
        myrdmaTcp.send_msg(i, oss.str() + "\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.mr->length) + "\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.mr->lkey) + "\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.mr->rkey) + "\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.lid) + "\n");
        myrdmaTcp.send_msg(i, to_string(myrdma[i].rdmaBaseData.qp_num) + "\n");
    }

    sleep(2);

    // Send RDMA info

    // Read RDMA info

    for (int i = 0; i < sockList.size(); i++)
    {
        map<string, string> returnVal = myrdmaTcp.readRDMAInfo(sockList[i]);
        rdmaInfo.push_back(returnVal);
    }
    // map<string, string> rdmaInfo = myrdmaTcp.readRDMAInfo();

    // Exchange queue pair state
    for (int i = 0; i < sockList.size(); i++)
    {
        myrdma[i].changeQueuePairStateToInit(myrdma[i].rdmaBaseData.qp);
        myrdma[i].changeQueuePairStateToRTR(myrdma[i].rdmaBaseData.qp, PORT, stoi(rdmaInfo[i].find("qp_num")->second), stoi(rdmaInfo[i].find("lid")->second));
        myrdma[i].changeQueuePairStateToRTS(myrdma[i].rdmaBaseData.qp);
    }



    ThreadPool server_t(4);

    if (serverMode)
    {
        for(int i=0; i<sockList.size(); i++)
            myrdma[i].readRDMAMsg(sockList.size());
    }
    
    if(clientMode)
    { // client mode
        while (true)
        {
            char input[1024];
            cin.getline(input, sizeof(input));

            string sss = string(argv[1]) + " : " + input;
            for (int i = 0; i < 1; i++)          // 수정해야함 !!!!!
            {
                strcpy(send_buffer[i], input);
                myrdma[i].post_rdma_write(myrdma[i].rdmaBaseData.qp,
                                          myrdma[i].rdmaBaseData.mr,
                                          send_buffer[i], 
                                          sizeof(char) * 1024, 
                                          rdmaInfo[i].find("addr")->second, 
                                          rdmaInfo[i].find("rkey")->second);
                myrdma[i].pollCompletion(myrdma[i].rdmaBaseData.completion_queue);
                printf("send 폴링 완료\n");
            }
        }
    }

    while(true)
    {
        sleep(2);
    }

    for (int i = 0; i < sockList.size(); i++)
    {
        ibv_destroy_qp(myrdma[i].rdmaBaseData.qp);
        ibv_destroy_cq(myrdma[i].rdmaBaseData.completion_queue);
        ibv_dereg_mr(myrdma[i].rdmaBaseData.mr);
        ibv_dealloc_pd(myrdma[i].rdmaBaseData.protection_domain);
    }

    return 0;
}