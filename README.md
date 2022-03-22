# lightdb


下面是压力测试实际表现，测试数据量均为100w。




### String

|  command   | qps  | 数据量 
|  ----  | ----  | ---- 
| strget  | 157579 | 100w
| strget  | 109361 | 100w
### List
测试代码：
```
int main(){
    LightDB* lightdb = new lightdb::LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);

    //1kw 次写
    uint64_t start_t = getCurrentTimeStamp();
    int keys = 10000;
    int values_per_list = 100;
    for(int i = 0; i < keys; i++){
        std::string key = "listkey-" + to_string(i);
        for(int i = 0; i < values_per_list; i++) {
            std::string value = "listvalue-" + to_string(i);
            int length;
            Status s;
            s = lightdb->LPush(key, value, length);
            if (s.ok()) {
                printf("");
            }
        }
    }
    uint64_t end_t = lightdb::getCurrentTimeStamp();
    double elapsed = end_t - start_t;
    printf(" %d times write，time_spend:%lu microseconds \n", keys * values_per_list, elapsed);
    double seconds = elapsed / 1000;
    double qps = keys * values_per_list / seconds;
    printf("qps :% f write/s \n", qps);

    return 0;
}

```
上面代码中keys表示测试中list的个数， values_per_list 表示平均每个list的大小，当参与测试的list较大时， 
大量的时间开销在于内存中list的维护，下面列出当两个参数变化时候的qps

|  command   | qps  | 数据量  | keys  | values_per_list|
|  ----  | ----  | ---- |----| ---- |
| listLPush  | 167616 | 100w |10000 | 100|
| listLPush  | 149611 | 100w |5000 | 200|
| listLPush  | 136742 | 100w |2500 | 400|
| listLPush  | 79472 | 100w |1000 | 1000|







