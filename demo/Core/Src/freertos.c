/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pcf8574.h"
#include <stdio.h>
#include "iwdg.h"
#include "w25q256.h"
#include <stdbool.h>
#include "usart.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include <string.h>
#include "lwip/sockets.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
const u8 TEXT_Buffer[]={"New SP Embeded QSPI TEST"};
#define SIZE sizeof(TEXT_Buffer)
uint8_t rxbuf[17] = {0};
uint8_t net_init_flag;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId dmaReceiveTaskHandle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId feedDogTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void test_w25Q256(void){
  u32 flash_size=32*1024*1024;	//FLASH 大�?为32M字节
  u8 datatemp[SIZE] = {0};
  printf("Start write data to W25Q256.... \r\n");
  W25Q256_Write((u8*)TEXT_Buffer,flash_size-100,SIZE);
  printf("W25Q256 Write Finished!\r\n");	//�??示传�?完�?
  printf("Start Read W25Q256.... \r\n");
  W25Q256_Read(datatemp, flash_size-100, SIZE);					//从倒数第100个地�?�处开始,读出SIZE个字节
  printf("Get data from w25Q256: %s\r\n", datatemp);
}

void StartTask03(void const * argument);

static void udp_thread();

static void tcp_client_thread();

static void tcp_server_thread();

int set_noblock(int sock);

