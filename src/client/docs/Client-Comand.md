## Client 信号

### 请求连接 CONNECT_REQ

服务端接收信号后发送 `CONNECT_REP` 信号回复

### 请求中断连接 CONNECT_INTERRUPTED

服务端收到请求后不再返回 `CONNECT_ACK` 信号

否则在发送 `CONNECT_REP` 间隔 5s 后发送 `CONNECT_ACK` 信号