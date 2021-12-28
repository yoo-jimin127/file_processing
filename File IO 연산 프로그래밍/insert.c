// 파일처리_(나)반_과제1_20193017_유지민_insert.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE 999

int main (int argc, char* argv[]) {
	char *filename; //파일명
	FILE *fptr = NULL; //파일 포인터
	int insert_offset = 0; //오프셋
	char insert_data[MAX_SIZE] = ""; //데이터
	char tmp_data[MAX_SIZE] = ""; //오프셋 +1의 내용을 잠시 저장해두는 버퍼
	int data_cnt = 0; //데이터 크기 카운트
	int reminder_cnt = 0;

	if (argc < 4) { //프로그램 실행이 잘못 되었을 경우
		fprintf(stderr, "usage : a.out <filename> <offset> <data>\n");
		exit(1);
	}

	filename = (char *)malloc(sizeof(char) *300); //파일명 저장 배열 메모리 동적할당
	strcpy(filename, argv[1]); //파일명 저장
	insert_offset = atoi(argv[2]); //오프셋 저장
	strcpy(insert_data, argv[3]); //데이터 저장

	if ((fptr = fopen(filename, "r+")) == NULL) {//파일을 r+모드로 fopen()
		fprintf(stderr, "<filename> fopen() error\n");
		exit(1); //파일 오픈에 실패하면 에러처리 후 비정상 종료
	}
	
	for (int i = 0; i < MAX_SIZE; i++) { //NULL을 만날 때까지의 데이터 크기 카운트
		if (insert_data[i] == '\0') {
			break;
		}
		data_cnt++;
	}

	fseek(fptr, insert_offset, SEEK_SET); //파일 포인터를 데이터를 삽입할 오프셋으로 옮김

	while(feof(fptr) == 0) {
		reminder_cnt = fread(tmp_data, sizeof(char), MAX_SIZE, fptr);
	}

	fseek(fptr, insert_offset, SEEK_SET); //파일 포인터를 데이터를 삽입할 오프셋으로 옮김
	fwrite(insert_data, 1, data_cnt, fptr); //삽입할 데이터 파일에 적기
	fseek(fptr, insert_offset + data_cnt, SEEK_SET);

	fwrite(tmp_data, 1, reminder_cnt, fptr);

	fclose(fptr);
	return 0;
}
