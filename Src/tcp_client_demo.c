#include "tcp_client_demo.h" 
#include "stdio.h"
#include "string.h" 
#include "lwip.h" 
#include "main.h" 

//TCP Client接收数据缓冲区
uint8_t tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	
//TCP服务器发送数据内容

//uint8_t tcp_client_sendbuf[10000 + 20]={0};

//TCP Client 测试全局状态标记变量
//bit7:0,没有数据要发送;1,有数据要发送
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有连接上服务器;1,连接上服务器了.
//bit4~0:保留
uint8_t tcp_client_flag;	 

//TCP Client 测试
void tcp_client_test(void)
{
 	//struct tcp_pcb *tcppcb;  	//定义一个TCP服务器控制块
	//struct ip_addr rmtipaddr;  	//远端ip地址
	
	uint8_t res=0;		
	uint8_t t=0; 
	
	//tcppcb=tcp_new();	//创建一个新的pcb
	//if(tcppcb)			//创建成功
	//{
	//	IP4_ADDR(&rmtipaddr,192, 168, 16, 100); 
	//	tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);  //连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
 	//}else res=1;
	//while(res==0)
	{
		if(tcp_client_flag&1<<6)//是否收到数据?
		{	

			tcp_client_flag&=~(1<<6);//标记数据已经被处理了.
		}
		if(tcp_client_flag&1<<5)//是否连接上?
		{

		}else if((tcp_client_flag&1<<5)==0)
		{
 			
		} 
		lwip_periodic_handle();
		HAL_Delay(2);
		t++;
		if(t==200)
		{
			if((tcp_client_flag&1<<5)==0)//未连接上,则尝试重连
			{ 
				tcp_client_connection_close(tcppcb,0);//关闭连接
				tcppcb=tcp_new();	//创建一个新的pcb
				if(tcppcb)			//创建成功
				{ 
					tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);//连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
				}
			}
			t=0;
		}		
	}
	//tcp_client_connection_close(tcppcb,0);//关闭TCP Client连接
} 

//lwIP TCP连接建立后调用回调函数
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	struct tcp_client_struct *es=NULL;  
	if(err==ERR_OK)   
	{
		es=(struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));  //申请内存
		if(es) //内存申请成功
		{
 			es->state=ES_TCPCLIENT_CONNECTED;//状态为连接成功
			es->pcb=tpcb;  
			es->p=NULL; 
			tcp_arg(tpcb,es);        			//使用es更新tpcb的callback_arg
			tcp_recv(tpcb,tcp_client_recv);  	//初始化LwIP的tcp_recv回调功能   
			tcp_err(tpcb,tcp_client_error); 	//初始化tcp_err()回调函数
			tcp_sent(tpcb,tcp_client_sent);		//初始化LwIP的tcp_sent回调功能
			tcp_poll(tpcb,tcp_client_poll,1); 	//初始化LwIP的tcp_poll回调功能 
 			tcp_client_flag|=1<<5; 				//标记连接到服务器了
			err=ERR_OK;
		}else
		{ 
			tcp_client_connection_close(tpcb,es);//关闭连接
			err=ERR_MEM;	//返回内存分配错误
		}
	}else
	{
		tcp_client_connection_close(tpcb,0);//关闭连接
	}
	return err;
}
    
//lwIP tcp_recv()函数的回调函数
err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err)
{ 
	uint32_t data_len = 0;
	struct pbuf *q;
	struct tcp_client_struct *es;
	err_t ret_err; 
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(struct tcp_client_struct *)arg; 
	if(p==NULL)//如果从服务器接收到空的数据帧就关闭连接
	{
		es->state=ES_TCPCLIENT_CLOSING;//需要关闭TCP 连接了 
 		es->p=p; 
		ret_err=ERR_OK;
	}else if(err!= ERR_OK)//当接收到一个非空的数据帧,但是err!=ERR_OK
	{ 
		if(p)pbuf_free(p);//释放接收pbuf
		ret_err=err;
	}else if(es->state==ES_TCPCLIENT_CONNECTED)	//当处于连接状态时
	{
		if(p!=NULL)//当处于连接状态并且接收到的数据不为空时
		{
			memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //数据接收缓冲区清零
			for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
			{
				//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
				//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
				if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//拷贝数据
				else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
				
				g_config_way = 1;
			}
			tcp_client_flag|=1<<6;		//标记接收到数据了
 			tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
			pbuf_free(p);  	//释放内存
			ret_err=ERR_OK;
		}
	}else  //接收到数据但是连接已经关闭,
	{ 
		tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
		es->p=NULL;
		pbuf_free(p); //释放内存
		ret_err=ERR_OK;
	}
	return ret_err;
}

