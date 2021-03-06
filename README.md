# p2p_Chat-FileTransfer
server and client file on Linux

## 1. 구현 목표
>![1](https://user-images.githubusercontent.com/82483513/119311982-ba3eb280-bcac-11eb-9413-cc7c8d733099.jpg)  

## 2. 설계 조건
>1. User1과 User2는 서버에 접속할 때, 로그인을 성공하면 자신의 P2P IP주소/Port번호 정보를 서버로 전송해준다.  
>2. User1은 P2P 파일전송을 요청하는 특수한 메시지(“[FILE-사용자이름]”)를 키보드로부터입력받으면 서버로 전송하고 P2P 파일 송수신을 위한 P2P 파일전송 서버를 생성한다.(hint. “[FILE-사용자이름]” 을 송신하고 fork() 시스템콜을 이용하여 P2P 파일전송 서버를 위한 프로세스 생성)  
>3. 서버는 요청한 사용자이름에 해당하는 클라이언트의 P2P IP주소/Port번호 정보와 함께“[FILE]” 메시지를 해당 사용자에 전송한다.  
>4. “[FILE]” 를 수신한 User2는 P2P 파일전송 클라이언트가 되어 수신한 P2P IP주소/Port번호로 User1에 접속한다. (hint. “[FILE]” 메시지를 수신하면 User2s느 fork() 시스템 콜을이용하여 자식프로세스가 P2P 파일전송 클라이언트 역할을 담당하도록 함)  
>5. P2P 파일전송 서버-클라이언트 연결이 성공적으로 생성되면 클라이언트인 User2는 자신이 소유한 파일리스트를 User1에 서버를 거치지 않고 전송한다.  
>6. User1은 P2P 서버-클라이언트 통신으로 수신한 파일리스트를 화면에 출력해주고 전송받을 파일을 User1이 선택하여 User2에 요청한다.  
>7. User2는 파일요청을 수신하면 해당파일을 읽어 메시지로 만들어 User1에 전송해준다.  
>8. User1은 수신한 메시지로 다시 해당파일 이름으로 파일을 생성하여 저장함으로써 User2에 성공적으로 수신한 파일이 생성되도록 한다.  

## 3. 구현 도중 발생한 문제점
>1. 로그인 성공시 서버에서 접속한 아이피주소와 포트번호를 저장시키는데 문제가 계속 발생하였다. 아이피주소가 문자열의 형태로 오는것을 알고 배열과 포인터 변수를 사용하여 아이피주소를 저장 시키려고 시도했지만. 문자열을 변수에 “=”을 사용해서 저장하려고 하니 기존에 저장되어있던 아이피주소가 새로운 아이피주소로 계속 덮어씌어지는 현상이 발생하였다.  
>2. IP주소는 문자열이여서 전송받는데 큰 어려움이 없었다. 또 accept시 두번째 인자에 아이피주소가 오는것을 확인하고 이를 바로 저장시키면 되었기에 문제가 없었다. 하지만 포트번호는 클라이언트에서 #define 되어있는 값이었다. 이를 클라이언트에서 전송하여 서버에서 저장받고 값을 확인하면 계속 엉뚱한 값이 출력되었다.  
>3. 유저가 파일요청을 어떤유저에게 보내는지에 대한 구별을 하는 과정을 추가하지 않아 기존 Proj#2의 방식대로 메시지를 전송하면 에러가 뜨거나 엉뚱한 유저에게 파일요청을 하는 등의 문제가 발생하였다.  

## 4. 해결방안
>1. 이는 strcpy()를 사용하여 변수에 문자열을 저장시킴으로써 해결되었다. 이 문제를 해결하면서 각 유저의 정보를 담고있는 구조체를 만들어 더욱 직관적이고 코딩할때 편하게 하였다.  
>2. 이를 해결하기 위해 먼저 int형으로 포트번호를 선언하여 send하였다. 하지만 검색결과 intger형의 변수를 send할시 send(sockfd, (void *)변수, .....)와 같이 형을 바꾸어 전송하고 받을때도 이러한 과정을 거쳐야 하는것을 확인하고 시도해보았지만 완벽히 해결이 되지않았다.
따라서 포트번호는 #define을 사용하여 하나의 클라이언트 파일로 여러 아이피 주소에서 어떤 유저가 접속하여도 작동이 되도록 설계하였다.   
>3. 이를 위하여 서버에서 유저 구조체를 만들어 각각의 유저정보를 입력할때 소켓번호도 저장하였다. 이후 세명의 유저가 파일요청을 할수있는 모든 경우의 수에 대하여 조건문을 걸어서 각각 저장된 소켓번호를 사용하여 통신 할 수 있도록 하였다.  

## 5. Flow Chart
>![1](https://user-images.githubusercontent.com/82483513/119313038-0807ea80-bcae-11eb-9291-bae6fc3388ed.png)  
>파일요청 메세지를 입력한쪽이 서버가되고 수신받은쪽이 클라이언트가된다. 수신 받은쪽은 요청한 클라이언트의 아이피주소를 받아 서버가 된쪽에 connect()를 한다. 이후 파일전송 프로세스를 진행할 수 있다.  

## 6. server.c 코드 설명
>![1](https://user-images.githubusercontent.com/82483513/119313341-659c3700-bcae-11eb-9b4c-a2c0c4e9b119.png)  
>서버 코드에서 3명분의 아이디와 패스워드를 정의하였고 P2P파일전송시 사용할 매크로 변수를 선언하였다. 11~18라인의 구조체는 로그인시 user의 아이피 및 정보를 각각의 구조체에 입력시킬 수 있도록 선언하였다.
소켓번호가 저장될 client변수는 main함수 의외에서도 사용이 용이하게 전역변수로 선언해주었다.  
>SEND_IP()는 후술.  
>![1](https://user-images.githubusercontent.com/82483513/119313410-7a78ca80-bcae-11eb-881c-fb835f0c63c6.png)  
>뒤에서 select()함수를 사용하기 위해 구조체 fd_set을 선언하였다.  
>Project#2와 바뀐점으로는 63라인에 클라이언트의 아이피를 문자열의 형태로 임시로 저장할 포인터 변수를 선언하였고 70라인의 로그인의 실패여부를 알려주는 변수를 선언하였다.
여기서 login_fail_flag의 값이 1이되면 로그인에 실패하였다는 의미이다.  
>![1](https://user-images.githubusercontent.com/82483513/119313629-bca20c00-bcae-11eb-90a9-a83ea8319cc1.png)  
>![2](https://user-images.githubusercontent.com/82483513/119313650-c3308380-bcae-11eb-8f0e-baaa9be2ae77.png)  
>Listen() 단계가 끝나면 nfdps = sockfd +1 을 해준다. nfdps는 select()함수의 필요 인자 중 하나로 최대 소켓번호 +1을 의미한다.  
>While(1) 문에 들어간 뒤, nfdps의 값을 갱신해준다. 이를 갱신하지 않을 경우 오류가 발생한다. FD_SET을 이용하여 입출력 변화를 감지할 소켓을 지정한다.  
>Select()함수를 호출한다. 만약 값이 -1이면 에러이다. 이제 어떤 입력이 발생했는지 판단하기 위하여 FD_ISSET을 사용한다. 초기 소켓인 sockfd에서 입력이 발생하면 클라이언트 즉, 유저의 접속을 허용하고 new_fd를 만든다. 만일 new_fd 의 값이 -1 일 경우, accept 단계에서 에러가 발생했다.  
>![1](https://user-images.githubusercontent.com/82483513/119318475-53bd9280-bcb4-11eb-8494-167b2ee7ad74.png)  
>클라이언트의 소켓 번호를 위에서 생성한 client에 저장한다. 그 후, recv함수를 이용하여 변수 id와 pw에 각각 클라이언트 측에서 보낸 id와 pw를 저장한다. 이때 ‘\0’ (널 문자)를 사용하지 않을 경우 id와 pw 부분에 쓰레기 값들이 들어가 자동적으로 로그인 실패 처리가 된다.	
>이제, strcmp 함수를 이용하여 아이디와 비밀번호가 일치하는지 확인한다. 
>일치하면 로그인 성공 메시지를 클라이언트에게 전송하고 로그인 성공메세지를 채팅룸화면에 띄우고 해당 유저의 소켓정보와 ip정보를 각각 구조체에 저장시킨다. 또한 서버에서 어떤 IP주소에서 접속을 하였고 현재 접속중인 클라이언트를 확인할 수 있다.
>![1](https://user-images.githubusercontent.com/82483513/119318954-d9d9d900-bcb4-11eb-86b7-662c8f2fc7e6.png)  
>Project#2에서 클라이언트에게 메세지를 보내는 방식을 조금 수정하였다.
먼저 SEND_IP()함수가 1을 Return하면 P2P IP주소를 보내는 과정을 수행하고 0을 return하면 그대로 자신을 제외한 접속한 유저들에게 메세지를 보낸다.
>![1](https://user-images.githubusercontent.com/82483513/119319079-fa099800-bcb4-11eb-830c-12aebedc070f.png)  
>클라이언트에서 받은 메세지인 buf를 인자로 가져온다.
이후 이 인자를 strstr()을 이용하여 처음에 정의된 모든 매크로변수와 비교하여 어떤 유저가 어떤 유저에게 파일을 요청하는지 확인한다. 확인이 되면 파일을 요청받는 유저에게 “[FILE]”메세지와 파일을 요청하는 유저의 아이피주소를 send하고 1을 return한다.
>조건이 충족되지 못하면 0을 return한다.  
>예를들면 유저1이 [FILE-user3]을 입력한다면 서버의 SEND_IP()함수에서 1을 return받으면서 user3에게 [FILE-user3]대신 [FILE]메세지가 전송이되고 user1의 ip주소를 전송한다.  

## 7. client.c 코드 설명
>![1](https://user-images.githubusercontent.com/82483513/119443878-43b1bb80-bd65-11eb-9603-a87e4c4797fc.png)  
>ip주소를 정의하였고, 서버에서 [FILE]메세지를 받을때를 구별할 매크로변수 정의, P2P서버와 클라이언트를 생성할 함수를 선언하였다.  
>![2](https://user-images.githubusercontent.com/82483513/119443880-457b7f00-bd65-11eb-90e2-71d1e91fb8ae.png)  
>![3](https://user-images.githubusercontent.com/82483513/119443889-4a403300-bd65-11eb-8e02-fd7544c9ea61.png)  
>서버와 마찬가지로 fd_set 구조체와 select() 함수를 사용한다. 위의 부분에선 대부분 서버 쪽 코드와 동일하며 서버로 보내주기 위해 먼저 아이디와 패스워드를 키보드를 통해 입력 받은 후, 서버로 보내준다.  
>![4](https://user-images.githubusercontent.com/82483513/119443890-4b716000-bd65-11eb-867c-f006fd607f49.png)  
>81라인에서 select함수를 호출하고 메세지를 입력하였을때의 동작과정이다.  
박스쳐있는 부분이 Project#3에서 핵심이되는 p2p 파일전송을 요청하는 서버를 만드는 과정이다. 만약 입력한 메세지의 9번째 문자까지 “[FILE-user”와 같다면 MAKE_P2PSERVER()함수를 실행시킨다.  
입력한 메세지가 파일전송을 요청받는 유저의 번호에 따라 1,2,3이 바뀌기에 strncmp()함수를 사용해 그 이전의 문자열인 “[FILE-user”까지만 비교를 진행하였다.  
>![5](https://user-images.githubusercontent.com/82483513/119443891-4b716000-bd65-11eb-813b-ff92b4f96f1f.png)  
>![6](https://user-images.githubusercontent.com/82483513/119443892-4c09f680-bd65-11eb-8576-853e273bd883.png)  
>이부분은 서버로부터 메세지를 받았을때 동작되는 과정이다.  
>박스쳐진 부분이 입력받은 메세지가 “[FILE]”이라면 클라이언트를 생성하는 과정이다.  
>일단 [FILE]메세지를 보낸 유저의 IP주소를 원할히 받기위해 1초간 쉬는 sleep()를 사용하고
IP주소를 받으면 이를 recv_ip변수에 저장한다. 그리고 MAKE_P2PCLIENT()함수를 실행하여
클라이언트를 생성한다.  
>![7](https://user-images.githubusercontent.com/82483513/119444362-1285bb00-bd66-11eb-8d32-1f57115c0130.png)  
>MAKE_P2PSERVER()함수의 서버를 생성하는 과정이다.
server.c의 서버 생성과정과 마찬가지로 소켓설정을 해주고 bind, listen, accept 과정을 실행시킨다.  
P2P서버에서는 파일을 전송받을것이기 때문에 file포인터를 선언해주고 파일리스트에서 전송받은 파일을 선택하기위한 변수도 선언해준다.  
>![8](https://user-images.githubusercontent.com/82483513/119444366-13b6e800-bd66-11eb-824e-c89964b437c4.png)  
>서버가 된 클라이언트는 accept에 성공하면 file list를 출력한다. 이후 파일번호를 (1/2/3)중 하나를 선택한다.  
라인 196~198에서 선택된 번호는 접속한 클라이언트에 전송이 되고 접속한 클라이언트에서 해당 파일의 내용을 서버에서 전송받는다.  
이후 선택된 번호에따라 fopen()으로 file을 생성하고 파일을 생성 후 작성할것이기 때문에 “w”인자를 사용한다. 접속한 클라이언트가 보낸 파일의 내용을 서버에서 생성된 FILE에 입력후, 저장된 내용을 확인하할수 있도록 화면에 출력한다.  
>![9](https://user-images.githubusercontent.com/82483513/119444368-13b6e800-bd66-11eb-905e-785b48e6748d.png)  
>MAKE_P2PCLIENT함수에서는 서버로부터 전송받은 ip주소를 인자로 받는다.  
새로운 소켓을 서버가 될 클라이언트의 ip주소로 초기화한다. 이후 connect()를 요청한다.  
이 함수 역시 파일을 전송하기 위한 과정이 포함되어있기에 파일포인터 변수를 선언해준다.  
>![10](https://user-images.githubusercontent.com/82483513/119444370-144f7e80-bd66-11eb-8adb-4faa0089becb.png)  
>서버측 클라이언트에서 select한 값을 넘겨받아 이를 조건문을 통해 파일을 오픈한다.  
이때 파일을 읽기형식(”r”)으로 열기때문에 파일이 존재하지 않으면 에러가 발생할 수 있다.  
클라이언트측은 서버측에게 파일의 내용을 전송해야하기때문에 fgets()로 파일의 내용을 읽고 이를 저장하여 서버에 전송한다.  
 
## 8. 실행 결과
>![1](https://user-images.githubusercontent.com/82483513/119444846-d868e900-bd66-11eb-8e05-3ce4dc374817.png)  
>IP주소 103로 접속한 서버측 모습이다. 로그인시 IP주소와 접속중인 클라이언트의 수를 나타내게 하였다.  
>![2](https://user-images.githubusercontent.com/82483513/119444866-df8ff700-bd66-11eb-8bc6-3167e3ce6091.PNG)  
>위는 IP 101로 접속한 클라이언트. user3으로 로그인 한 모습이다. 클라이언트를 실행하기전
ls명령어를 통해 폴더내에 아무런 파일이 없는것을 확인할 수 있다.  
채팅방에 접속한후 채팅을 하다가 “[FILE-userX]” (X는 전송받을 상대의 user number) 을 입력하게되면 2초후 자신이 서버가되어 선택한유저와 통신을 하게된다.
위는[FILE-user1]을 입력하여 user1에게 파일을 요청하는 모습이다. 클라이언트와 연결이 완료되면 파일리스트를 출력하고 전송받은 파일을 선택한다.
마지막으로 전송받은 파일의 내용을 화면에 출력한다.  
>![3](https://user-images.githubusercontent.com/82483513/119444869-e0288d80-bd66-11eb-9244-aa5aa669c033.PNG)  
>IP주소 102로 접속한 user1의 모습이다. 전송할 파일이 세개가 존재한다. (*)만약 여기서 파일이존재하지 않는데 상대 클라이언트가 존재하지 않는 파일을 요청한다면 fopen()시 '읽기' 인자를 주었기에 에러가 생길 수 있다.   
user1으로 로그인 후 채팅방에서 채팅 도중 user3가 파일을 요청하였다. 이때 user3은 “[FILE-user3]”을 입력함으로써 파일을 요청하였지만 user1에게는 “[FILE]”이라고 메세지가 오는것을 확인 할 수 있다.  
>![4](https://user-images.githubusercontent.com/82483513/119444873-e0c12400-bd66-11eb-987a-26c14e5e93b1.PNG)  
>파일 전송이 완료된 후에도 정상적으로 다시 채팅이 이용가능하다. 다시 채팅을 하다가 exit!를 입력하여 정상적으로 접속을 종료하는 모습이다. 101측 클라이언트에서 다시 ls 명령어를 통해 FILE3.txt를 전송받은것을 확인 할 수 있다.  
>![5](https://user-images.githubusercontent.com/82483513/119444850-d9017f80-bd66-11eb-99b9-bb238592359c.png)  
>101측 클라이언트로 전송받은 FILE3을 열어 FILE3의 내용까지 완벽히 전송받았는지 확인하였다.  

## 9. 추후 개선사항
>1. 코드를 짜기전 파일리스트를 출력하는 부분에 대하여 생각할때 폴더 내 파일을 전부 읽어서 전송하는 방식이었다. 하지만 정보부족으로 구현이 쉽지 않았고 추후에 io.h소스를 사용하여 파일을 탐색할수 있는 방법을 알게 되었다. 이 부분에 대해 더욱 이해하여 해당 소스를 잘 이해하게 된다면 폴더 내 파일을 전부 읽고 이를 데이터화시켜 send/recv하여 다룰 수 있을 것이다. 현재는 printf()를 사용하여 파일리스트를 일방적으로 출력하였지만. 해결된다면 상대방의 파일을 전부 확인 할 수 있을 것이다.  
>2. 파일전송이 완료된 후 >>가 바로 출력되지않는 경우가 발생한다.  

