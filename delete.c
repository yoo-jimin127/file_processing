// 파일처리_(나)반_과제1_20193017_유지민_delete.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE 999

int main (int argc, char *argv[]) {
	char *filename; //파일명
	FILE *fptr = NULL; //파일 포인터
	int delete_offset = 0; //오프셋
	int delete_bytes = 0; //삭제 바이트 수
	char front_data[MAX_SIZE]; //삭제 오프셋 이전의 데이터
	int front_cnt = 0; //삭제 오프셋 이전의 데이터 수
	char back_data[MAX_SIZE]; //삭제 오프셋 이후의 데이터
	int back_cnt = 0; //삭제 오프셋 이후의 데이터 수

	filename = (char *)malloc(sizeof(char) * 300); //파일명 저장 배열 메모리 동적할당
	strcpy(filename, argv[1]); //파일명 저장
	delete_offset = atoi(argv[2]); //오프셋 저장
	delete_bytes = atoi(argv[3]); //삭제 바이트 수 저장

	if ((fptr = fopen(filename, "r+")) == NULL) { //파일을 r+ 모드로 fopen()
		fprintf(stderr, "<filename> fopen() error\n");
		exit(1);
	}

	fseek(fptr, 0, SEEK_SET); //파일포인터의 위치를 파일의 시작에 놓음
	front_cnt = fread(front_data, delete_offset, 1, fptr); //front_data에 오프셋 이전의 데이터들을 저장함

	fseek(fptr, delete_offset + delete_bytes, SEEK_SET); //파일포인터를 <오프셋 + 삭제할 바이트 수>의 위치로 옮겨 삭제 데이터 이후의 내용을 저장하도록 fseek()

	while(feof(fptr) == 0) {
		back_cnt = fread(back_data, sizeof(char), MAX_SIZE, fptr); //삭제할 내용 이후의 데이터 저장
	}

	fclose(fptr); //삭제 전 데이터를 저장해두기위한 작업에서의 파일포인터 닫기

	if ((fptr = fopen(filename, "w")) == NULL) { //파일의 내용을 지우기 위해 존재하는 파일이 초기화되는 쓰기모드 "w"로 재오픈
		fprintf(stderr, "<filename> fopen() error\n");
		exit(1); //fopen() 실패 시 예외처리
	}
	fwrite(front_data, 1, strlen(front_data), fptr); //삭제 오프셋 이전의 데이터 fwrite()
	fwrite(back_data, 1, back_cnt, fptr); //삭제 오프셋 이후이 데이터 fwrite()

	fclose(fptr); //파일 닫기

	return 0;
}

