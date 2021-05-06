// 파일처리 과제 2 20193017 유지민 _ create_file.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
// 필요하면 header file을 추가할 수 있음

#define FILENAME_SIZE 512

int main(int argc, char **argv) {
    // 사용자로부터 입력 받은 레코드 수만큼의 레코드를 레코드 파일에 저장하는 코드를 구현
    FILE *fptr = NULL; //파일 포인터
    char *filename; //파일명
    int record_num = 0; //레코드 수
    char record_data[250]; //250바이트 짜리 레코드
	char buf[250] = ""; //임시 버퍼
	char data[6] = "abcde";//레코드의 데이터를 저장하기 위한 임시의 데이터 저장 배열
    
    if (argc < 3) { //실행 인자 입력이 제대로 되지 않은 경우
        fprintf(stderr, "usage : %s <number of record> <filename>\n", argv[0]);
        exit(1);
    }   

    record_num = atoi(argv[1]); // 레코드 수 저장

    filename = (char *)malloc(sizeof (char) * FILENAME_SIZE); //동적 메모리 할당

    memset(filename, 0, FILENAME_SIZE); //파일명 저장 배열 메모리 초기화
    strcpy(filename, argv[2]); //파일명 저장
	
	if ((fptr = fopen(filename, "wb")) == NULL) { //파일에 헤더레코드를 binary integer로 작성하기 위한 fopen()
		fprintf(stderr, "<filename> fopen() error : writing header record\n");
		exit(1);
	}

	fwrite(&record_num, sizeof(int), 1, fptr); // binary int로 헤더 레코드 작성
	//fseek(fptr, 0, SEEK_END);
	//int header_size = ftell(fptr);
	//fseek(fptr, 0, SEEK_SET);
	fclose(fptr); //바이너리 쓰기 모드로 열려있는 파일 닫기

	//fptr = fopen(filename, "rb");
	//int tmp = 0;
	//fread(&tmp, 1, sizeof(int), fptr);
	//printf("header : %d\n", tmp);
	//fclose(fptr);

	memset(buf, 0, 250);
	for (int i = 0; i < 5; i++) { //레코드 저장 내용 초기화 부분
		for (int j = 0; j < 50; j++) {
			strcpy(&buf[j + (i*50)], &data[i]);
		}
	}

    if ((fptr = fopen(filename, "a+")) == NULL) { //파일 append 모드로 fopen()
        fprintf(stderr, "<filename> fopen() error\n");
        exit(1);//동일 파일 존재 시 내용 지워짐, 빈 파일 새로 생성
    }   

    for (int i = 0; i < record_num; i++) {
        memset(record_data, 0, 250); //레코드 데이터 초기화
        strcpy(record_data, buf); //버퍼의 내용 레코드에 복사
        fwrite(record_data, 1, sizeof(record_data), fptr); //250 바이트만큼 파일에 작성
    }   

    fclose(fptr);
    return 0;
}
