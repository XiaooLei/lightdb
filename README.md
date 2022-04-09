# lightdb


测试说明：在clone代码编译测试之后，需要先创建目录/tmp/lightdb 作为数据文件存储目录

编译：clone之后，执行./build.sh

下面是压力测试实际表现，测试数据量均为100w。


### String

测试代码：

|  command   | qps  | 数据量 
|  ----  | ----  | ---- 
| strset  | 140984 | 100w
| strget  | 104220 | 100w
## List

### listLpush
测试代码：
```cpp
int main(){
    LightDB* lightdb = new lightdb::LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);

    //1kw次写
    uint64_t start_t = getCurrentTimeStamp();
    int keys = 10000;
    int values_per_list = 1000;
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


## Set

测试代码：
```cpp
#include "../../lightdb.h"
using namespace lightdb;
int main(){
    LightDB* lightdb = new lightdb::LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);

    //1kw 次写
    uint64_t start_t = getCurrentTimeStamp();
    int keys = 1000;
    int values_per_set = 1000;
    for(int i = 0; i < keys; i++){
        std::string key = "setkey-" + to_string(i);
        for(int i = 0; i < values_per_set; i++) {
            std::string value = "setvalue-" + to_string(i);
            int length;
            Status s;
            s = lightdb->SAdd(key, value, length);
        }
    }
    uint64_t end_t = lightdb::getCurrentTimeStamp();
    for(int i = 0; i < keys; i++){
        std::string key = "listkey-" + to_string(i);
        Status s;
        int count = 0;
        s = lightdb->SClear(key, count);
    }
    double elapsed = end_t - start_t;
    printf(" %d times write，time_spend:%lu microseconds \n", keys * values_per_set, elapsed);
    double seconds = elapsed / 1000;
    double qps = keys * values_per_set / seconds;
    printf("qps :% f write/s \n", qps);
    return 0;

}

```
说明：参数keys表示测试中key的数量，参数values_per_set表示每个key的大小

测试结果
|  command   | qps  | 数据量  | keys  | values_per_set|
|  ----  | ----  | ---- |----| ---- |
| SAdd  | 502260 | 100w |10000 | 100|
| SAdd  | 491883 | 100w |5000 | 200|
| SAdd  | 456621 | 100w |2500 | 400|
| SAdd  | 490677 | 100w |1000 | 1000|
| SAdd  | 502260 | 100w |500 | 2000|

## ZSet

测试结果
|  command   | qps  | 数据量  | keys  | values_per_zset|
|  ----  | ----  | ---- |----| ---- |
| SAdd  | 283687 | 100w |10000 | 100|
| SAdd  | 160102 | 100w |5000 | 200|
| SAdd  | 95210 | 100w |2500 | 400|
| SAdd  | 711743 | 100w |1000 | 1000|
| SAdd  | 87519 | 100w |500 | 2000|

## ZScore

|  command   | qps  | 数据量  | keys  | values_per_zset|
|  ----  | ----  | ---- |----| ---- |
| SAdd  | 586510 | 100w |1000 | 1000|













