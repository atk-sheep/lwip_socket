#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <errno.h>

#define PORT 6789

typedef struct sockaddr sockaddr;

typedef struct sockaddr_in sockaddr_in;

void print_addr(sockaddr_in* addr){

    char ipAddress[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &(addr->sin_addr), ipAddress, INET_ADDRSTRLEN);

    printf("remote ip: %s\n", ipAddress);

    uint16_t port = ntohs(addr->sin_port);

    printf("remote port: %d\n", port);

    return;
}

void udp_demo(){
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

   struct sockaddr_in local_addr;
   local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //inet_pton(AF_INET, "192.168.1.5", &local_addr.sin_addr.s_addr);
    local_addr.sin_port = htons(PORT);

    if(bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr))< 0){
        printf("bind error\n");
        return;
    }

    struct sockaddr_in addr;
   addr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.1.10", &addr.sin_addr.s_addr);
    addr.sin_port = htons(12345);

    struct sockaddr_in remote_addr;

    socklen_t remote_len = sizeof(remote_addr);

    int len = -1;

    char buf[50];

    while((len = recvfrom(sock, buf, 50, 0, (struct sockaddr*)&remote_addr, &remote_len)) > 0)
    {
        printf("len:%d ", len);
        
        buf[len] = '\0';
        printf("recv: %s\n", buf);
        
        buf[len] = 'E';

        if(remote_addr.sin_family == AF_INET){
            //printf("yes\n");
        }
        else{
            printf("no\n");
        }

        print_addr(&remote_addr);

        remote_addr.sin_family = AF_INET;

        if(sendto(sock, buf, len+1, 0, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0){
            perror("send failed: ");
        }
    }
    
    close(sock);
}

void tcp_server(){
    int lsock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    //local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, "192.168.1.5", &local_addr.sin_addr.s_addr);
    local_addr.sin_port = htons(6789);

    if(bind(lsock, (sockaddr *)&local_addr, sizeof(local_addr)) < 0){
        perror("bind failed!!!");
    }

    listen(lsock, 10);

    sockaddr_in csock;
    socklen_t clen = sizeof(csock);
    char buf[20];
    int sessionfd = -1;
    while((sessionfd = accept(lsock, (sockaddr *)&csock, &clen)) < 0){
        fprintf(stderr, "%s: %s\n", "accept error", strerror(errno));
    }

    print_addr(&csock);

    while (1)
    {
        int len = read(sessionfd, buf, 20);

        if(len == 0){
            printf("remote close!\n");
            close(sessionfd);
            break;
        }
        else if(len < 0){
            //阻塞模式对端close socket， RST
            close(sessionfd);
            break;
        }
        else{
            buf[len] = '\0';
            printf("read: %s\n", buf);
        }

        buf[len] = 'E';

        if(write(sessionfd, buf, len) < 0){
            perror("write failed!");
            close(sessionfd);
            break;
        }
        else{
            buf[len+1] = '\0';
            printf("write: %s", buf);
        }
    }
    
    close(lsock);
    return;
}

#define LWIP_SEND_DATA 0x80
uint8_t udp_flag = 0x80;

void tcp_client(){
    int localsock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.1.10", &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(12345);

    // sockaddr_in local_addr;
    // local_addr.sin_family = AF_INET;
    // inet_pton(AF_INET, "192.168.1.5", &local_addr.sin_addr.s_addr);
    // local_addr.sin_port = htons(5678);

    // if(bind(localsock, (sockaddr*)&local_addr, sizeof(local_addr)) < 0){
    //     perror("bind failed!");
    //     close(localsock);
    //     return;
    // }

    int ret = -1;
    if((ret = connect(localsock, (sockaddr *)&server_addr, sizeof(server_addr))) < 0){
        perror("connect failed!");
        close(localsock);
        return;
    }

    int index = 0;
    uint8_t *txbuffer = (uint8_t*)malloc(sizeof(uint8_t)*50);
    uint8_t *rxbuffer = (uint8_t*)malloc(sizeof(uint8_t)*50);
    memset(txbuffer, 0, 50);
    memset(rxbuffer, 0, 50);

    while(1){
      if ((udp_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
      {
        char buffer[20];

        char letter = (uint8_t)(index + 0x30);

        sprintf(buffer, "hello world! %c", letter);

        int len = strlen(buffer);

        buffer[len] = '\0';

        memcpy(txbuffer, buffer, len+1);

        if(send(localsock, txbuffer, len, 0) < 0){   //最后flags 置0，同write
          printf("send failed!!!\r\n");
          break;
        }

        printf("tcp client send: %s\r\n", buffer);

        index = index>=9 ? 0: (++index);

        udp_flag &= ~LWIP_SEND_DATA;	//清除数据发送标志
      }

      int  recv_len  =  recv(localsock,  rxbuffer,  50,  0);

      if(recv_len > 0 && recv_len <= 50){
        rxbuffer[recv_len] = '\0';

        printf("tcp client recieve reply from server: %s\r\n", rxbuffer);
      }

      udp_flag = LWIP_SEND_DATA;

      sleep(2);
      //delay(500); //500*5 ms
    }

    close(localsock);
}


int main(){
    tcp_client();
    
    return 0;
}