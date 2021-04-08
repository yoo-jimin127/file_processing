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
    char *filename;
    int filesize = 0;
    struct timeval start_time;
    struct timeval end_time;
    char buf[MAX_SIZE];

    if (argc < 2) {
        fprintf(stderr, "Usage : %s <filename>\n", argv[0]);
        exit(1);
    }   

    filename = (char *)malloc(sizeof(char) * FILENAME_SIZE);
    memset(filename, 0, FILENAME_SIZE); // 파일명 저장 배열 초기화
    
    strcpy(filename, argv[1]); //파일명 저장

    if ((fptr = fopen(filename, "r+")) == NULL) { //읽기 모드로 파일 오픈
        fprintf(stderr, "<filename> fopen() error\n");
        exit(1);
    }   

    filesize = fseek(fptr, 0, SEEK_END); //파일 크기 저장
    //printf("filesize = %d\n", filesize);
    fseek(fptr, 0, SEEK_SET); //파일 포인터 SEEK_SET으로 위치

    gettimeofday(&start_time, NULL); //측정 시작
  
    memset(buf, 0, MAX_SIZE);
    fread(buf, 1, sizeof(buf), fptr); //레코드 읽어옴
    //printf("buf : %s\n", buf);

    gettimeofday(&end_time, NULL); //측정 끝

    end_time.tv_sec = end_time.tv_sec - start_time.tv_sec;
    end_time.tv_usec = end_time.tv_usec - start_time.tv_usec;

    end_time.tv_usec += (end_time.tv_sec * 1000000);

    printf("#records: %d elapsed_time: %ld us\n", filesize/250, end_time.tv_usec);

    return 0;
}
