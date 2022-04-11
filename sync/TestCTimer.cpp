//
// Created by xiaolei on 2022/2/27.
//


#include "CTimer.h"

//定时器任务函数
void didHeartbeatThread(void *arg){
    printf("run once \n");
}

int main(){

    CTimer *pTimer = new CTimer("定时器2");
    pTimer->AsyncLoop(1000, didHeartbeatThread, 10, pTimer);	//异步循环执行，间隔时间10毫秒

    return 0;
}
