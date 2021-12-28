// 파일처리_(나)반_과제1_20193017_유지민_read.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE 999

int main (int argc, char *argv[]) {
	char *filename; //파일명
	int read_start_offset = 0; //오프셋
	int read_bytes = 0; //읽기 바이트 수
	int reminder = 0; //잔여 읽을 바이트 수
	FILE *fptr = NULL; //파일 포인터
	char buf[MAX_SIZE]; //내용 저장 버퍼

	if (argc < 4) { //프로그램 실행이 잘못된 경우
		fprintf(stderr, "usage : a.out <filename> <offset> <read_bytes>\n");
		exit(1);
	}

	filename = (char *)malloc(sizeof(char) * 300); //파일명 메모리 동적 할당
	strcpy(filename, argv[1]); //파일명 저장
	read_start_offset = atoi(argv[2]); //오프셋 저장
	read_bytes = atoi(argv[3]); //읽기 바이트 수 저장

	memset(buf, 0, MAX_SIZE); //버퍼 초기화

	if ((fptr = fopen(filename, "r")) == NULL) { //읽기 모드로 파일 open
		fprintf(stderr, "fopen() error\n");
		exit(1); //파일 오픈에 실패하면 에러메시지 후 비정상종료
	}

	fseek(fptr, read_start_offset, SEEK_SET); //파일포인터를 파일 처음 부분에서 오프셋만큼 이동
	reminder = fread(buf, read_bytes, 1, fptr); //읽기 바이트 수만큼 버퍼에 데이터 읽어옴

	if (reminder > 0) { //읽을 바이트 수 만큼의 데이터가 존재할 경우
		fwrite(buf, 1, read_bytes, stdout);
	}

	if (reminder == 0) { //읽을 바이트 수 만큼의 데이터가 존재하지 않을 경우
		fwrite(buf, 1, strlen(buf), stdout);
	}

	fclose(fptr); //파일포인터 Close

	return 0;
}
