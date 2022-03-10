#include "include/my/rdmaTcp.hpp"
#include "include/my/myrdma.hpp"
#include "include/tools/ThreadPool.hpp"

#define PORT_NUMBER 40201
#define SERVER_IP "192.0.0.1"

#define NUM_DEST 3

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("argument must be required\n");
        exit(1);
    }

    const char *myip = argv[1];
    const char *mode = argv[2];

    //myip = "192.168.0.100";
    //mode = "-sc";
    printf("%s\n", myip);
    printf("%s mode run\n", mode);

    bool serverMode = false;
    bool clientMode = false;

    if(strcmp(mode, "-s") == 0)
    {
        serverMode = true;
    }
    else if(strcmp(mode, "-sc") == 0)
    {
        serverMode = true;
        clientMode = true;
    }
    else if(strcmp(mode, "-c") == 0)
    {
        clientMode = true;
    }

    printf("Server Mode : %s\n", (serverMode) ? "ON" : "OFF");
    printf("Client Mode : %s\n", (clientMode) ? "ON" : "OFF");
    

    // tcp connect
    rdmaTcp myrdmaTcp(myip);
    myrdmaTcp.server();
    sleep(1);
    myrdmaTcp.client(NoOfNode);
    sleep(1);

    
    // Send RDMA info
    myRDMA myrdma[NUM_DEST];
    vector<int> sockList = myrdmaTcp.getValidSock();
    vector<map<string, string>> rdmaInfo;
    char send_buffer[NUM_DEST][1024];
    char recv_buffer[NUM_DEST][1024];

    for (int i = 0; i < sockList.size(); i++)
    {
        myrdma[i].makeRDMAqp(send_buffer[i], recv_buffer[i], sizeof(send_buffer));

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


    // Read RDMA info
    for (int i = 0; i < sockList.size(); i++)
    {
        map<string, string> returnVal = myrdmaTcp.readRDMAInfo(sockList[i]);
        rdmaInfo.push_back(returnVal);
    }

    // Exchange queue pair state
    for (int i = 0; i < sockList.size(); i++)
    {
        myrdma[i].changeQueuePairStateToInit(myrdma[i].rdmaBaseData.qp);
        myrdma[i].changeQueuePairStateToRTR(myrdma[i].rdmaBaseData.qp, PORT,
                                            stoi(rdmaInfo[i].find("qp_num")->second),
                                            stoi(rdmaInfo[i].find("lid")->second));
        myrdma[i].changeQueuePairStateToRTS(myrdma[i].rdmaBaseData.qp);
    }



    // Run Server (msg recv)
    ThreadPool server_t(sockList.size());
    if (serverMode)
    {
        printf("server mode run\n");
        for(int i=0; i<sockList.size(); i++)
            server_t.EnqueueJob([&myrdma, i]()
                                { while(true) {myrdma[i].tempRecv();} });
    }
    
    // RUn Client (msg send)
    if(clientMode)
    {
        printf("client mode run\n");
        while (true)
        {
            char input[1024];
            cin.getline(input, sizeof(input));

            string sss = string(myip) + " : " + input;
            for (int i = 0; i < sockList.size(); i++)
            {
                strcpy(send_buffer[i], sss.c_str());
                myrdma[i].post_rdma_write(myrdma[i].rdmaBaseData.qp,
                                          myrdma[i].rdmaBaseData.mr,
                                          send_buffer[i], 
                                          sizeof(char) * 1024, 
                                          rdmaInfo[i].find("addr")->second, 
                                          rdmaInfo[i].find("rkey")->second);
                myrdma[i].pollCompletion(myrdma[i].rdmaBaseData.send_cq);
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
        ibv_destroy_cq(myrdma[i].rdmaBaseData.send_cq);
        ibv_destroy_cq(myrdma[i].rdmaBaseData.recv_cq);
        ibv_dereg_mr(myrdma[i].rdmaBaseData.mr);
        ibv_dealloc_pd(myrdma[i].rdmaBaseData.protection_domain);
    }

    return 0;
}