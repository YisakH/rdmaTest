#include <rdmaTcp.hpp>

rdmaTcp::rdmaTcp(string ip):manyToMany(ip){}

map<string, string> rdmaTcp::readRDMAInfo(int sock)
{
    map<string, string> info;
    string info_name[6] = {"addr", "len", "lkey", "rkey", "lid", "qp_num"};

    vector<int> sockList = getValidSock();
    if(sockList.size() <= 0){
        printf("유효한 소켓이 없습니다.\n");
        return {};
    }
    else{
        printf("sock size: %d\n", sockList.size());
    }

    for(int i = 0; i < 6; i++){

        char str[10];
        string result="";

        while(result.back() != '\n'){
            read(sock, str, 1);
            result += str[0];
        }
        info.insert({info_name[i], result});

        printf("%s : %s", info_name[i].c_str(), result.c_str());
    }
    printf("\n");

    return info;
}

void rdmaTcp::send_msg(int index, string msg_s)
{
    char msg[100];
    strcpy(msg, msg_s.c_str());

    if(index >= getValidSock().size()){
        printf("sock size error\n");
        error_handring("no valid sock input");
    }
    
    write(connect_clnt_sock[index], msg, strlen(msg));
        
    if(strcmp(msg, "exit") == 0)
        exit_call();
}

vector<int> rdmaTcp::getValidSock()
{
    return this->connect_clnt_sock;
}