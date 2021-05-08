// 
// 과제3의 채점 프로그램은 기본적으로 아래와 같이 동작함
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blockmap.h"

FILE *flashfp;
FILE *testfp;
/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void ftl_print();

//
// 이 함수는 file system의 역할을 수행한다고 생각하면 되고,
// file system이 flash memory로부터 512B씩 데이터를 저장하거나 데이터를 읽어 오기 위해서는
// 각자 구현한 FTL의 ftl_write()와 ftl_read()를 호출하면 됨
//
int main(int argc, char *argv[])
{
	char *blockbuf;
	char *testbuf;
    char sectorbuf[SECTOR_SIZE];
	int lsn, i;

    flashfp = fopen("flashmemory", "w+b");
	if(flashfp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}
	   
    //
    // flash memory의 모든 바이트를 '0xff'로 초기화한다.
    // 
    blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, flashfp);
	}

	free(blockbuf);

	ftl_open();    // ftl_read(), ftl_write() 호출하기 전에 이 함수를 반드시 호출해야 함

	//
	// ftl_write() 및 ftl_read() 테스트 코드 작성
	ftl_write(0, "aaaaa");
	ftl_write(1, "bbbbb");
	ftl_write(2, "ccccc");
	ftl_write(3, "ddddd");
	ftl_write(4, "eeeee");
	ftl_write(5, "fffff");
	ftl_write(6, "ggggg");
	ftl_write(7, "hhhhh");
	ftl_write(8, "iiiii");
	ftl_write(9, "jjjjj");
	ftl_write(10, "kkkkk");
	ftl_write(11, "lllll");
	ftl_write(12, "mmmmm");
	ftl_write(13, "nnnnn");
	ftl_write(14, "ooooo");
	//ftl_write(3, "d1d1d1d1d1");
	//ftl_write(9, "j1j1j1j1j1");
	//ftl_write(13, "n1n1n1n1n1");

	
	ftl_read(0, sectorbuf);
	ftl_read(1, sectorbuf);
	ftl_read(2, sectorbuf);
	ftl_read(3, sectorbuf);
	ftl_read(4, sectorbuf);
	ftl_read(5, sectorbuf);
	ftl_read(6, sectorbuf);
	ftl_read(7, sectorbuf);
	ftl_read(8, sectorbuf);
	ftl_read(9, sectorbuf);
	ftl_read(10, sectorbuf);
	ftl_read(11, sectorbuf);
	ftl_read(12, sectorbuf);
	ftl_read(13, sectorbuf);
	ftl_read(14, sectorbuf);
	
	/*
	testfp = fopen("flashmemory", "rb");
	testbuf = (char *)malloc(BLOCK_SIZE);
	memset(testbuf, 0, BLOCK_SIZE);

	for (int j = 0; j < BLOCKS_PER_DEVICE; j++) {
		fread(testbuf, BLOCK_SIZE, 1, testfp);
		printf("%s\n", testbuf);
	}*/

	ftl_print();

	//


	fclose(flashfp);

	return 0;
}
