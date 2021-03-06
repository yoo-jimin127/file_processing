// 주의사항
// 1. blockmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blockmap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(blockmap.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include "blockmap.h"
// 필요한 경우 헤더 파일을 추가하시오.

typedef struct {
	int lbn; //logical block number
	int pbn; //physical block number
}MappingTableEntry;

typedef struct {
	MappingTableEntry entry[BLOCKS_PER_DEVICE];
}AddrMappingTable;

AddrMappingTable addrMappingTable; //address mapping table 구조체 변수
int freeBlockIdx = 0; //free block의 pbn 값의 초기화를 위해 필요한 인덱스 변수
int pbnIdx = 0; //ftl_write()에서 pbn 값을 임의로 지정하여줄 때 필요한 변수

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//
void ftl_open() {
	//
	// address mapping table 초기화 또는 복구
	// free block's pbn 초기화
	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
	char * checkbuf = (char *)malloc(sizeof(char) * PAGE_SIZE); //flash memory 파일의 존재여부 판단을 위해 dd_read()에 pagebuf 인자를 넘길 메모리 동적 할당
	char *cmpbuf = (char *)malloc(sizeof(char) * PAGE_SIZE);
	int empty_check = 0;

	memset(checkbuf, 0, PAGE_SIZE); //메모리 초기화
	memset(cmpbuf, 0xff, PAGE_SIZE);
	empty_check = dd_read(0, checkbuf); //checkbuf에 ppn 0번째 값을 읽어옴

	//if (strcmp(checkbuf,cmpbuf) != 0) { 
	//	printf("다름\n");
	//}
	//printf("%s\n", checkbuf);
	//printf("%s\n", cmpbuf);

	//	if (checkbuf) { //기존 파일이 존재하는 경우
	//각 블록의 첫번째 페이지를 읽어서 address mapping table 복구 과정
	//	}

	for (int i = 0; i < DATABLKS_PER_DEVICE; i++) {
		addrMappingTable.entry[i].lbn = i; //address mapping table의 lbn값을 초기화
		addrMappingTable.entry[i].pbn = -1; //address mapping table의 entry의 pbn을 초기화
	}

	freeBlockIdx = BLOCKS_PER_DEVICE -1; // free block의 pbn 값을 맨 마지막 블록의 인덱스로 초기화
	addrMappingTable.entry[DATABLKS_PER_DEVICE].pbn = freeBlockIdx;
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf) {
	char pagebuf[PAGE_SIZE]; //페이지 버퍼(페이지 단위로 데이터를 읽어오기 위해 PAGE_SIZE만큼)
	char sparebuf[SPARE_SIZE];
	int lbn, offset; //데이터를 읽을 lbn과 offset
	int pbn; //physical block number
	int ppn; //physical page number(인자로 주어진 lsn에 대응되는 ppn)

	memset(pagebuf, 0, PAGE_SIZE); //페이지버퍼 메모리 초기화
	memset(sparebuf, 0, SPARE_SIZE); //스페어 버퍼 메모리 초기화
	
	sprintf(sparebuf, "%d", lsn); //스페어버퍼에 lsn의 값을 문자열로 저장

	lbn = lsn / PAGES_PER_BLOCK; //데이터를 읽을 블록의 번호를 얻기 위해 lsn/블럭 개수
	offset = lsn % PAGES_PER_BLOCK; //lbn번째 블록의 몇번째 페이지에 데이터를 읽을 것인지에 대한 offset

	pbn = addrMappingTable.entry[lbn].pbn; //인자로 주어진 lsn으로부터 대응되는 pbn을 구함
	ppn = offset + (pbn * PAGES_PER_BLOCK);

	if (pbn < 0) { //pbn이 -1인 경우(free page인 경우)
		//printf("---There is No Data---\n");
		return;
	}

	dd_read(ppn, pagebuf); //해당 ppn에서 페이지버퍼 읽어오고
	strncpy(sectorbuf, pagebuf, SECTOR_SIZE); //섹터 사이즈만큼 섹터버퍼에 페이지버퍼 내용 복사

	for (int i = PAGES_PER_BLOCK + (PAGES_PER_BLOCK * pbn); i < PAGES_PER_BLOCK + (PAGES_PER_BLOCK * pbn); i++) {
		dd_read(i, pagebuf); //i번째 인덱스의 값을 pagebuf에 읽어옴

		if (strncmp(pagebuf + SECTOR_SIZE, sparebuf, SPARE_SIZE) != 0) { // 스페어버퍼의 값과 lsn이 같을 경우 0을 리턴, 다를 경우 0이 아닌 다른 값 리턴
			strncpy(sectorbuf, pagebuf, SECTOR_SIZE); //업데이트된 내용을 SECTOR_SIZE만큼 다시 복사
		}
	}
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char *sectorbuf) {
	char spare_data[SPARE_SIZE]; //spare영역의 메타데이터 작성을 위한 배열
	char cmp_spare_data[SPARE_SIZE]; //임시 스페어 버퍼
	char tmpbuf[PAGE_SIZE]; //임시 버퍼
	char pagebuf[PAGE_SIZE]; //페이지 버퍼
	int lbn, offset; //데이터를 쓸 lbn과 offset
	int pbn;//physical block number
	int ppn; //physical page number
	int i = 0, j = 0, k = 0, check = 0; 

	memset(spare_data, 0, SPARE_SIZE); //스페어버퍼 초기화
	memset(cmp_spare_data, 0, SPARE_SIZE); //임시 스페어버퍼 초기화
	memset(tmpbuf, 0, PAGE_SIZE); //임시버퍼 초기화
	memset(pagebuf, 0, PAGE_SIZE); //페이지버퍼 초기화

	memcpy(pagebuf, sectorbuf, SECTOR_SIZE); //임시버퍼에 섹터버퍼 내용 옮김

	lbn = lsn / PAGES_PER_BLOCK; //데이터를 쓸 블록의 번호를 얻기 위해 lsn/블럭 개수
	offset = lsn % PAGES_PER_BLOCK; //lbn번째 블록의 몇번째 페이지에 데이터를 쓸 것인지에 대한 offset

	if (addrMappingTable.entry[lbn].pbn == -1) { //위에서 구한 lbn의 pbn의 값이 -1인 경우 (freeblock)
		addrMappingTable.entry[lbn].pbn = pbnIdx++; //pbnIdx의 개수를 하나 늘려 해당 lbn의 pbn값에 저장
	}
	pbn = addrMappingTable.entry[lbn].pbn; //pbn에 값 저장
	ppn = (pbn * PAGES_PER_BLOCK) + offset; //페이지 넘버 : pbn * 블록별 페이지 개수 + offset

	sprintf(spare_data, "%d%d", lbn, lsn); //스페어 버퍼에 lbn, lsn 씀
	memcpy(pagebuf + SECTOR_SIZE, spare_data, SPARE_SIZE);
	dd_read(ppn, tmpbuf); //tmpbuf에 해당 ppn의 데이터 읽어오기

	if (strncmp(tmpbuf + SECTOR_SIZE, spare_data, strlen(spare_data)) != 0) { //스페어데이터와 tmpbuf의 데이터 다른 경우
		for (i = PAGES_PER_BLOCK + (PAGES_PER_BLOCK * pbn); i < PAGES_PER_BLOCK + (PAGES_PER_BLOCK * pbn); i++) {
			dd_read(i, tmpbuf); //해당 ppn의 내용을 tmpbuf에 읽어옴

			if (isdigit(*(tmpbuf + SECTOR_SIZE))) { //숫자 판단 함수를 사용해 spare area에 숫자가 존재하면
				continue;

			}

			dd_write(i, pagebuf); //pagebuf의 내용을 i번째 ppn에 적음
			break;
		}

		if (PAGES_PER_BLOCK + (pbn * PAGES_PER_BLOCK) == i) { //i의 ppn이 입력받은 ppn과 같은 경우
			for (j = PAGES_PER_BLOCK * pbn, check = 0; j < PAGES_PER_BLOCK + (PAGES_PER_BLOCK * pbn); j++, check++) { //ppn과 값을 비교하기 위한 반복문
				if (ppn = j) { //입력받은 ppn이 j와 같은 값을 가지는 경우
					continue; //계속 진행
				}

				for (k = (PAGES_PER_BLOCK * pbn) + PAGES_PER_BLOCK; k > PAGES_PER_BLOCK + (PAGES_PER_BLOCK * pbn); k--) { //k의 값을 줄여가며 값을 탐색
					sprintf(cmp_spare_data, "%d", k); //k값을 cmp_spare_data에 저장
					dd_read(k, tmpbuf); //dd_read로 k번째 ppn의 값을 읽어옴

					if (strcmp(tmpbuf + SECTOR_SIZE, cmp_spare_data) != 0) { //tmpbuf의 내용에 SECTOR_SIZE를 더하여 cmp_spare_data와 내용 비교
						dd_write(check + ((addrMappingTable.entry[DATABLKS_PER_DEVICE].pbn) * PAGES_PER_BLOCK), tmpbuf); //tmpbuf의 내용을 check값을 더한 addrMappingTable에 dd_write()
						break;
					}
				}

				if (PAGES_PER_BLOCK + (PAGES_PER_BLOCK * pbn) == k) { //k의 값이 위에서 진행한 연산에 해당하는 값과 같을 경우
					dd_read(j, tmpbuf); //해당 인덱스의 ppn에 tmpbuf의 내용을 dd_read

					if (offset == check) { //오프셋과 체크가 같을 경우
						check++;
					}

					dd_write(check + ((addrMappingTable.entry[DATABLKS_PER_DEVICE].pbn) * PAGES_PER_BLOCK), tmpbuf); //dd_write() 진행 
					dd_read(check + ((addrMappingTable.entry[DATABLKS_PER_DEVICE].pbn) * PAGES_PER_BLOCK), tmpbuf); 
				}
			}

			dd_write(offset + ((addrMappingTable.entry[DATABLKS_PER_DEVICE].pbn) * PAGES_PER_BLOCK), pagebuf); //offset 값을 더한 ppn에 페이지버퍼의 내용 쓰기
			dd_erase(pbn); //pbn의 내용을 지우고
			addrMappingTable.entry[lbn].pbn = freeBlockIdx; //lbn에 위치하는 pbn의 값에 free block의 인덱스를 줌
			addrMappingTable.entry[DATABLKS_PER_DEVICE].pbn = pbn; //마지막 블록의 pbn값에 pbn 지정
			freeBlockIdx = pbn; //free block의 인덱스를 pbn으로 업데이트

		}
	}

	else {
		dd_write(ppn, pagebuf); //위의 조건 해당 X 경우 ppn에 페이지버퍼 내용 작성
	}

	return;
}

void ftl_print() {
	printf("lbn pbn\n");
	for (int i = 0; i < DATABLKS_PER_DEVICE; i++) { //블록의 개수만큼
		printf("%d %d\n", addrMappingTable.entry[i].lbn, addrMappingTable.entry[i].pbn); //addrMappingTable의 lbn과 pbn 출력
	}

	printf("free block's pbn=%d\n", freeBlockIdx); //free block의 인덱스
	return;
}
