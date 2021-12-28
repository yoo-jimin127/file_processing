// 파일처리_(나)반_과제1_20193017_유지민_overwrite.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE 999

int main (int argc, char * argv[]) {
	char *filename; //파일명
	FILE *fptr = NULL; //파일 포인터
	int overwrite_offset = 0; //오프셋
	char overwrite_data[MAX_SIZE] = ""; //데이터
	int data_cnt = 0; //데이터크기 카운트

	if (argc < 4) { //프로그램 실행이 잘못 되었을 경우
		fprintf(stderr, "usage : a.out <filename> <offset> <data>\n");
		exit(1);
	}

	filename = (char *)malloc(sizeof(char) * 300);
	strcpy(filename, argv[1]); //파일명 저장
	overwrite_offset = atoi(argv[2]); //오프셋 저장
	strcpy(overwrite_data, argv[3]); //데이터 저장

	if ((fptr = fopen(filename, "r+")) == NULL) { //파일을 r+ 모드로 fopen()
		fprintf(stderr, "<filename> fopen error\n");
		exit(1); //파일 오픈에 실패하면 에러처리 후 비정상종료
	}

	for (int i = 0; i < MAX_SIZE; i++) { //데이터 크기 카운트
		if (overwrite_data[i] == '\0') {
			break;
		}
		data_cnt++;
	}
	fseek(fptr, overwrite_offset, SEEK_SET); //파일포인터를 파일 처음 부분에서 오프셋만큼 이동
	fwrite(overwrite_data, data_cnt, 1, fptr); //데이터 크기만큼 파일에 데이터를 overwrite
	
	fclose(fptr); //파일 닫기
	return 0;
}
