#include <rdmaTcp.hpp>

rdmaTcp::rdmaTcp(string ip):manyToMany(ip){}

map<string, string> rdmaTcp::readRDMAInfo()
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
        printf("받기 시작\n");
        string result="";

        while(result.back() != '\n'){
            read(sockList.back(), str, 1);
            result += str[0];
        }
        info.insert({info_name[i], result});

        printf("받은 문자열은 %s\n", result.c_str());
    }

    return info;
}

vector<int> rdmaTcp::getValidSock()
{
    return this->connect_clnt_sock;
}