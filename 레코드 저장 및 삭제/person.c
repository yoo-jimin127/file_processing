#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "person.h"
//�ʿ��� ��� ��� ���ϰ� �Լ��� �߰��� �� ����

// ���� ������� �����ϴ� ����� ���� �ٸ� �� ������ �ణ�� ������ �Ӵϴ�.
// ���ڵ� ������ ������ ������ ���� �����Ǳ� ������ ����� ���α׷����� ���ڵ� ���Ϸκ��� �����͸� �а� �� ����
// ������ ������ ����մϴ�. ���� �Ʒ��� �� �Լ��� �ʿ��մϴ�.
// 1. readPage(): �־��� ������ ��ȣ�� ������ �����͸� ���α׷� ������ �о�ͼ� pagebuf�� �����Ѵ�
// 2. writePage(): ���α׷� ���� pagebuf�� �����͸� �־��� ������ ��ȣ�� �����Ѵ�
// ���ڵ� ���Ͽ��� ������ ���ڵ带 �аų� ���ο� ���ڵ带 ���ų� ���� ���ڵ带 ������ ����
// ���� readPage() �Լ��� ȣ���Ͽ� pagebuf�� ������ ��, ���⿡ �ʿ信 ���� ���ο� ���ڵ带 �����ϰų�
// ���� ���ڵ� ������ ���� ��Ÿ�����͸� �����մϴ�. �׸��� �� �� writePage() �Լ��� ȣ���Ͽ� ������ pagebuf��
// ���ڵ� ���Ͽ� �����մϴ�. �ݵ�� ������ ������ �аų� ��� �մϴ�.
//
// ����: ������ �������κ��� ���ڵ�(���� ���ڵ� ����)�� �аų� �� �� ������ ������ I/O�� ó���ؾ� ������,
// ��� ���ڵ��� ��Ÿ�����͸� �����ϰų� �����ϴ� ��� ������ ������ ó������ �ʰ� ���� ���ڵ� ������ �����ؼ� ó���Ѵ�.

typedef struct _DeletedRecord {
	char delimiter; //������ ���ڵ����� �ǹ��ϴ� delimeter mark
	int RRN; //���� �ֱٿ� ������ record��ȣ
	int RPN; //���� �ֱٿ� ������ page��ȣ
}DeletedRecord;

typedef struct _HeaderRecord {
	int total_datapage; //��ü ������ ������ ��
	int total_record; //���ڵ� ���Ͽ� �����ϴ� ��� ���ڵ� ��
	int deleted_pagenum; //������ ��ȣ (���� �ֱٿ� ������ ���ڵ� ����Ű��)
	int deleted_recordnum; //���ڵ� ��ȣ (���� �ֱٿ� ������ ���ڵ� ����Ű��)
	char dummy[PAGE_SIZE - 16]; //���̵����� ������ ���� �迭
}HeaderRecord;

//
// ������ ��ȣ�� �ش��ϴ� �������� �־��� ������ ���ۿ� �о �����Ѵ�. ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
//
void readPage(FILE *fp, char *pagebuf, int pagenum) {
	 memset(pagebuf, 0, PAGE_SIZE); //������ �����ŭ�� �����͸� 0���� �ʱ�ȭ
	 fseek(fp, PAGE_SIZE * pagenum, SEEK_SET); //��ġ �Űܼ�
	 fread(pagebuf, PAGE_SIZE, 1, fp); //������ �����ŭ �� �� �о��
}

//
// ������ ������ �����͸� �־��� ������ ��ȣ�� �ش��ϴ� ���ڵ� ������ ��ġ�� �����Ѵ�. 
// ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum) {
	fseek(fp, PAGE_SIZE * pagenum, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);
}

//
// ���ο� ���ڵ带 ������ �� �͹̳ηκ��� �Է¹��� ������ Person ����ü�� ���� �����ϰ�, pack() �Լ��� ����Ͽ�
// ���ڵ� ���Ͽ� ������ ���ڵ� ���¸� recordbuf�� �����. 
// 
void pack(char *recordbuf, const Person *p) {
	sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#", p -> id, p -> name, p -> age, p -> addr, p -> phone, p -> email);
}

