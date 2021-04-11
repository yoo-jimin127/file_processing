#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

//필요하면 header file 추가 가능

#define SUFFLE_NUM  10000   // 이 값은 마음대로 수정할 수 있음.
#define FILENAME_SIZE 512
#define MAX_SIZE 250

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// 필요한 함수가 있으면 더 추가할 수 있음.

int main(int argc, char **argv) {
    int *read_order_list;
    int num_of_records; // 레코드 파일에 저장되어 있는 전체 레코드의 수
    FILE *fptr = NULL; //파일 포인터
    char *filename; //파일명
    int curr_pointer = 0; //현재 파일 포인터를 저장할 수 있는 파일 포인터
    char buf[MAX_SIZE] = ""; //버퍼 생성
    struct timeval start_time; //timeval start
    struct timeval end_time; //timeval end
	struct timeval differ_time;

    if (argc < 2) { //프로그램 실행 인자 잘못 전달된 경우
        fprintf(stderr, "Usage : %s <filename>\n", argv[0]);
        exit(1);
    }   

    filename = (char *)malloc(sizeof(char) * FILENAME_SIZE); // 파일명 저장 배열 동적 할당
    memset(filename, 0, FILENAME_SIZE); //메모리 초기화

    strcpy(filename, argv[1]); //파일명 저장

	if ((fptr = fopen(filename, "rb")) == NULL) { //binary read 모드로 파일 오픈
		fprintf(stderr, "<filename> fopen() error : read binary mode\n");
		exit(1);
	}

	fread(&num_of_records, sizeof(int), 1, fptr); //레코드 수를 읽어옴
	curr_pointer = ftell(fptr); //레코드 수 읽고 난 뒤 파일 포인터 읽어옴
	fclose(fptr); //바이너리 리드 모드로 열린 파일포인터 닫기

    if ((fptr = fopen(filename, "r")) == NULL) { //파일 Read모드로 열기
        fprintf(stderr, "<filename> fopen() error\n");
        exit(1);
    }   

    read_order_list = (int *)calloc(num_of_records, sizeof(int)); //배열의 동적할당 calloc() 사용

    // 이 함수를 실행하면 'read_order_list' 배열에는 읽어야 할 레코드 번호들이 나열되어 저장됨
    GenRecordSequence(read_order_list, num_of_records);

    // 'read_order_list'를 이용하여 random 하게 read 할 때 걸리는 전체 시간을 측정하는 코드 구현
    gettimeofday(&start_time, NULL); //시간 측정 시작
	
	fseek(fptr, curr_pointer, SEEK_SET); //레코드 수를 읽어 온 이후의 위치로 파일 포인터 위치 옮김
    for (int i = 0; i < num_of_records; i++) { //레코드 수만큼 반복
        fseek(fptr, read_order_list[i] * 250, SEEK_CUR); //250바이트씩 읽고 포인터 위치 변경
        fread(buf, sizeof(char) * 250, 1, fptr); //250바이트씩 버퍼에 읽어옴
		//printf("num_of_records : %d buf : %s\n", i, buf);
    }

    gettimeofday(&end_time, NULL); //시간 측정 종료

    end_time.tv_sec = end_time.tv_sec - start_time.tv_sec; //차이 sec 단위로 계산
    end_time.tv_usec = end_time.tv_usec - start_time.tv_usec; //차이 usec 단위로 계산

    end_time.tv_usec += (end_time.tv_sec * 1000000); //usec에 sec 차이를 추가

    printf("#records: %d elapsd_time: %ld us\n", num_of_records, end_time.tv_usec);

    return 0;
}

void GenRecordSequence(int *list, int n)
{
    int i, j, k;

    srand((unsigned int)time(0));

    for(i=0; i<n; i++)
    {
        list[i] = i;
    }

    for(i=0; i<SUFFLE_NUM; i++)
    {
        j = rand() % n;
        k = rand() % n;
        swap(&list[j], &list[k]);
    }
}

void swap(int *a, int *b)
{
    int tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}
