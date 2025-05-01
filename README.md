# Echo Client-Server

간단한 TCP 기반의 echo 서버 및 클라이언트 프로그램입니다.\
멀티 클라이언트를 지원하며, `-e`(echo), `-b`(broadcast) 옵션을 통해 다양한 통신 시나리오를 실험할 수 있습니다.

---

## 포함 파일

- `echo-server.cpp` : TCP echo 서버
- `echo-client.cpp` : TCP 클라이언트
- `Makefile` : 컴파일 스크립트

---

## 빌드 방법

```bash
make
```

실행 파일 생성:

- `echo-server`
- `echo-client`

---

## 실행 방법

### 서버

```bash
./echo-server <port> [-e] [-b]
```

- `-e` : 받은 메시지를 클라이언트에게 echo해주며 복사 전송
- `-b` : 모든 클라이언트에게 broadcast

**예시:**

```bash
./echo-server 1234 -e -b
```

### 클라이언트

```bash
./echo-client <server_ip> <port>
```

**예시:**

```bash
./echo-client 127.0.0.1 1234
```

입력한 메시지를 서버로 전송하며, 서버로부터 받은 메시지를 화면에 출력합니다.

---

## 테스트 방법

### 1. 서버 실행

```bash
./echo-server 1234 -e -b
```

### 2. 클라이언트 2개 실행 (2개 터미널)

```bash
./echo-client 127.0.0.1 1234
```

### 3. 하나의 클라이언트에서 메시지 입력

```
hello from A
```

### 4. 결과

- A: echo로 한 번 받음
- B: broadcast로 받음

---

## 실행파일 삭제

```bash
make clean
```

생성된 실행 파일을 삭제합니다.

---

## 보조 정보

- `-e` 없이 `-b`만 쓰면: 자신은 메시지를 받지 않음
- `-e -b` 같이 쓰면: 자신은 echo로, 다른 클라이언트들에게는 broadcast로 받음

