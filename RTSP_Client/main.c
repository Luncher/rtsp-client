#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fcntl.h>
#include <io.h>
#include "H264Parser.h"
 
 #define BUFFERSIZE 4096
 
 char *filename_buf,*ip_buf;
 
 int creat_udp(int i, int port);
 int recv_data_from_server(int sock_udp_fd1,int sock_udp_fd2);
 
 int main(/*int argc, char *argv[]*/)
 {
     int i = 0, j = 0, k = 0, m = 0;
     int sock_fd,inet_pton_fd,connect_fd,send_fd,recv_fd,sock_udp_fd1,sock_udp_fd2;
     char *p, *q, buf[BUFFERSIZE],rec_buf[BUFFERSIZE],session[BUFFERSIZE];
     struct sockaddr_in ser_addr;
 
	 char* argv[2] =  {NULL, "rtsp://192.168.1.161:554/av0_0"};

     p = argv[1];
     ip_buf = (char *)malloc(20);
     filename_buf = (char *)malloc(20);
     memset(ip_buf, '\0', 20);
     memset(filename_buf, '\0', 20);
     
     while (1)
     {
         if (p[i] <= '9' && p[i] >= '0')
         {
             ip_buf[k] = p[i];
             k++;
         }
         if (p[i] == '.')
         {
             ip_buf[k] = p[i];
             k++;
             j++;
         }
         i++;
         if (3 == j)
         {
             if ((p[i] < '0') || (p[i] > '9'))
                 break;
         }
 
         memcpy(filename_buf,argv[1] + strlen(ip_buf) + 9, strlen(argv[1] + strlen(ip_buf)));
     }
     
     printf("ip = %s\n",ip_buf);
     printf("filename = %s\n",filename_buf);
	 {
#ifdef WIN32
		WORD wVersionRequested;
		WSADATA  wsaData;
		int err;
		wVersionRequested =MAKEWORD( 1, 1 );
		err = WSAStartup( wVersionRequested, (LPWSADATA)&wsaData );
#endif
	 }
     sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     if (sock_fd < 0)
     {
         printf("TCP socket() fail.\n");
         exit(1);
     }
     printf("TCP socket() success.\n");
     printf("sock_fd = %d\n",sock_fd);
 
     memset(&ser_addr,0,sizeof(ser_addr));
     ser_addr.sin_family = AF_INET;
 
     inet_pton_fd = inet_pton(AF_INET, ip_buf, &ser_addr.sin_addr);
     if (inet_pton_fd < 0)
     {
         printf("TCP inet_pton() fail.\n");
         exit(1);
    }
    printf("TCP inet_pton() success.\n");
    printf("inet_pton_fd = %d\n",inet_pton_fd);

	//RTSP 协议的默认端口号为 554，
	//默认的承载协议为 TCP。 
    ser_addr.sin_port = htons(554);

    connect_fd = connect(sock_fd,(struct sockaddr *)&ser_addr,sizeof(ser_addr));
    if (connect_fd < 0)
    {
        printf("TCP connect() fail.\n");
        exit(1);
    }
    printf("TCP connect() success.\n");
    printf("connect_fd = %d\n",connect_fd);

    memset(buf, 0, BUFFERSIZE);

	//客户端向服务端发送 OPTIONS 报文请求，
	//目的是得到服务端提供的可用方法。
    printf("*********\n");
    sprintf(buf, "OPTIONS %s RTSP/1.0\r\n"
             "CSeq:1\r\n"
             "User-Agent:rtsp client(v1.0)"
             "\r\n"
             "\r\n",argv[1]);
    printf("send to server %s\n", buf);

    send_fd = send(sock_fd, buf, strlen(buf), 0);
    if (send_fd < 0)
    {
        printf("send() fail.\n");
        exit(1);
    }
    printf("send() success.\n");
    printf("send_fd = %d\n",send_fd);

    memset(rec_buf, 0, BUFFERSIZE);

    recv_fd = recv(sock_fd, rec_buf, sizeof(buf), 0);
    if (recv_fd < 0)
    {
        printf("recv() fail.\n");
        exit(1);
    }
    printf("recv() success.\n");
    printf("recv_fd = %d\n",recv_fd);
    printf("options recv from server is %s\n",rec_buf);

    memset(buf, 0, BUFFERSIZE);

    sprintf(buf, "DESCRIBE %s RTSP/1.0\r\n"
             "CSeq:2\r\n"
             "User-Agent:VLC media player(LIVE555 Streaming Media library version 2012.12.09)"
             "\r\n"
             "\r\n",argv[1]);
    printf("send to server %s\n", buf);

    send_fd = send(sock_fd, buf, strlen(buf), 0);
    if (send_fd < 0)
    {
        printf("send() fail.\n");
        exit(1);
    }
    printf("send() success.\n");
    printf("send_fd = %d\n",send_fd);

    memset(rec_buf, 0, BUFFERSIZE);

    recv_fd = recv(sock_fd, rec_buf, sizeof(buf), 0);
    if (recv_fd < 0)
    {
        printf("recv() fail.\n");
        exit(1);
    }
    printf("recv() success.\n");
    printf("recv_fd = %d\n",recv_fd);
    printf("describe recv from server is %s\n",rec_buf);
    
    sock_udp_fd1 = creat_udp(1, 5554);
    sock_udp_fd2 = creat_udp(2, 5555);

    memset(buf, 0, BUFFERSIZE);

    sprintf(buf, "SETUP %s RTSP/1.0\r\n"
             "CSeq:3\r\n"
             "Transport: RTP/AVP;unicast;client_port=5554-5555"
             "User-Agent:VLC media player(LIVE555 Streaming Media library version 2012.12.09)"
             "\r\n"
             "\r\n",argv[1]);
    printf("send to server %s\n", buf);

    send_fd = send(sock_fd, buf, strlen(buf), 0);
    if (send_fd < 0)
    {
        printf("send() fail.\n");
        exit(1);
    }
    printf("send() success.\n");
    printf("send_fd = %d\n",send_fd);

    memset(rec_buf, 0, BUFFERSIZE);

    recv_fd = recv(sock_fd, rec_buf, sizeof(buf), 0);
    if (recv_fd < 0)
    {
        printf("recv() fail.\n");
        exit(1);
    }
    printf("recv() success.\n");
    printf("recv_fd = %d\n",recv_fd);
    printf("setup recv from server is %s\n",rec_buf);

    memset(session, 0, BUFFERSIZE);
    if (NULL == rec_buf)
    {
        printf("session fail.\n");
        exit(1);
    }
    q = strstr(rec_buf, "Session:");
    printf("q = %s\n",q);
    while (*(q + 9 + m) != ('\0' && '\n' && '\r'))
    {
        session[m] = *(q +9 + m);
        m++;
    }
    session[m] = '\0';
    printf("Session success and it is %s\n",session);

    memset(buf, 0, BUFFERSIZE);
    sprintf(buf, "PLAY %s RTSP/1.0\r\n"
             "CSeq:4\r\n"
             "Session: %s\r\n"
             "Range:npt=0.000\r\n"
             "User-Agent:VLC media player(LIVE555 Streaming Media library version 2012.12.09)"
             "\r\n"
             "\r\n",argv[1],session);
    printf("send to server %s\n", buf);

    send_fd = send(sock_fd, buf, strlen(buf), 0);
    if (send_fd < 0)
    {
        printf("send() fail.\n");
        exit(1);
    }
    printf("send() success.\n");
    printf("send_fd = %d\n",send_fd);

    memset(rec_buf, 0, BUFFERSIZE);

    recv_fd = recv(sock_fd, rec_buf, sizeof(buf), 0);
    if (recv_fd < 0)
    {
        printf("recv() fail.\n");
        exit(1);
    }
    printf("recv() success.\n");
    printf("recv_fd = %d\n",recv_fd);
    printf("play recv from server is %s\n",rec_buf);

    recv_data_from_server(sock_udp_fd1,sock_udp_fd2);

    close(sock_fd);
    return 0;
}

