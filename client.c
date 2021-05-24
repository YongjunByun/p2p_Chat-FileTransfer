#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define P2P_USER1 "[user1] : [FILE]\n"
#define P2P_USER2 "[user2] : [FILE]\n"
#define P2P_USER3 "[user3] : [FILE]\n"
#define SERVER_IP "220.149.128.103"

#define fail "\n==fail==\n"
#define EXIT "exit!"

#define P2P_PORT 4501
void ClearBuffer(void);
void MAKE_P2PSERVER();
void MAKE_P2PCLIENT(char ip[30]);
int main(int argc, char *argv[])
{

	int sockfd;
	struct sockaddr_in dest_addr;

	char id[20];
	char pw[20];

	char text[512];
	char msg[512];
	char recv_ip[30];

	int rcv_byte;
	int nfdps ;
	int msg_flag = 0;

	fd_set read_fds;
	FD_ZERO(&read_fds);

	printf("ID : ");
	scanf("%s" , id);

    printf("PW : ");
	scanf("%s" , pw);
	ClearBuffer();
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Client-socket() error lol");
		exit(1);
	}

	else printf("Client_socket() sockfd is Ok...\n");

	bzero((char *)&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	dest_addr.sin_port = htons(4508);
	
	if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) ==-1)
	{
		perror("Client-connect error! lol!");
		exit(1);
	}
	else
	{
		printf("Client-connect() is Ok...\n\n");
		send(sockfd, id, strlen(id)+1, 0 );
		send(sockfd, pw, strlen(pw)+1, 0 );
		
	}

	nfdps = sockfd +1 ;
	
	while(1)
	{
		FD_SET(0, &read_fds);
		FD_SET(sockfd, &read_fds);

		if (select(nfdps, &read_fds, (fd_set *)0, (fd_set *)0, NULL) == -1)
		{
			printf("Select is  error lol!");
			exit(1);
		}
			if (FD_ISSET(0, &read_fds))
			{
				msg_flag = 0; //플래그 0으로 초기화
				fgets(msg, 512, stdin);
				sprintf(text, "[%s] : %s", id, msg);
				send(sockfd, text, strlen(text), 0);
				if (strstr(msg, EXIT) != NULL)
				{
					close(sockfd);
					exit(1);
				}
				if (strncmp(msg, "[FILE-user", 9) == 0)
				{
					MAKE_P2PSERVER();
				}
				
			}
			if (FD_ISSET(sockfd, &read_fds))
			{
				msg_flag = 0; //플래그 0으로 초기화
				rcv_byte = recv(sockfd, msg, 512, 0 );
				msg[rcv_byte] = '\0';

				if(rcv_byte>0)
					printf("%s", msg);
				if (strstr(msg, fail) != NULL)
				{
					close(sockfd);
					exit(1);
				}
			}
			//초기에 0으로 선언되어있다 따라서 로그인 직후는 입력이 없으므로 >>를 출력한다.
			//출력후 1로 값이 바뀌면서 입력을 받기전까지는 다시 출력하지않는다.
			//입력을 받으면 flag가 다시 0으로 바뀌며 if문 조건을 만족하여 >>를 출력한다.
			if (msg_flag == 0)
			{
				//fflush를 사용하지않으면 \n을 입력받기전까지 >>가 정상적으로 출력되지않는다.
				fflush(stdout);
				printf(">>");
				fflush(stdout);
				msg_flag == 1;
			}
			if ((strstr(msg, P2P_USER1) != NULL) || (strstr(msg, P2P_USER2) != NULL) ||
				(strstr(msg, P2P_USER3) != NULL))
			{
				sleep(1);
				msg_flag == 0;
				rcv_byte = recv(sockfd, msg, 512, 0);
				msg[rcv_byte] = '\0';
				strcpy(recv_ip, msg);
				MAKE_P2PCLIENT(recv_ip);
			}
 	}
}

