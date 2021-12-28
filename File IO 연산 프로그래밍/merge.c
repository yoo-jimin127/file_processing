// 파일처리_(나)반_과제1_20193017_유지민_merge.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE 999

int main (int argc, char* argv[]) {
	char *merged_filename; //병합된 파일1
	char *filename2; //파일2
	char *filename3; //파일3
	FILE *merged_fptr1 = NULL; //파일2와 파일3을 병합하여 생성된 파일1
	FILE *fptr2 = NULL; //파일2 포인터
	FILE *fptr3 = NULL; //파일3 포인터
	char buf[MAX_SIZE];

	if (argc < 4) { //프로그램 실행이 잘못된 경우
		fprintf(stderr, "usage : a.out <filename1> <filename2> <filename3>\n");
		exit(1);
	}

	merged_filename = (char *)malloc(sizeof(char) * 300);
	filename2 = (char *)malloc(sizeof(char) * 300);
	filename3 = (char *)malloc(sizeof(char) * 300); //파일명 저장배열 메모리 동적할당

	strcpy(merged_filename, argv[1]);
	strcpy(filename2, argv[2]);
	strcpy(filename3, argv[3]); //argv 원소들을 각 파일명에 옮겨주는 작업

	if ((merged_fptr1 = fopen(merged_filename, "a")) == NULL) { //병합된 파일 생성 및 열기
		fprintf(stderr, "<filename1> fopen() error\n");
		exit(1); //정상적으로 생성되지 않을 경우 에러처리 후 비정상종료
	}

	if ((fptr2 = fopen(filename2, "r")) == NULL) { //파일2 열기
		fprintf(stderr, "<filename2> fopen() error\n");
		exit(1); //정상적으로 fopen() 안될 경우 에러처리 후 비정상종료
	}

	if ((fptr3 = fopen(filename3, "r")) == NULL) { //파일3 열기
		fprintf(stderr, "<filename3> fopen() error\n");
		exit(1); //정상적으로 fopen() 안될 경우 에러처리 후 비정상종료
	}

	while(feof(fptr2) == 0) { //파일2의 내용을 버퍼에 복사한 뒤 병합된 파일1에 쓰는 작업
		int file2_cnt = fread(buf, sizeof(char), MAX_SIZE, fptr2);
		fwrite(buf, file2_cnt, 1, merged_fptr1);
	}

	memset(buf, 0, MAX_SIZE); //버퍼 초기화
	fseek(merged_fptr1, 0, SEEK_END); //파일포인터의 현재 위치 얻음

	while(feof(fptr3) == 0) { //파일3의 내용을 버퍼에 복사한 뒤 병합된 파일1에 쓰는 작업
		int file3_cnt = fread(buf, sizeof(char), MAX_SIZE, fptr3);
		fwrite(buf, file3_cnt, 1, merged_fptr1);
	}

	fclose(merged_fptr1);
	fclose(fptr2);
	fclose(fptr3);

	return 0;
}
