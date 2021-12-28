#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터를 저장합니다. 그리고 난 후 writePage() 함수를 호출하여 수정된 pagebuf를
// 레코드 파일에 저장합니다. 반드시 페이지 단위로 읽거나 써야 합니다.
//
// 주의: 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근해서 처리한다.

typedef struct _DeletedRecord {
	char delimiter; //삭제된 레코드임을 의미하는 delimeter mark
	int RRN; //가장 최근에 삭제된 record번호
	int RPN; //가장 최근에 삭제된 page번호
}DeletedRecord;

typedef struct _HeaderRecord {
	int total_datapage; //전체 데이터 페이지 수
	int total_record; //레코드 파일에 존재하는 모든 레코드 수
	int deleted_pagenum; //페이지 번호 (가장 최근에 삭제된 레코드 가리키는)
	int deleted_recordnum; //레코드 번호 (가장 최근에 삭제된 레코드 가리키는)
	char dummy[PAGE_SIZE - 16]; //더미데이터 저장을 위한 배열
}HeaderRecord;

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE *fp, char *pagebuf, int pagenum) {
	 memset(pagebuf, 0, PAGE_SIZE); //페이지 사이즈만큼의 데이터를 0으로 초기화
	 fseek(fp, PAGE_SIZE * pagenum, SEEK_SET); //위치 옮겨서
	 fread(pagebuf, PAGE_SIZE, 1, fp); //페이지 사이즈만큼 한 번 읽어옴
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다. 
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum) {
	fseek(fp, PAGE_SIZE * pagenum, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);
}

//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 
// 
void pack(char *recordbuf, const Person *p) {
	sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#", p -> id, p -> name, p -> age, p -> addr, p -> phone, p -> email);
}

// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
//
void unpack(const char *recordbuf, Person *p) {
	char tmp_recordbuf[MAX_RECORD_SIZE]; //레코드 버퍼 임시 저장 공간
	char *token; //토큰 분리 위한 포인터
	
	memset(tmp_recordbuf, 0, MAX_RECORD_SIZE);
	strcpy(tmp_recordbuf, recordbuf); //임시 저장 공간에 복사

	token = strtok(tmp_recordbuf, "#"); //구분자 기준으로 토큰 분리
	strcpy(p -> id, token); //주민번호 저장

	strtok(NULL, "#");
	strcpy(p -> name, token); //이름 저장

	strtok(NULL, "#");
	strcpy(p -> age, token); //나이 저장

	strtok(NULL, "#");
	strcpy(p -> addr, token); //주소 저장

	strtok(NULL, "#");
	strcpy(p -> phone, token); //전화번호 저장

	strtok(NULL, "#");
	strcpy(p -> email, token); //이메일주소 저장
}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값들을 구조체에 저장한 후 아래 함수를 호출한다.
//packing은 여기서 하기
void add(FILE *fp, const Person *p) {
	char pagebuf[PAGE_SIZE];
	char *recordbuf = (char*)malloc(sizeof(char) * MAX_RECORD_SIZE);

	int cnt = 0; //카운터 변수
	int recordnum = 0; //레코드 번호
	int totalRecord = 0; //페이지에 저장되어 있는 전체 레코드 수
	int addcheck = 0;

	int offset = 0, length = 0; //데이터 페이지의 header area에 담길 offset, length
	long long sum = 0; //offset 값 저장을 위해 필요한 변수

	HeaderRecord headerRecord; //헤더 레코드 구조체
	DeletedRecord deletedRecord; //삭제 레코드 구조체

	pack(recordbuf, p); //person구조체 pack
	sum = strlen(recordbuf); //recordbuf의 length 저장
	//printf("in add func. recordbuf : %s, strlen : %ld\n", recordbuf, strlen(recordbuf));
	readPage(fp, (char*)&headerRecord, 0); //readPage 호출을 통해 헤더레코드의 내용 읽어옴
	//printf("headerRecord before: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);

	if (headerRecord.deleted_pagenum == -1 && headerRecord.deleted_recordnum == -1) { //삭제된 레코드가 존재하지 않는 경우
		for (int i = 1; i < headerRecord.total_datapage; i++) {
			totalRecord = PAGE_SIZE / strlen(recordbuf); //레코드 개수 저장
			recordnum = totalRecord - (((headerRecord.total_datapage - 1) * totalRecord) - headerRecord.total_record); //데이터를 추가할 레코드 번호

			readPage(fp, pagebuf, i); //페이지 버퍼 읽어오기

			for (int j = 0; j < PAGE_SIZE; j++) { //6개의 구조체 변수 내용 저장되면 # cnt ++
				if (pagebuf[j] == '#') {
					cnt++;
				}
			}

			if (totalRecord * 6 != cnt) { //빈 공간이 있어 존재하는 페이지에 쓸 수 있는 경우
				memcpy(strlen(recordbuf) * recordnum + pagebuf, recordbuf, strlen(recordbuf)); //동적으로 레코드버퍼 내용 복사
				
				writePage(fp, pagebuf, i); //해당 인덱스번째부터 데이터페이지에 씀
				headerRecord.total_record += 1;
				addcheck = 1;
				//printf("headerRecord after case 1: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);	
				break;
			}
		}

		if(addcheck == 0) { //빈 공간이 없어 새롭게 페이지를 추가해야 하는 경우
			memset(pagebuf, 0, PAGE_SIZE);
			memcpy(pagebuf, recordbuf, strlen(recordbuf));
			writePage(fp, pagebuf, headerRecord.total_datapage); //해당 데이터 페이지번째에 페이지 버퍼 내용 씀
			//printf("headerRecord after case 2: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);	
			headerRecord.total_datapage += 1;
			headerRecord.total_record += 1; //총 페이지 수와 레코드 수 +1
		}
	}

	else { //삭제된 레코드 공간이 있는 경우
		readPage(fp, pagebuf, headerRecord.deleted_pagenum); //삭제된 페이지 번호 읽어옴
		memcpy(&deletedRecord, (headerRecord.deleted_recordnum * strlen(recordbuf)) + pagebuf, sizeof(DeletedRecord));
		memcpy((headerRecord.deleted_recordnum * strlen(recordbuf)) + pagebuf, recordbuf, strlen(recordbuf));
		writePage(fp, pagebuf, headerRecord.deleted_pagenum);
		//printf("headerRecord after case 3: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);	
		headerRecord.deleted_pagenum = deletedRecord.RPN;
		headerRecord.deleted_recordnum = deletedRecord.RRN; //삭제된 레코드 구조체 RRN , RPN에 저장
	}

	writePage(fp, (char*)&headerRecord, 0); //헤더 레코드 내용 씀
	//printf("headerRecord final: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);	
}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE *fp, const char *id) {
	HeaderRecord headerRecord;
	Person p;
	DeletedRecord deletedRecord;

	char pagebuf[PAGE_SIZE];
	char *recordbuf = (char *)malloc(sizeof(char) * MAX_RECORD_SIZE);

	int recordnum = 0, totalRecord = 0;
	int deletecheck = 0;

	readPage(fp, (char *)&headerRecord, 0); //헤더레코드의 내용을 읽어옴

	for (int i = 1; i < headerRecord.total_datapage; i++) {
		totalRecord = PAGE_SIZE / MAX_RECORD_SIZE; //총 레코드 수 구하기
		recordnum = totalRecord - ((totalRecord * (headerRecord.total_datapage - 1)) - headerRecord.total_record);
		readPage(fp, pagebuf, i); //해당 인덱스번째 페이지에 레코드 넣기

		for (int j = 0; j < totalRecord; j++) {
			memcpy(recordbuf, (j * MAX_RECORD_SIZE) + pagebuf, MAX_RECORD_SIZE);
			unpack(recordbuf, &p);

			if (!strcmp(p.id, id)) {
				deletedRecord.RRN = headerRecord.deleted_recordnum;
				deletedRecord.RPN = headerRecord.deleted_pagenum;
				deletedRecord.delimiter = '*'; //RRN, RPN, 구분자 갱신

				memcpy((j * MAX_RECORD_SIZE) + pagebuf, &deletedRecord, sizeof(DeletedRecord)); //삭제 레코드에 쓰기
				writePage(fp, pagebuf, i);

				deletecheck = 1;
				headerRecord.deleted_recordnum = j;
				headerRecord.deleted_pagenum = i;
				writePage(fp, (char *)&headerRecord, 0);
				break;
			}
		}

		if (deletecheck == 1) {
			break;
		}
	}
}

