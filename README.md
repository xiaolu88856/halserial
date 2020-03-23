# libserial

## 说明

目前有多种通信管理机：

* S3100C2E1
* S3100C4E2
* S3100C8E2
* S3100C16E2BJ
* SC10
* S20C4
* S20C8
* ...

不同型号对应的串口号几乎都不同，为简化设计，避免重复代码，特此编写了libserial通用串口库。

使用libserial可以方便的创建串口、打开串口、读写串口，最大化的降低用户操作。

## 编译方法

### lib库编译

直接运行`./compile_so.sh`,就在当前目录下生成libserial.so

### 测试编译

1. 运行`make`，就在当前目录下生成libserial可执行文件

2. 将可执行文件复制到通信管理机的`$project/bin` 目录下

3. 运行`./libserial COM1`即可

## API

### 串口创建、销毁

```serial_create
serial_t* serial_create(const char *com,
                         int baud, char parity, int data_bit,
                         int stop_bit);

函数说明：创建串口句柄，将串口参数绑定
参数：
com      -- 端口号，COM1,COM2....COM16
baud     -- 波特率，1200-115200
parity   -- 奇偶检验位，'N','E','O'
data_bit -- 数据位
stop_bit -- 停止位
return   -- 串口句柄，创建失败返回NULL
```

```serial_release
void serial_release(serial_t* serial);
```

### 串口打开、关闭

```serial_open
int serial_open(serial_t* serial);

函数说明：打开串口，如果是485设备，将打开GPIO
```

```serial_close
void serial_close(serial_t* serial);

函数说明：关闭串口，如果是485设备，将同时关闭GPIO
```

### 串口发送

```serial_write
int serial_write(serial_t* serial, uint8_t *msg, int msg_len);

函数说明：将数据写入串口
参数：
serial  -- 串口句柄
msg     -- 缓存区
msg_len -- 待写入数据长度
return  -- -1(写入失败)，>0(写入成功)
```

### 串口预读

```serial_select
int serial_select(serial_t* serial,int timeout);

函数说明：在设定时间内，判断是否有数据可读
参数：
serial  -- 串口句柄
timeout -- 读超时，单位ms
return  -- 0(超时)， > 0(有数据)， < 0(错误)
```

### 串口读取

```serial_read
int serial_read(serial_t* serial, uint8_t *msg, int msg_len);

函数说明：非阻塞读取串口数据
参数：
serial  -- 串口句柄
msg     -- 数据缓存
msg_len -- 希望读取的数据长度
return  -- 返回实际读取的长度
```