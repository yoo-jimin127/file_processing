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
#define MAX_SIZE 999

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// 필요한 함수가 있으면 더 추가할 수 있음.

int main(int argc, char **argv) {
    int *read_order_list;
    int num_of_records; // 레코드 파일에 저장되어 있는 전체 레코드의 수
    FILE *fptr = NULL;
    char *filename;
    int curr_pointer = 0;
    char buf[MAX_SIZE];
    struct timeval start_time;
    struct timeval end_time;

    if (argc < 2) {
        fprintf(stderr, "Usage : %s <filename>\n", argv[0]);
        exit(1);
    }   

    filename = (char *)malloc(sizeof(char) * FILENAME_SIZE);
    memset(filename, 0, FILENAME_SIZE);

    strcpy(filename, argv[1]);

	if ((fptr = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "<filename> fopen() error : read binary mode\n");
		exit(1);
	}

	fread(&num_of_records, sizeof(int), 1, fptr);
	curr_pointer = ftell(fptr);
	fclose(fptr);

    if ((fptr = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "<filename> fopen() error\n");
        exit(1);
    }   

    read_order_list = (int *)calloc(num_of_records, sizeof(int)); //배열의 동적할당 calloc() 사용

    // 이 함수를 실행하면 'read_order_list' 배열에는 읽어야 할 레코드 번호들이 나열되어 저장됨
    GenRecordSequence(read_order_list, num_of_records);

    // 'read_order_list'를 이용하여 random 하게 read 할 때 걸리는 전체 시간을 측정하는 코드 구현
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < num_of_records; i++) {
        fseek(fptr, read_order_list[i] * 250, curr_pointer);
        fread(buf, sizeof(char) * 250, 1, fptr); //250바이트씩 버퍼에 읽어옴
        memset(buf, 0, MAX_SIZE);
    }

    gettimeofday(&end_time, NULL);

    end_time.tv_sec = end_time.tv_sec - start_time.tv_sec;
    end_time.tv_usec = end_time.tv_usec - start_time.tv_usec;

    end_time.tv_usec += (end_time.tv_sec * 1000000);

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
