// 파일처리_(나)반_과제1_20193017_유지민_copy.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE 999

int main (int argc, char* argv[]) {
	char *original_filename; //원본 파일명
	char *copy_filename; //복사본 파일명
	FILE *read_fptr = NULL; //파일의 내용을 read할 목적으로 사용되는 파일포인터
	FILE *write_fptr = NULL; //파일의 내용을 Write할 목적으로 사용되는 파일포인터
	char buf[10] = ""; //파일 내용 복사 버퍼
	int cnt = 0; //읽어들인 파일의 데이터 counter

	if (argc < 3) { //파일 실행 방법이 잘못된 경우
		fprintf(stderr, "usage : a.out <filename> <copy_file>\n");
		exit(1);
	}
	
	original_filename = (char *)malloc(sizeof(char) * 999); //원본 파일명 메모리 동적할당
	copy_filename = (char *)malloc(sizeof(char) * 999); //복사본 파일명 메모리 동적할당

	strcpy(original_filename, argv[1]); //표준입력으로 전달받은 원본파일명을 original_filename변수에 저장
	strcpy(copy_filename, argv[2]); //표준입력으로 전달받은 복사본파일명을 copy_filename변수에 저장

	if ((read_fptr = fopen(original_filename,"r")) == NULL) { //읽기 전용 모드로 원본파일 fopen
		fprintf(stderr, "원본파일을 여는 작업을 실패하였습니다.\n");
		exit(1); //정상적으로 열리지 않을 경우 에러메시지 출력 후 비정상 종료처리
	}
	
	if ((write_fptr = fopen(copy_filename, "w")) == NULL) { //파일을 읽기/덧붙이기 모드로 open
		fprintf(stderr, "복사본 파일을 생성하는 작업을 실패하였습니다.\n");
		exit(1); //파일이 정상적으로 생성, fopen 되지 않은 경우 에러메시지 출력 후 비정상 종료처리
	}

	while (feof(read_fptr) == 0) { //파일포인터가 end of file이 아닐 때까지 반복
		fread(buf, 1, 10, read_fptr); //10바이트 단위로 원본파일에서 데이터를 읽어옴(복사)
		fwrite(buf, 1, strlen(buf), write_fptr); //10바이트 단위로 복사본파일에 버퍼에 저장된 데이터를 붙여넣음(붙여넣기)
		memset(buf, 0, 10); //buf 내용 초기화
	}

	fclose(read_fptr);
	fclose(write_fptr); // fopen() 되어있는 파일 close
	return 0;
}