//lwIP tcp_err函数的回调函数
void tcp_client_error(void *arg,err_t err)
{  
	//这里我们不做任何处理
} 
int testLen = 0;
//LWIP数据发送，用户应用程序调用此函数来发送数据
//tpcb:TCP控制块
//返回值:0，成功；其他，失败
err_t tcp_client_usersent(struct tcp_pcb *tpcb, uint8_t *buf, uint16_t len)
{
    err_t ret_err;
	int i = 0;
	struct tcp_client_struct *es; 
	es=tpcb->callback_arg;
	if(es!=NULL)  //连接处于空闲可以发送数据
	{
		testLen = len;
		es->p=pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL);	//申请内存 
        pbuf_take(es->p,(char*)buf,len);	//将tcp_client_sentbuf[]中的数据拷贝到es->p_tx中
        tcp_client_senddata(tpcb,es);//将tcp_client_sentbuf[]里面复制给pbuf的数据发送出去
        tcp_client_flag&=~(1<<7);	//清除数据发送标志
        if(es->p)pbuf_free(es->p);	//释放内存
		ret_err=ERR_OK;
#if 0
        es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_client_sendbuf),PBUF_POOL);	//申请内存 
        pbuf_take(es->p,(char*)tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf));	//将tcp_client_sentbuf[]中的数据拷贝到es->p_tx中
        tcp_client_senddata(tpcb,es);//将tcp_client_sentbuf[]里面复制给pbuf的数据发送出去
        tcp_client_flag&=~(1<<7);	//清除数据发送标志
        if(es->p)pbuf_free(es->p);	//释放内存
		ret_err=ERR_OK;
#endif		
	}else
	{ 
		tcp_abort(tpcb);//终止连接,删除pcb控制块
		ret_err=ERR_ABRT;
	}
	return ret_err;
}
//lwIP tcp_poll的回调函数
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_client_struct *es; 
	es=(struct tcp_client_struct*)arg;
    if(es->state==ES_TCPCLIENT_CLOSING)         //连接断开
	{ 
 		tcp_client_connection_close(tpcb,es);   //关闭TCP连接
		
		tcppcb=tcp_new();	//创建一个新的pcb
		if(tcppcb)			//创建成功
		{ 
			tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);//连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
		}
	} 
	ret_err=ERR_OK;
    return ret_err;
} 
//lwIP tcp_sent的回调函数(当从远端主机接收到ACK信号后发送数据)
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_client_struct *es;
	LWIP_UNUSED_ARG(len);
	es=(struct tcp_client_struct*)arg;
	if(es->p)tcp_client_senddata(tpcb,es);//发送数据
	return ERR_OK;
}
//此函数用来发送数据
void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
	struct pbuf *ptr; 
 	err_t wr_err=ERR_OK;
	while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
	{
		ptr=es->p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,2); //将要发送的数据加入到发送缓冲队列中
		if(wr_err==ERR_OK)
		{  
			es->p=ptr->next;			//指向下一个pbuf
			if(es->p)pbuf_ref(es->p);	//pbuf的ref加一
			pbuf_free(ptr);				//释放ptr 
		}else if(wr_err==ERR_MEM)es->p=ptr;
        tcp_output(tpcb);		        //将发送缓冲队列中的数据立即发送出去
	} 	
} 
//关闭与服务器的连接
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
	//移除回调
	tcp_abort(tpcb);//终止连接,删除pcb控制块
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);  
	if(es)mem_free(es); 
	tcp_client_flag&=~(1<<5);//标记连接断开了
}