void set_timeout(int sockfd, long t);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 4096);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of feedDogTask */
  osThreadDef(feedDogTask, StartTask02, osPriorityIdle, 0, 2048);
  feedDogTaskHandle = osThreadCreate(osThread(feedDogTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(dmaReceiveTask, StartTask03, osPriorityIdle, 0, 2048);
  dmaReceiveTaskHandle = osThreadCreate(osThread(dmaReceiveTask), NULL);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */

  net_init_flag = 1;

  W25Q256_Init();

  while(W25Q256_ReadID()!=W25Q256)								//检测�?到W25Q256
	{
		printf("QSPI Check Failed!");
		HAL_Delay(1000);
		printf("Please Check!\n\n\n\n");
		HAL_Delay(1000);
	}
  printf("QSPI Ready!\r\n");

  test_w25Q256();

   uint32_t bootaddr = READ_REG(FLASH->OPTCR1);

   printf("bootaddr: %x\r\n", bootaddr);

  for(;;)
  {
	  HAL_GPIO_WritePin(GPIOB, LED0_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOB, LED1_Pin, GPIO_PIN_SET);
	  osDelay(500);
	  HAL_GPIO_WritePin(GPIOB, LED0_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOB, LED1_Pin, GPIO_PIN_RESET);
	  osDelay(500);

	  printf("hello\r\n");
    
    printf("world\r\n");

  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the feedDogTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  
  //等待网络初始化
   while (!net_init_flag)
   {
     printf("not init\r\n");
   }
  //osDelay(1000);
  
  /* Infinite loop */
  for(;;)
  {
    // HAL_IWDG_Refresh(&hiwdg);

    // printf("refresh iwdg\r\n");

    // osDelay(6000);  //看门狗为8s左�?�

	  //udp_thread();

    //tcp_client_thread();

    tcp_server_thread();
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void StartTask03(void const * argument)
{
  static bool flag = true;
  rxbuf[16] = '\0';   //字符串输出最后一字节\0
  /**
   * @brief DMA接收
   * 
   */
  HAL_UART_Receive_DMA(&huart1, rxbuf, sizeof(rxbuf)-1);

  osDelay(50);

  printf("reveive dma state: %d\r\n", hdma_usart1_rx.State);

  while(1){
    if(hdma_usart1_rx.State == HAL_DMA_STATE_READY)
		{
      printf("reveive string: %s\r\n", rxbuf);
      HAL_UART_Receive_DMA(&huart1, rxbuf, sizeof(rxbuf)-1);
    }
  }

  /**
   * @brief 中断接收
   * 
   */
  // HAL_UART_Receive_IT(&huart1, rxbuf, sizeof(rxbuf));

  // osDelay(50);

  // printf("reveive dma state: %x\r\n", huart1.RxState);

  // while(1){
  //   if(huart1.RxState == HAL_UART_STATE_READY)
	// 	{
  //     printf("reveive string: %s\r\n", rxbuf);
  //     while(huart1.RxState != HAL_UART_STATE_BUSY_RX){
  //       HAL_UART_Receive_IT(&huart1, rxbuf, sizeof(rxbuf));
  //     }
  //     // not work
  //     //while(HAL_UART_Receive_IT(&huart1, rxbuf, sizeof(rxbuf)) != HAL_OK) ;
  //   }
    
  // }
  
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    //数据处理
}

//udp 任务函数
#define UDP_PORT 12345
#define SERVER_PORT 6789
#define SERVER_IP "192.168.1.5"
#define LWIP_SEND_DATA 0x80

u8 udp_flag = 0x80;

static void udp_thread()
{
  struct sockaddr_in server_addr, local_addr;
  int local_sockid = -1;
  int index = 0;

  // 发送与接收缓冲
  char* txbuffer = (char *)pvPortMalloc(50*sizeof(char));
  char* rxbuffer = (char *)pvPortMalloc(50*sizeof(char));
  memset(txbuffer, 0, 50);
  memset(rxbuffer, 0, 50);

  local_sockid = socket(AF_INET, SOCK_DGRAM, 0);  //传0，使用默认协议
  if(local_sockid < 0){
    return;
  }

  //可以不bind
  memset(&local_addr, 0, sizeof(struct sockaddr_in));
  local_addr.sin_family  =  AF_INET;
  local_addr.sin_addr.s_addr  =  htonl(INADDR_ANY);
  local_addr.sin_port  =  htons(UDP_PORT);

  if(bind(local_sockid, (struct sockaddr *)&local_addr, sizeof(local_addr) ) < 0){
    printf("bind failed!!!\r\n");
    return;
  }

  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);
  server_addr.sin_port = htons(SERVER_PORT);

  while (1)
  {
      if ((udp_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
      {
        char buffer[20];

        char letter = (u8)(index + 0x30);

        sprintf(buffer, "hello world! %c", letter);

        int len = strlen(buffer);

        buffer[len] = '\0';

        memcpy(txbuffer, buffer, len+1);

        if(sendto(local_sockid, txbuffer, len, 0, &server_addr, sizeof(server_addr)) < 0){
          printf("send failed!!!\r\n");
          continue;
        }

        printf("UDP send: %s\r\n", buffer);

        index = index>=9 ? 0: (++index);

        udp_flag &= ~LWIP_SEND_DATA;	//清除数据发送标志
      }

      struct sockaddr_in remote_addr;

      socklen_t remote_len;

      int  recv_len  =  recvfrom(local_sockid,  rxbuffer,  50,  0,  (struct  sockaddr  *)&remote_addr,  &remote_len);

      if(recv_len > 0 && recv_len <= 50){
        rxbuffer[recv_len] = '\0';

        printf("UDP recieve reply from server: %s\r\n", rxbuffer);
      }

      udp_flag = LWIP_SEND_DATA;

      osDelay(2000);

  }
}

//  设置套接字为非阻塞模式
int set_noblock(int sock){
  int  flags  =  fcntl(sock,  F_GETFL,  0);
  if  (flags  ==  -1)  {
      //  错误处理
      close(sock);
      return  -1;
  }
  flags  |=  O_NONBLOCK;
  if  (fcntl(sock,  F_SETFL,  flags)  ==  -1)  {
      //  错误处理
      close(sock);
      return  -1;
  }
  return 0;
}

void set_timeout(int sockfd, long t){
  //  设置超时
  struct  timeval  timeout;
  timeout.tv_sec  =  t  /  1000;
  timeout.tv_usec  =  (t  %  1000)  *  1000;
  setsockopt(sockfd,  SOL_SOCKET,  SO_SNDTIMEO,  (const  char*)&timeout,  sizeof(timeout));
  setsockopt(sockfd,  SOL_SOCKET,  SO_RCVTIMEO,  (const  char*)&timeout,  sizeof(timeout));

  return;
}


//tcp 客户端函数
static void tcp_client_thread(){
  struct sockaddr_in server_addr, local_addr;
  int local_sockid = -1;
  int index = 0;

  // 发送与接收缓冲
  char* txbuffer = (char *)pvPortMalloc(50*sizeof(char));
  char* rxbuffer = (char *)pvPortMalloc(50*sizeof(char));
  memset(txbuffer, 0, 50);
  memset(rxbuffer, 0, 50);

  local_sockid = socket(AF_INET, SOCK_STREAM, 0);  //传0，使用默认协议
  if(local_sockid < 0){
    return;
  }

  // if(set_noblock(local_sockid) < 0){
  //   printf("set noblock failed!\r\n");
  //   return;
  // }

  set_timeout(local_sockid, 2000);

  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);
  server_addr.sin_port = htons(SERVER_PORT);

  int ret = 0;

  if((ret = connect(local_sockid, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0){
    if(errno == EINPROGRESS){
      printf("connect in progresss\r\n");
      //wait for a while ?
      return;
    }
    
    printf("connect to server failed!!!\r\n");
    osDelay(1000);
    close(local_sockid);
    return;
  }

  while(1){
      if ((udp_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
      {
        char buffer[20];

        char letter = (u8)(index + 0x30);

        sprintf(buffer, "hello world! %c", letter);

        int len = strlen(buffer);

        buffer[len] = '\0';

        memcpy(txbuffer, buffer, len+1);

        if(send(local_sockid, txbuffer, len, 0) < 0){   //最后flags 置0，同write
          printf("send failed!!!\r\n");
          //continue;
        }

        printf("tcp client send: %s\r\n", buffer);

        index = index>=9 ? 0: (++index);

        udp_flag &= ~LWIP_SEND_DATA;	//清除数据发送标志
      }

      int  recv_len  =  recv(local_sockid,  rxbuffer,  50,  0);

      if(recv_len > 0 && recv_len <= 50){
        rxbuffer[recv_len] = '\0';

        printf("tcp client recieve reply from server: %s\r\n", rxbuffer);
      }

      udp_flag = LWIP_SEND_DATA;

      osDelay(2000);
  }


}

void servertask(void const * argument)
{


}

static void handle_client(int new_sock){
  // osThreadId serverTaskHandle;

  // osThreadDef(serverTask, servertask, osPriorityIdle, 0, 2048);
  // serverTaskHandle = osThreadCreate(osThread(servertask), NULL);

  char *buffer = (char*)pvPortMalloc(20);

  while(1){
    int len = recv(new_sock, buffer, 20, 0);

    if(len>0){
      buffer[len] = '\0';

      printf("recv from server: %s\r\n", buffer);

      buffer[len] = 'E';
      buffer[len+1] = '\0';

      send(new_sock, buffer, len+1, 0);
    }
    else{
      printf("recv nothing!\r\n");
      close(new_sock);
      break;
    }
  }

  close(new_sock);

  //clients 后处理

}

#define  MAX_CLIENTS  3

struct client
{
  int sock;
  struct in_addr ip;
  u16_t port;
};

struct client clients[3];

//tcp 服务端函数
static void tcp_server_thread(){
    int  server_socket;
    struct  sockaddr_in  server_addr,  client_addr;
    socklen_t  client_len  =  sizeof(client_addr);
    int  i,  new_sock;

    for(i=0; i<MAX_CLIENTS; ++i){
      clients[i].sock = -1;
    }

    server_socket  =  socket(AF_INET,  SOCK_STREAM,  0);
    if  (server_socket  <  0)  {
        //  错误处理
        return;
    }

    server_addr.sin_family  =  AF_INET;
    server_addr.sin_addr.s_addr  =  INADDR_ANY;
    server_addr.sin_port  =  htons(12345);

    if  (bind(server_socket,  (struct  sockaddr  *)&server_addr,  sizeof(server_addr))  <  0)  {
        //  错误处理
        printf("bind failed!!\r\n");
        close(server_socket);
        return;
    }

    if  (listen(server_socket,  MAX_CLIENTS)  <  0)  {
        //  错误处理
        printf("listen error\r\n");
        closesocket(server_socket);
        return;
    }

    printf("2222222222\r\n");

    while  (1)  {
        new_sock  =  accept(server_socket,  (struct  sockaddr  *)&client_addr,  &client_len);
        if  (new_sock  <  0)  {
            printf("accept error!!!\r\n");
            continue;
        }

        //  添加新客户端到列表
        for  (i  =  0;  i  <  MAX_CLIENTS;  i++)  {
            if  (clients[i].sock  ==  -1)  {
                clients[i].sock  =  new_sock;
                clients[i].ip  =  client_addr.sin_addr;
                clients[i].port  =  ntohs(client_addr.sin_port);
                break;
            }
        }

        if  (i  ==  MAX_CLIENTS)  {
            //  客户端列表已满
            close(new_sock);
            continue;
        }

        //  处理客户端连接
        handle_client(new_sock);
    }

    close(server_socket);
}


/* USER CODE END Application */

