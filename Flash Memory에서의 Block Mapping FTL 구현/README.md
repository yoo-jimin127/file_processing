### 파일처리 과제 3: Flash Memory에서의 Block Mapping FTL 구현

#### 1. ftl_open() 구현
  - 논리 주소를 물리주소로 변환하는 작업
  - 이전에 사용했던 flash memory 파일의 존재 여부 체크
    - 기존 파일 존재하는 경우 각 블록의 첫번째 페이지를 읽어 address mappign table 복구
    - 그렇지 않은 경우 address mapping table의 각 lbn에 해당하는 pbn 값을 모두 -1으로 초기화
  - file system에서 ftl_write() 또는 ftl_read()를 최초로 호출하기 전 반드시 ftl_open() 호출
    
#### 2. ftl_write(int lsn, char *sectorbuf) 구현

#### 3. ftl_read(int lsn, char *sectorbuf) 구현

#### 4. ftl_print() 구현
  ```
  lbn pbn
  0 7
  1 -1
  2 1
  3 2
  4 3
  5 -1
  6 4
  free block’s pbn=0
```

#### 5. spare 영역에 메타데이터 쓰기
  - Flash memory 파일의 모든 페이지에서 16B의 spare 영역 중 첫 4B는 lbn을 저장하기 위한 용도로 사용
  - 이것은 ftl_open()에서 address mapping table 복구를 위해 활용
