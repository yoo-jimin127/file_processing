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
	int lbn; //logical block number
	char dummyData[SPARE_SIZE - 8];
	int check_block; //유효한 block인지 확인하기 위한 변수
}SpareData;

typedef struct {
	MappingTableEntry entry[DATAPAGES_PER_DEVICE];
}AddrMappingTable;

AddrMappingTable addrMappingTable; //address mapping table 구조체 변수
int freeBlockIdx; //free block의 pbn 값의 초기화를 위해 필요한 인덱스 변수

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
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf) {
	char pagebuf[PAGE_SIZE]; //페이지 버퍼(페이지 단위로 데이터를 읽어오기 위해 PAGE_SIZE만큼)
	int lbn, offset; //데이터를 읽을 lbn과 offset
	int pbn; //physical block number
	int ppn; //physical page number(인자로 주어진 lsn에 대응되는 ppn)

	lbn = lsn / PAGES_PER_BLOCK; //데이터를 읽을 블록의 번호를 얻기 위해 lsn/블럭 개수
	offset = lsn % PAGES_PER_BLOCK; //lbn번째 블록의 몇번째 페이지에 데이터를 읽을 것인지에 대한 offset
	
	pbn = addrMappingTable.entry[lbn].pbn; //인자로 주어진 lsn으로부터 대응되는 ppn을 구함

	if (pbn == -1) { //pbn이 -1인 경우(free page인 경우)
		printf("---There is No Data---\n");
		return;
	}

	else { //pbn이 -1이 아닌 경우(해당 영역에 데이터가 존재하는 경우)
		dd_read(pbn, pagebuf); //페이지 단위로 데이터를 읽어옴
		memcpy(sectorbuf, pagebuf, SECTOR_SIZE); //flash memory에서 페이지를 읽어와 sectorbuf에 데이터를 복사함
		printf("%s\n", sectorbuf); //복사된 데이터(sectorbuf) 출력
	}

	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char *sectorbuf) {
	char spare_data[SPARE_SIZE]; //spare영역의 메타데이터 작성을 위한 배열
	char pagebuf[PAGE_SIZE]; //페이지 버퍼
	int lbn, offset; //데이터를 쓸 lbn과 offset
	int pbn;
	int ppn;
	
	memset(spare_data, 0, SPARE_SIZE);
	memset(pagebuf, 0, PAGE_SIZE);

	lbn = lsn / PAGES_PER_BLOCK; //데이터를 쓸 블록의 번호를 얻기 위해 lsn/블럭 개수
	offset = lsn % PAGES_PER_BLOCK; //lbn번째 블록의 몇번째 페이지에 데이터를 쓸 것인지에 대한 offset
	ppn = addrMappingTable.entry[lsn].pbn;

	if (ppn == -1) { //lsn에 최초로 데이터를 쓰는 경우
		
	}

	else { //lsn에 데이터를 갱신(update)하는 경우

	}

	return;
}

void ftl_print() {
	printf("lbn pbn\n");
	for (int i = 0; i < DATABLKS_PER_DEVICE; i++) {
		printf("%d %d\n", addrMappingTable.entry[i].lbn, addrMappingTable.entry[i].pbn);
	}

	printf("free block's pbn=%d\n", freeBlockIdx);
	return;
}