// 
// �Ʒ��� unpack() �Լ��� recordbuf�� ����Ǿ� �ִ� ���ڵ带 ����ü�� ��ȯ�� �� ����Ѵ�.
//
void unpack(const char *recordbuf, Person *p) {
	char tmp_recordbuf[MAX_RECORD_SIZE]; //���ڵ� ���� �ӽ� ���� ����
	char *token; //��ū �и� ���� ������
	
	memset(tmp_recordbuf, 0, MAX_RECORD_SIZE);
	strcpy(tmp_recordbuf, recordbuf); //�ӽ� ���� ������ ����

	token = strtok(tmp_recordbuf, "#"); //������ �������� ��ū �и�
	strcpy(p -> id, token); //�ֹι�ȣ ����

	strtok(NULL, "#");
	strcpy(p -> name, token); //�̸� ����

	strtok(NULL, "#");
	strcpy(p -> age, token); //���� ����

	strtok(NULL, "#");
	strcpy(p -> addr, token); //�ּ� ����

	strtok(NULL, "#");
	strcpy(p -> phone, token); //��ȭ��ȣ ����

	strtok(NULL, "#");
	strcpy(p -> email, token); //�̸����ּ� ����
}

//
// ���ο� ���ڵ带 �����ϴ� ����� �����ϸ�, �͹̳ηκ��� �Է¹��� �ʵ尪���� ����ü�� ������ �� �Ʒ� �Լ��� ȣ���Ѵ�.
//packing�� ���⼭ �ϱ�
void add(FILE *fp, const Person *p) {
	char pagebuf[PAGE_SIZE];
	char *recordbuf = (char*)malloc(sizeof(char) * MAX_RECORD_SIZE);

	int cnt = 0; //ī���� ����
	int recordnum = 0; //���ڵ� ��ȣ
	int totalRecord = 0; //�������� ����Ǿ� �ִ� ��ü ���ڵ� ��
	int addcheck = 0;

	int offset = 0, length = 0; //������ �������� header area�� ��� offset, length
	long long sum = 0; //offset �� ������ ���� �ʿ��� ����

	HeaderRecord headerRecord; //��� ���ڵ� ����ü
	DeletedRecord deletedRecord; //���� ���ڵ� ����ü

	pack(recordbuf, p); //person����ü pack
	sum = strlen(recordbuf); //recordbuf�� length ����
	//printf("in add func. recordbuf : %s, strlen : %ld\n", recordbuf, strlen(recordbuf));
	readPage(fp, (char*)&headerRecord, 0); //readPage ȣ���� ���� ������ڵ��� ���� �о��
	//printf("headerRecord before: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);

	if (headerRecord.deleted_pagenum == -1 && headerRecord.deleted_recordnum == -1) { //������ ���ڵ尡 �������� �ʴ� ���
		for (int i = 1; i < headerRecord.total_datapage; i++) {
			totalRecord = PAGE_SIZE / strlen(recordbuf); //���ڵ� ���� ����
			recordnum = totalRecord - (((headerRecord.total_datapage - 1) * totalRecord) - headerRecord.total_record); //�����͸� �߰��� ���ڵ� ��ȣ

			readPage(fp, pagebuf, i); //������ ���� �о����

			for (int j = 0; j < PAGE_SIZE; j++) { //6���� ����ü ���� ���� ����Ǹ� # cnt ++
				if (pagebuf[j] == '#') {
					cnt++;
				}
			}

			if (totalRecord * 6 != cnt) { //�� ������ �־� �����ϴ� �������� �� �� �ִ� ���
				memcpy(strlen(recordbuf) * recordnum + pagebuf, recordbuf, strlen(recordbuf)); //�������� ���ڵ���� ���� ����
				
				writePage(fp, pagebuf, i); //�ش� �ε�����°���� �������������� ��
				headerRecord.total_record += 1;
				addcheck = 1;
				//printf("headerRecord after case 1: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);	
				break;
			}
		}

		if(addcheck == 0) { //�� ������ ���� ���Ӱ� �������� �߰��ؾ� �ϴ� ���
			memset(pagebuf, 0, PAGE_SIZE);
			memcpy(pagebuf, recordbuf, strlen(recordbuf));
			writePage(fp, pagebuf, headerRecord.total_datapage); //�ش� ������ ��������°�� ������ ���� ���� ��
			//printf("headerRecord after case 2: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);	
			headerRecord.total_datapage += 1;
			headerRecord.total_record += 1; //�� ������ ���� ���ڵ� �� +1
		}
	}

	else { //������ ���ڵ� ������ �ִ� ���
		readPage(fp, pagebuf, headerRecord.deleted_pagenum); //������ ������ ��ȣ �о��
		memcpy(&deletedRecord, (headerRecord.deleted_recordnum * strlen(recordbuf)) + pagebuf, sizeof(DeletedRecord));
		memcpy((headerRecord.deleted_recordnum * strlen(recordbuf)) + pagebuf, recordbuf, strlen(recordbuf));
		writePage(fp, pagebuf, headerRecord.deleted_pagenum);
		//printf("headerRecord after case 3: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);	
		headerRecord.deleted_pagenum = deletedRecord.RPN;
		headerRecord.deleted_recordnum = deletedRecord.RRN; //������ ���ڵ� ����ü RRN , RPN�� ����
	}

	writePage(fp, (char*)&headerRecord, 0); //��� ���ڵ� ���� ��
	//printf("headerRecord final: %d %d %d %d\n", headerRecord.total_datapage, headerRecord.total_record, headerRecord.deleted_pagenum, headerRecord.deleted_recordnum);	
}

