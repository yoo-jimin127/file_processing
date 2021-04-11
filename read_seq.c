// 파일처리 과제2_ 20193017 유지민 _ read_seq.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>

//필요하면 header file 추가 가능

#define FILENAME_SIZE 512
#define MAX_SIZE 512

int main(int argc, char **argv) {
    // 레코드 파일로부터 전체 레코들을 순차적으로 read할 때
    // 걸리는 전체 시간을 측정하는 코드 구현
    
    FILE *fptr = NULL; //파일포인터
    char *filename; //파일명 저장 변수
    int record_num = 0; //레코드 수 저장 변수
	int curr_pointer = 0; //파일의 현재 포인터 저장 변수
    struct timeval start_time; //timeval start
    struct timeval end_time; //timeval end
    char buf[MAX_SIZE];

    if (argc < 2) {
        fprintf(stderr, "Usage : %s <filename>\n", argv[0]);
        exit(1);
    }   

    filename = (char *)malloc(sizeof(char) * FILENAME_SIZE);
    memset(filename, 0, FILENAME_SIZE); // 파일명 저장 배열 초기화
    
    strcpy(filename, argv[1]); //파일명 저장

	if ((fptr = fopen(filename, "rb")) == NULL) { //레코드 수를 읽기 위해 rb모드로 fopen()
		fprintf(stderr, "<filename> fopen() error : read binary mode\n");
		exit(1);
	}

	fread(&record_num, sizeof(int), 1, fptr);
	curr_pointer = ftell(fptr);
	fclose(fptr);

    if ((fptr = fopen(filename, "r+")) == NULL) { //읽기 모드로 파일 오픈
        fprintf(stderr, "<filename> fopen() error\n");
        exit(1);
    }   

    fseek(fptr, curr_pointer, SEEK_SET); //파일 포인터 SEEK_SET부터 헤더 레코드 읽은 뒤로 위치

    gettimeofday(&start_time, NULL); //측정 시작
  
	for (int i = 0; i < record_num; i++) {
		memset(buf, 0, MAX_SIZE);
		fread(buf, 1, sizeof(buf), fptr); //레코드 읽어옴
		printf("record num : %d buf : %s\n", i, buf);
	}

    gettimeofday(&end_time, NULL); //측정 끝

    end_time.tv_sec = end_time.tv_sec - start_time.tv_sec;
    end_time.tv_usec = end_time.tv_usec - start_time.tv_usec;

    end_time.tv_usec += (end_time.tv_sec * 1000000);

    printf("#records: %d elapsed_time: %ld us\n", record_num, end_time.tv_usec);

    return 0;
}