void ClearBuffer(void) {
	while (getchar() != '\n');
}
void MAKE_P2PSERVER() {
	
	int S_sockfd;
	int S_newfd;
	int S_size;
	char s_buffer[300];

	struct sockaddr_in sock_addr;
	struct sockaddr_in new_addr;

	FILE *s_fp;
	
	
	char s_select[20];

	S_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (S_sockfd == -1)
	{
		printf("P2P socket error!");
		exit(1);
	}

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_addr.sin_port = htons(P2P_PORT);

	if (bind(S_sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1)
	{
		printf("P2P bind error!");
		exit(1);
	}
	if (listen(S_sockfd, 10) == -1)
	{
		printf("P2P listen error!");
		exit(1);
	}

	memset(s_buffer, 0, 300);
	S_size = sizeof(new_addr);
	S_newfd = accept(S_sockfd, (struct sockaddr*)&new_addr, &S_size);
	if (S_newfd == -1)
	{
		printf("P2P accept errer!");
	}
	
	int s_buf_byte;
	printf("[file list]\n");
	printf("1. FILE1\n");
	printf("2. FILE2\n");
	printf("3. FILE3\n");
	printf("please select file number : ");
	scanf("%s", s_select);
	send(S_newfd, s_select, strlen(s_select) + 1, 0);

	s_buf_byte = recv(S_newfd, s_buffer, 512, 0);
	s_buffer[s_buf_byte] = '\0';
	
	if (strcmp(s_select, "1") == 0) {
		s_fp = fopen("FILE1.txt", "w");//파일 생성 후 작성 "w"
	}
	else if (strcmp(s_select, "2") == 0) {
		s_fp = fopen("FILE2.txt", "w");
	}
	else if (strcmp(s_select, "3") == 0) {
		s_fp = fopen("FILE3.txt", "w");
	}
	else {
		printf("wrong input.");
		exit(1);
	}

	fputs(s_buffer, s_fp);

	fgets(s_buffer, sizeof(s_buffer), s_fp);
	printf("downloaded File text : \n");
	printf("%s", s_buffer);
	printf("------download complete.------\n");

	fclose(s_fp);

	close(S_newfd);
	close(S_sockfd);

}
///////////////////////////////////////////////////////////////////////////////
void MAKE_P2PCLIENT(char ip[30]) { //
	sleep(1);
	
	int C_newfd;
	char c_text[300];
	FILE *c_fp;
	struct sockaddr_in sock_addr;

	char c_buffer[1024];
	char c_select[20];

	C_newfd = socket(PF_INET, SOCK_STREAM, 0);
	if (C_newfd == -1)
	{
		printf("P2P socket errer!");
		exit(1);
	}

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(ip);
	sock_addr.sin_port = htons(P2P_PORT);

	if (connect(C_newfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1)
	{
		printf("P2P connect error!");
		exit(1);
	}


	
	int buf2_byte;
	buf2_byte = recv(C_newfd, c_select, strlen(c_select) + 1, 0);
	c_select[buf2_byte] = '\0';

	printf("selected number : %s\n", c_select);
	memset(c_buffer, 0, 300);
//파일을 읽기형식으로 엽니다. 만약 파일이 없으면 에러가 뜰 수 있습니다.
	if (strcmp(c_select, "1") == 0) {
		c_fp = fopen("./FILE1.txt", "r");
	}
	else if (strcmp(c_select, "2") == 0) {
		c_fp = fopen("./FILE2.txt", "r");
	}
	else if (strcmp(c_select, "3") == 0) {
		c_fp = fopen("./FILE3.txt", "r");
	}
	else {
		printf("wrong input. (1/2/3)");
		exit(1);
	}

	if (c_fp == NULL)//개방 여부 확인
	{
		printf("file open error\n");
		exit(1);
	}

	fgets(c_text, sizeof(c_text), c_fp);
	puts("file upload complete");
	strcpy(c_buffer, c_text);

	send(C_newfd, c_buffer, strlen(c_buffer) + 1, 0);
	fclose(c_fp);
	close(C_newfd);
}