int main(int argc, char *argv[]) {
	FILE *fp;  // 레코드 파일의 파일 포인터
	HeaderRecord headerRecord; //헤더 레코드 저장 구조체
	Person p; //person구조체
	
	if (access(argv[2], F_OK) != 0) { //파일이 존재하지 않는 경우
		if ((fp = fopen(argv[2], "w+")) == NULL) { //쓰기 모드로 열고
			fprintf(stderr, "%s fopen() w+ mode error\n", argv[2]);
			exit(1);
		}

		//헤더 레코드 초기화
		headerRecord.total_datapage = 1;
		headerRecord.total_record = 0;
		headerRecord.deleted_pagenum = -1;
		headerRecord.deleted_recordnum = -1;
		memset(headerRecord.dummy, 0, PAGE_SIZE - 16);

		writePage(fp, (char *)&headerRecord, 0);

		fclose(fp); //binary integer 모드로 헤더 레코드 내용 썼으니 닫기
	}

	if ((fp = fopen(argv[2], "r+")) == NULL) {
		fprintf(stderr, "%s fopen() r+ mode error(second open in main function\n", argv[2]);
		exit(1);
	}

	if (strcmp(argv[1], "a") == 0) { //a옵션을 주어 레코드 추가
		strcpy(p.id, argv[3]);
		strcpy(p.name, argv[4]);
		strcpy(p.age, argv[5]);
		strcpy(p.addr, argv[6]);
		strcpy(p.phone, argv[7]);
		strcpy(p.email, argv[8]);

		add(fp, &p);
	}

	else if (strcmp(argv[1], "d") == 0) { //d옵션을 주어 레코드 삭제
		delete(fp, argv[3]);
	}


	else if ((strcmp(argv[1], "a") != 0) || strcmp(argv[1], "d") != 0) {
		printf("do not support this option\n");
	}
	
	fclose(fp);
	
	return 1;
}
