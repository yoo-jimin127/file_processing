### 파일처리 과제 4: 레코드 저장 및 삭제

레코드 삭제 관리 및 재사용에 관한 프로그램 작성

#### 1. person 레코드 파일 구조
  - 레코드 파일
  - 헤더 레코드
  - 데이터 페이지
  - 레코드

![image](https://user-images.githubusercontent.com/66112716/147569723-086561f6-e80c-456e-8496-65b268cb9e5a.png)
![image](https://user-images.githubusercontent.com/66112716/147569743-f852ee4e-27d1-446a-9db0-26a47625d777.png)
  

#### 2. 레코드 삽입
``` a.out a <record file name><field values list> ``` <br>
ex) ``` a.out a person.dat “8811032129018” “GD Hong” “23” “Seoul” “02-820-0924” “gdhong@ssu.ac.kr” ``` <br>

#### 3. 레코드 삭제
``` a.out d <record file name> <field value> ``` <br>
ex) ```a.out d person.dat “8811032129018” ``` <br>
