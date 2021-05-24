#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//P2P통신할시에 정보들을 입력받을 구조체
typedef struct UserInfo
{
	char ip[30];
	int client_sock;//각각의 user의 접속 소켓까지 저장한다.
	//P2P포트번호는 어짜피 어떤 클라이언트에서 받든 동일하기때문에
	//main()에서 하나의 변수로 입력 받을 수 있다.
} UI;
UI user1, user2, user3;//IP를 저장시킬 세가지 구조체변수를 선언한다.
int client[30]; // chat client's socket number list , max = 30;
#define BACKLOG 10
#define USER1_ID "user1"
#define USER1_PW "password1"

#define USER2_ID "user2"
#define USER2_PW "password2"

#define USER3_ID "user3"
#define USER3_PW "password3"

#define TRUE 1
#define FALSE 0

#define success1 "\nLog-in success!! [user1] *^^*\n===========chat room==========\nEnter [FILE-userX] to get files\n==========(X : 1,2,3)=========\n"
#define success2 "\nLog-in success!! [user2] *^^*\n===========chat room==========\nEnter [FILE-userX] to get files\n==========(X : 1,2,3)=========\n"
#define success3 "\nLog-in success!! [user3] *^^*\n===========chat room==========\nEnter [FILE-userX] to get files\n==========(X : 1,2,3)=========\n"
#define fail "\n==fail==\n"
#define EXIT  "exit!"

#define P2P_USER1 "[user1] : [FILE]\n"
#define P2P_USER2 "[user2] : [FILE]\n"
#define P2P_USER3 "[user3] : [FILE]\n"
#define P2P_USER1toUSER2 "[user1] : [FILE-user2]\n"
#define P2P_USER1toUSER3 "[user1] : [FILE-user3]\n"
#define P2P_USER2toUSER1 "[user2] : [FILE-user1]\n"
#define P2P_USER2toUSER3 "[user2] : [FILE-user3]\n"
#define P2P_USER3toUSER1 "[user3] : [FILE-user1]\n"
#define P2P_USER3toUSER2 "[user3] : [FILE-user2]\n"

