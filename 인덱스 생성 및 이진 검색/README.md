### 파일처리 과제 5: 인덱스 생성 및 이진 검색

#### 1. person 레코드 파일의 구조
![image](https://user-images.githubusercontent.com/66112716/147569723-086561f6-e80c-456e-8496-65b268cb9e5a.png)
![image](https://user-images.githubusercontent.com/66112716/147569743-f852ee4e-27d1-446a-9db0-26a47625d777.png)

#### 2. 인덱스 파일 생성 – person.c에서 createIndex() 구현
  ``` a.out i <record file name> <indexfile name> ``` <br>
  ex) ``` $ a.out i records.dat records.idx ``` <br>
  
#### 3. 이진 검색 (binary search) - person.c에서 binarysearch() 구현
  ```a.out b <record file name> <index file name> <key value> ``` <br>
  ex)
  ```
  $ a.out b records.dat records.idx “010223291234“
  #reads:18
  id=010223291234
  name=Gildong Hong
  age=20
  addr=Seoul, Korea
  phone=010-123-4567
  email=gdhong@ssu.ac.kr
  ```


  주어진 킷값을 만족하는 레코드가 존재하지 않는 경우 아래와 같이 출력
  ```
  $ a.out b records.dat records.idx “010223291234“
  #reads:18
  no persons
  ```