//
// �ֹι�ȣ�� ��ġ�ϴ� ���ڵ带 ã�Ƽ� �����ϴ� ����� �����Ѵ�.
//
void delete(FILE *fp, const char *id) {
	HeaderRecord headerRecord;
	Person p;
	DeletedRecord deletedRecord;

	char pagebuf[PAGE_SIZE];
	char *recordbuf = (char *)malloc(sizeof(char) * MAX_RECORD_SIZE);

	int recordnum = 0, totalRecord = 0;
	int deletecheck = 0;

	readPage(fp, (char *)&headerRecord, 0); //������ڵ��� ������ �о��

	for (int i = 1; i < headerRecord.total_datapage; i++) {
		totalRecord = PAGE_SIZE / MAX_RECORD_SIZE; //�� ���ڵ� �� ���ϱ�
		recordnum = totalRecord - ((totalRecord * (headerRecord.total_datapage - 1)) - headerRecord.total_record);
		readPage(fp, pagebuf, i); //�ش� �ε�����° �������� ���ڵ� �ֱ�

		for (int j = 0; j < totalRecord; j++) {
			memcpy(recordbuf, (j * MAX_RECORD_SIZE) + pagebuf, MAX_RECORD_SIZE);
			unpack(recordbuf, &p);

			if (!strcmp(p.id, id)) {
				deletedRecord.RRN = headerRecord.deleted_recordnum;
				deletedRecord.RPN = headerRecord.deleted_pagenum;
				deletedRecord.delimiter = '*'; //RRN, RPN, ������ ����

				memcpy((j * MAX_RECORD_SIZE) + pagebuf, &deletedRecord, sizeof(DeletedRecord)); //���� ���ڵ忡 ����
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
	FILE *fp;  // ���ڵ� ������ ���� ������
	HeaderRecord headerRecord; //��� ���ڵ� ���� ����ü
	Person p; //person����ü
	
	if (access(argv[2], F_OK) != 0) { //������ �������� �ʴ� ���
		if ((fp = fopen(argv[2], "w+")) == NULL) { //���� ���� ����
			fprintf(stderr, "%s fopen() w+ mode error\n", argv[2]);
			exit(1);
		}

		//��� ���ڵ� �ʱ�ȭ
		headerRecord.total_datapage = 1;
		headerRecord.total_record = 0;
		headerRecord.deleted_pagenum = -1;
		headerRecord.deleted_recordnum = -1;
		memset(headerRecord.dummy, 0, PAGE_SIZE - 16);

		writePage(fp, (char *)&headerRecord, 0);

		fclose(fp); //binary integer ���� ��� ���ڵ� ���� ������ �ݱ�
	}

	if ((fp = fopen(argv[2], "r+")) == NULL) {
		fprintf(stderr, "%s fopen() r+ mode error(second open in main function\n", argv[2]);
		exit(1);
	}

	if (strcmp(argv[1], "a") == 0) { //a�ɼ��� �־� ���ڵ� �߰�
		strcpy(p.id, argv[3]);
		strcpy(p.name, argv[4]);
		strcpy(p.age, argv[5]);
		strcpy(p.addr, argv[6]);
		strcpy(p.phone, argv[7]);
		strcpy(p.email, argv[8]);

		add(fp, &p);
	}

	else if (strcmp(argv[1], "d") == 0) { //d�ɼ��� �־� ���ڵ� ����
		delete(fp, argv[3]);
	}


	else if ((strcmp(argv[1], "a") != 0) || strcmp(argv[1], "d") != 0) {
		printf("do not support this option\n");
	}
	
	fclose(fp);
	
	return 1;
}
