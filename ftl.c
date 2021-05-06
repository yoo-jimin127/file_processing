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

typedef struct MappingTableEntry{
	int ppn; //physical page number
};

typedef struct SpareData {
	int lpn; //logical page number
	char dummyData[SPARE_SIZE - 8];
	int check_block; //유효한 block인지 확인하기 위한 변수
};

typedef struct AddrMappingTable {
	MappingTableEntry entry[DATAPAGES_PER_DEVICE];
}

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

	for (int i = 0; i < DATAPAGES_PER_DEVICE; i++) {
		addrMappingTable.entry[i].ppn = -1; //address mapping table의 entry의 pbn을 초기화시키는 작업
	}

	freeBlockIdx = BLOCKS_PER_DEVICE -1; // free block의 pbn 값을 맨 마지막 블록의 인덱스로 초기화
	
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf) {
	
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char *sectorbuf) {
	SpareData spareData;
	int pbn;
	return;
}

void ftl_print()
{

	return;
}