#define P2P_PORT 4501
int SEND_IP(char *msg);
int main(int argc, char *argv[])
{
    int num_chat = 0; //chat client number , i use 3 client 
    int sockfd, new_fd; //listen on sock_fd , new connection on new_fd  

    struct sockaddr_in my_addr; //my address information 
    struct sockaddr_in their_addr; //remote address informaiton
    
    unsigned sin_size; 

	//strcpy를 원활히 사용할 수 있도록 ip주소를 
	//포인터 변수에 저장하고 이를 구조체 배열변수에 저장한다.
	char *ip;
    char id[20]; 
    char pw[20];

    int rcv_byte; 
    char buf[512];
    char buf1[20];
	int login_fail_flag = 0;

    fd_set  read_fds; // I/O check struct , not use fork 
    FD_ZERO(&read_fds); // init 0 
    int nfdps; //max socket number , select() need maxfdp

    //first socket create // 
    sockfd = socket(PF_INET, SOCK_STREAM, 0); 

    if(sockfd == -1){
        perror("Sever-socket() error lol!");
        exit(1);
    }

    else printf("Server-socket() sockfd is Ok...\n");

    // socket is PF_INET because block "core dumped" 

    
    bzero((char *)&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(4508);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  

    // bind // 
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Server-bind() error lol!");
        exit(1);
    }

    else printf("Server-bind() is Ok...\n");

    //listen //

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen error lol!");
        exit(1);
    
    }
    else printf("listen() is Ok...\n\n");

    // end listen >> nfdps  = sockfd +1 , max socketnumber +1 
    // because not accept , present is listen step

    nfdps = sockfd + 1;
   
    while(1)
    {

        if((num_chat - 1) >=0)
        {
            nfdps = client[num_chat - 1] +1; 
        }

        // because nfdps is maxsocketnumber +1 , this step is re new 
        FD_SET(sockfd, &read_fds); // FD_SET pick  I/O check socketnumber  >> sockfd 

        for (int i=0; i<num_chat; i++) // all pick client enter socket 
        {
            FD_SET(client[i], &read_fds);
        }
      
        if (select(nfdps, &read_fds, (fd_set *)0, (fd_set *)0, NULL) ==-1)
        {
            perror("Select() is error lol!");
            exit(1);
        }
        // else nono beacause sever chat room so durty 

        if (FD_ISSET(sockfd, &read_fds))
        {
            sin_size = sizeof(their_addr);
            new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
            if (new_fd == -1)
            {
                printf("accept error");
                exit(1);
            }             
            else
            {
                printf("accept() is Ok...\n\n");
            }
            client[num_chat] = new_fd;

            rcv_byte = recv(new_fd, id, 20, 0);
            id[rcv_byte] = '\0'; //must do , prevent error
            
            rcv_byte = recv(new_fd, pw, 20, 0);
            pw[rcv_byte] = '\0'; //must do , prevent error 
           
            // using strcmp id , pw check part //
            if ((strcmp(id, USER1_ID) == 0) && (strcmp(pw, USER1_PW) == 0 ))
            {
				ip = inet_ntoa(their_addr.sin_addr);
				strcpy(user1.ip, ip);
				printf("IP : %s ----\n", user1.ip);
				printf("number of clients : %d \n", num_chat + 1);
				printf("%s", success1);
				send(new_fd, success1, strlen(success1) + 1, 0);
				login_fail_flag = 0;
				user1.client_sock = new_fd;
                num_chat++;
            }
            else if ((strcmp(id, USER2_ID) == 0) && (strcmp(pw, USER2_PW) ==0))
            {
				ip = inet_ntoa(their_addr.sin_addr);
				strcpy(user2.ip, ip);
				printf("IP : %s ----\n", user2.ip);
				printf("number of clients : %d \n", num_chat + 1);
				printf("%s", success2);
				send(new_fd, success2, strlen(success2) + 1, 0);
				login_fail_flag = 0;
				user2.client_sock = new_fd;
                num_chat++;
            }
            else if ((strcmp(id, USER3_ID)==0) && (strcmp(pw, USER3_PW) ==0))
            {
				ip = inet_ntoa(their_addr.sin_addr);
				strcpy(user3.ip, ip);
				printf("IP : %s ----\n", user3.ip);
				printf("number of clients : %d \n", num_chat + 1);
				printf("%s", success3);
				send(new_fd, success3, strlen(success3) + 1, 0);
				login_fail_flag = 0;
				user3.client_sock = new_fd;
                num_chat++;
            }
            else
            {
                printf("%s \n",fail);
                send(new_fd, fail, strlen(fail) +1, 0);
				login_fail_flag = 1;
            }
        }
		for (int i = 0; i < num_chat; i++)
		{
			if (FD_ISSET(client[i], &read_fds))
			{
				char text[512];
				rcv_byte = recv(client[i], buf, 512, 0);

				if (rcv_byte == -1)
				{
					if (i != num_chat - 1)
					{
						client[i] = client[num_chat - 1];
					}
					num_chat--;

					if (num_chat == 0)
					{
						close(client[i]);
					}
				}
				buf[rcv_byte] = '\0';
				for (int j = 0; j < num_chat; j++)
				{
					
					if (i != j) {
						if (SEND_IP(buf) == 1)
							continue;
						else
						send(client[j], buf, strlen(buf) + 1, 0);
								
					}
				}

			    sprintf(text, ">>%s", buf);
                printf("%s", text);
				
				if (strstr(buf, EXIT) != NULL)
				{
					if (i != num_chat - 1)
					{
						client[i] = client[num_chat - 1];
					}
					num_chat--;
					if (num_chat == 0)
					{
						close(client[i]);
					}
				}
			}
		}	
    }
}
//msg를 읽어들여 
int SEND_IP(char msg[500])
{
	if (strstr(msg, P2P_USER1toUSER2) != NULL)
	{
		send(user2.client_sock, P2P_USER1, strlen(P2P_USER1) + 1, 0);
		send(user2.client_sock, user1.ip, strlen(user1.ip) + 1, 0);
		return 1;
	}
	else if (strstr(msg, P2P_USER2toUSER1) != NULL)
	{
		send(user1.client_sock, P2P_USER2, strlen(P2P_USER2) + 1, 0);
		send(user1.client_sock, user2.ip, strlen(user2.ip) + 1, 0);
		return 1;
	}
	else if (strstr(msg, P2P_USER1toUSER3) != NULL)
	{
		send(user3.client_sock, P2P_USER1, strlen(P2P_USER1) + 1, 0);
		send(user3.client_sock, user1.ip, strlen(user1.ip) + 1, 0);
		return 1;
	}
	else if (strstr(msg, P2P_USER2toUSER3) != NULL)
	{
		send(user3.client_sock, P2P_USER2, strlen(P2P_USER2) + 1, 0);
		send(user3.client_sock, user2.ip, strlen(user2.ip) + 1, 0);
		return 1;
	}
	else if (strstr(msg, P2P_USER3toUSER1) != NULL)
	{
		send(user1.client_sock, P2P_USER3, strlen(P2P_USER3) + 1, 0);
		send(user1.client_sock, user3.ip, strlen(user3.ip) + 1, 0);
		return 1;
	}
	else if (strstr(msg, P2P_USER3toUSER2) != NULL)
	{
		send(user2.client_sock, P2P_USER3, strlen(P2P_USER3) + 1, 0);
		send(user2.client_sock, user3.ip, strlen(user3.ip) + 1, 0);
		return 1;
	}
	else
		return 0;
}