int creat_udp(int i, int port)
{
    struct sockaddr_in udp_addr;
    int sock_udp_fd,bind_udp_fd;
    
    sock_udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_udp_fd < 0)
    {
        printf("UDP socket() fail.\n");
        exit(1);
    }
    printf("UDP socket() success.\n");
    printf("sock_udp_fd = %d\n",sock_udp_fd);
    
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    udp_addr.sin_port = htons(port);

    bind_udp_fd = bind(sock_udp_fd,(struct sockaddr *)&udp_addr,sizeof(udp_addr));
    if (bind_udp_fd < 0)
    {
        printf("UDP bind() fail.\n");
        exit(1);
    }
    printf("UDP bind() success.\n");
    printf("bind_udp_fd = %d\n",bind_udp_fd);
    
    printf("create UDP port%d success.\n",i);
    printf("sock_udp_fd%d = %d\n",i,sock_udp_fd);

    return sock_udp_fd;
}

int recv_data_from_server(int sock_udp_fd1, int sock_udp_fd2)
{
    char recv_buf[BUFFERSIZE];
    int maxfd,filelen,fd,select_ret;
    struct timeval tm;
    fd_set read_fd;
	char* pcFilePath = "./demo.dat";
	//FILE* fp

    if (sock_udp_fd1 >= sock_udp_fd2)
        maxfd = sock_udp_fd1;
    else
        maxfd = sock_udp_fd2;

    fd = open(pcFilePath, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
    {
        printf("open %s fail.\n",filename_buf);
        exit(1);
    }
    while (1)
    {
        tm.tv_sec = 5;
        tm.tv_usec = 0;
        
        FD_ZERO(&read_fd);
        FD_SET(sock_udp_fd1,&read_fd);
        FD_SET(sock_udp_fd2,&read_fd);

        select_ret = select(maxfd + 1, &read_fd, NULL, NULL, &tm);
        if (select_ret < 0)
        {
            perror("not readly file.\n");
            continue;
        }
        else if (0 == select_ret)
        {
            printf("time out\n");
            continue;
        }
        else
        {
            if (FD_ISSET(sock_udp_fd1,&read_fd))
            {
                memset(recv_buf, 0, BUFFERSIZE);
                filelen = recv(sock_udp_fd1,recv_buf,sizeof(recv_buf),0);
                printf("sizeof recv_buf is %d.\n",sizeof(recv_buf));
                printf("1recv data size from server:%d\n",filelen);
            
                if (filelen < 12)
                {
                    printf("1recv data error.\n");
                    continue;
                }
                //filelen = filelen - 12;
                //write(fd,(recv_buf + 12),filelen);
				
				/*BYTE *pBuf = NULL;
				int outSize = 0;
				pBuf = unpack.Parse_RTP_Packet((BYTE*)recv_buf, filelen, &outSize);
				if(pBuf != NULL && outSize > 0)
				{
					write(fd, pBuf, outSize);
				}*/
            }
            if (FD_ISSET(sock_udp_fd2,&read_fd))
            {
                memset(recv_buf, 0, BUFFERSIZE);
                filelen = recv(sock_udp_fd2,recv_buf,sizeof(recv_buf),0);
                printf("sizeof recv_buf is %d.\n",sizeof(recv_buf));
                printf("2recv data size from server:%d\n",filelen);
            
                if (filelen < 12)
                {
                    printf("2recv data error.\n");
                    continue;
                }
                filelen = filelen - 12;
                write(fd,(recv_buf + 12),filelen);
            }
        }
    }

    close(fd);
    printf("file recv complete!\n");
    return 0;
}