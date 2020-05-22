#define _CTR_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>
#include "BaseDefine.h"
#include "DB.h"
#include "DBPrintModule.h"
#include <ctype.h>

#pragma foreign_keys = 1 // 참조키 활성화

static int callback(void *NotUsed, int argc, char **argv, char **azColName) { // callback
    int i;
    for(i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i]:"NULL");
    }
    printf("\n");

    return 0;
}

void removeEnter() { // scanf()의 Enter key 제거
    char tmp;

    while((tmp = getchar()) != '\n') {
        putchar(tmp);
    }
}

char *dateNow(struct tm *t) { // date 가져오는 함수
    static char now[DATElen] = { 0, };

    sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900,
                t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    return now;
}

void printBOF_gets(char str[], int strsize, int define_size) { // gets() 버퍼오버플로우 방지 : 재입력 요구 메시지 출력
    printf("입력한 byte(s)는 %dbyte(s)입니다.\n", strsize-1);
    printf("입력 가능한 최대 길이 %dbyte(s)보다 %dbyes(s)가 초과됐습니다.\n", define_size-1, strsize-define_size);
    printf("입력할 데이터를 다시 확인해보세요.\n");
}

int checkDate(int date) { // 입력받은 날짜의 유효성 검사-> 1이면 유효한 날짜 (0이면 없는 날짜임)
    int yyyy, mm, dd = 0;

	yyyy = date / 10000;
    mm = (date-(yyyy*10000)) / 100;
	dd = date % 100 ;
	printf("\n%d년 %d월 %d일\n", yyyy, mm, dd);

	if(yyyy <= 1000)
		return 0;
	if(mm == 0 || mm >= 13)
		return 0;
	if(dd == 0 || dd >= 32)
		return 0;

	if (mm == 2) { // 2월달의 윤년(29일), 평년(28일) 구분
        if(yyyy%400 == 0) { // 400으로 나눠지면 윤년
            if(dd <= 0 || dd >30)
            	return 0;
		}
		else if((yyyy%4 == 0) && (yyyy%100 != 0)) { // 4와 100으로 나눠지면 윤년
            if(dd <= 0 || dd >30)
            	return 0;
		}
		else { // 그 외에는 전부 평년
			if(dd <=0 || dd > 28)
					return 0;
		}
	}
	else if (mm == 4 || mm == 6 || mm == 9 || mm == 11) { // 4, 6, 9, 11월 -> 30일까지 있음
        if (dd <= 0 || dd > 30)
			return 0;
	}
	else { // 1, 3, 5, 7, 8, 10, 12월 -> 31일까지 있음
		if (dd <= 0 || dd > 31)
			return 0;
	}
	return 1;
}

int hintIDPWD() { // id, pwd 분실시 지원 기능을 위한 관리자 정보를 호출하는 함수 // 아직 작성 못함.

}

int inADMIN() { // case 16 ok
    sqlite3 *db;
    char *errmsg;
    int rc;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };
    char pwd[PWDlen] = { 0, };
    char access[ACCESSlen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    puts("\nADMIN TABLE's insert\n");

    while(1) {
        printf("ID는 등록 후에 수정이 불가능합니다!!!\n");
        puts("input id (9bytes 보다 길면 다시 입력함):");
        gets(str);
        strsize = strlen(str)+1;
        if(strsize <= IDlen)
            break;
        printBOF_gets(str, strsize, IDlen);
    }
    strncpy(id, str, IDlen-1);

    while(1) {
        printf("ACCESS는 수정이 가능합니다.\n");
        puts("input access (1byte 보다 길면 다시 입력함):");
        gets(str);
        strsize = strlen(str)+1;
        if(strsize <= ACCESSlen)
            break;
        printBOF_gets(str, strsize, ACCESSlen);
    }
    strncpy(access, str, ACCESSlen-1);

    while(1) {
        printf("PWD는 수정이 가능합니다.\n");
        puts("input pwd (513bytes 보다 길면 다시 입력함):");
        gets(str);
        strsize = strlen(str)+1;
        if(strsize <= PWDlen)
            break;
        printBOF_gets(str, strsize, PWDlen);
    }
    strncpy(pwd, str, PWDlen-1);

    __fpurge(stdin);
    strncpy(input_sql, "insert into ADMIN(id, access, pwd) values('", 43);
    strncat(input_sql, id, IDlen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, access, ACCESSlen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, pwd, PWDlen-1);
    strncat(input_sql, "');", 3);
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't input : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "admin table input successfully\n");
    }

    sqlite3_close(db);

    return 0;
}

int inMAC() { // case 17 // 통신과 연결해야해서 작성 못함

}

int inINFO() { // case 18
    sqlite3 *db;
    char *errmsg;
    int rc;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };
    char name[NAMElen] = { 0, };
    char birth[BIRTHlen] = { 0, };
    char email[EMAILlen] = { 0, };
    char phone[PHONElen] = { 0, };
    char date[DATElen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    int b_date = 0; // birth 문자열을 정수형으로 받을 변수

    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    printf("\n지금 입력하는 정보는 id, pwd 분실 시 확인 정보로 사용됩니다!!!\n");
    printf("등록하려면 입력 후 EnterKey,\n");
    printf("등록을 건너띄려면 바로 EnterKey를 누르세요.\n");

    puts("\nINFO TABLE's insert\n");

    while(1) {
        puts("input id (9bytes 보다 길면 다시 입력함):");
        gets(str);
        strsize = strlen(str)+1;
        if(strsize <= IDlen)
            break;
        printBOF_gets(str, strsize, IDlen);
    }
    strncpy(id, str, IDlen-1);

    while(1) { // name 입력받음
        printf("\n지금 입력하는 정보는 id, pwd 분실 시 확인 정보로 사용됩니다!!!\n");
        printf("NAME은 수정이 가능합니다.\n");
        printf("등록을 건너띄려면 바로 EnterKey를 누르세요.\n");
        puts("input name (30bytes 보다 길면 다시 입력함):");
        gets(str);
        strsize = strlen(str)+1;
        if(strsize <= NAMElen)
            break;
        printBOF_gets(str, strsize, NAMElen);
    }
    strncpy(name, str, NAMElen-1);

    while(1) { // 생년월일은 yyyymmdd 8개의 유효 날짜로 입력받음
        printf("\n지금 입력하는 정보는 id, pwd 분실 시 확인 정보로 사용됩니다!!!\n");
        printf("BIRTH는 수정이 가능합니다.\n");
        printf("등록을 건너띄려면 바로 EnterKey를 누르세요.\n");
        puts("input birth (yyyymmdd 8개 유효 날짜(숫자)가 아니면 다시 입력함):");
        gets(str);
        strsize = strlen(str)+1;
        if(strsize == BIRTHlen) {
            b_date = atoi(str);
            if(checkDate(b_date) == 1) // 1이면 참(유효날짜)
                 break;
            else {
                printf("유효 날짜가 아닙니다!!!\n");
                continue;
            }
        }
        else
            printf("yyyymmdd 8개의 유효 날짜(숫자)로 입력해야 합니다!\n");
    }
    strncpy(birth, str, BIRTHlen-1);

    puts("input email(50글자까지 입력 가능):");
    fgets(email, EMAILlen, stdin);
    strsize = strlen(email)+1;
    if(strsize > EMAILlen)


    puts("input phone(20글자까지 입력 가능):");
    fgets(phone, PHONElen, stdin);
    strsize = strlen(phone)+1;
    if(strsize > PHONElen)


    puts("EnterKey를 누르세요.");
    printf("%s\n", str_now);
    strncpy(date, str_now, DATElen);

    __fpurge(stdin);
    strncpy(input_sql, "insert into INFO(id, name, birth, email, phone, date) values('", 62);
    strncat(input_sql, id, IDlen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, name, NAMElen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, birth, BIRTHlen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, email, EMAILlen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, phone, PHONElen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, date, DATElen-1);
    strncat(input_sql, "');", 3);
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't input : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "info table input successfully\n");
    }
    sqlite3_close(db);

    return 0;
}

int inWL() { // case 19
    sqlite3 *db;
   	char *errmsg;
    int rc;
    char input_sql[SQLlen] = { 0, };
    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    char date[DATElen] = { 0, };
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    puts("WHITELIST TABLE's insert\n");
    puts("input whitelist(30글자까지 입력 가능):");
    fgets(whitelist, WLlen, stdin);
    strsize = strlen(whitelist)+1;
    if(strsize > WLlen)
        //secuBOF_gets(whitelist, strsize, WLlen);

    puts("input id(9글자까지 입력 가능):");
    fgets(id, IDlen, stdin);
    strsize = strlen(id)+1;
    if(strsize > IDlen)
        //secuBOF_gets(id, strsize, IDlen);

    puts("EnterKey를 누르세요.");
    printf("%s\n", str_now);
    strncpy(date, str_now, DATElen);

    __fpurge(stdin);
    strncpy(input_sql, "insert into WHITELIST(whitelist, id, date) values('", 51);
    strncat(input_sql, whitelist, WLlen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, id, IDlen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, date, DATElen-1);
    strncat(input_sql, "');", 3);
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't input : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print input successfully\n");
    }

    sqlite3_close(db);

    return 0;
}

int upADMIN() { // case 26
    sqlite3 *db;
    char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char *sql;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };
    char access[ACCESSlen] = { 0, };
    int menu; // 수정 항목을 선택하는 변수
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    char** result; // get table result
    int row, col; // get table row,column

    char src_pwd[PWDlen] = { 0, }; // 수정할 PWD -> 다른 PWD로 변경됨.
    char pwd[PWDlen] = { 0, };

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    res = "Callback Function Called";

    printf("ADMIN TABLE's Update\n");

    puts("수정할 데이터의 id(기본키) 입력:");
    fgets(id, IDlen, stdin);
    strsize = strlen(id)+1;
    if(strsize > IDlen)
        //secuBOF_gets(id, strsize, IDlen);

    puts("수정할 id의 PWD 입력(PWD 틀리면 종료):");
    fgets(src_pwd, PWDlen, stdin);
    strsize = strlen(pwd)+1;
    if(strsize > PWDlen)
        //secuBOF_gets(pwd, strsize, PWDlen);

    fflush(stdin);
    strncpy(input_sql, "SELECT id, access FROM ADMIN WHERE pwd = '", 42);
    strncat(input_sql, src_pwd, PWDlen-1);
    strncat(input_sql, "' AND id = '", 12);
    strncat(input_sql, id, IDlen-1);
    strncat(input_sql, "';", 2);
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print Admin Table : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print Admin Table successfully\n");
    }

    rc = sqlite3_get_table(db, input_sql, &result, &row, &col, &errmsg);
    //printf("%s\n", result[0]);  // id 와 pwd 를 원래 정보와 동일하게 입력하면 id 라고 화면에 출력됨.

    if(!strcmp(result[0], "id")) {
        printf("1. ACCESS(권한)만 수정\n");
        printf("2. PWD(비밀번호)만 수정\n");
        printf("3. ACCESS(권한)과 PWD(비밀번호) 모두 수정\n");
        puts("input number:");
        scanf("%d", &menu);
        removeEnter();
        switch(menu) {
            case 1:
                printf("수정할 ACCESS(권한)값 입력:\n");
                fgets(access, ACCESSlen, stdin);
                strsize = strlen(access)+1;
                if(strsize > ACCESSlen)
                //secuBOF_gets(access, strsize, ACCESSlen);

                fflush(stdin);
                strncpy(input_sql, "UPDATE ADMIN SET access = '", 27);
                strncat(input_sql, access, ACCESSlen-1);
                strncat(input_sql, "' WHERE id = '", 14);
                strncat(input_sql, id, IDlen-1);
                strncat(input_sql, "';", 2);
                printf("%s\n", input_sql);

                rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Access Update : %s\n", sqlite3_errmsg(db));
                    return 1;
                }
                else {
                    fprintf(stderr, "Access update successfully\n");
                }
                break;

            case 2:
                puts("수정할 PWD(비밀번호)값 입력:");
                fgets(src_pwd, PWDlen, stdin);
                strsize = strlen(pwd)+1;
                if(strsize > PWDlen)
                    //secuBOF_gets(pwd, strsize, PWDlen);

                fflush(stdin);
                strncpy(input_sql, "UPDATE ADMIN SET pwd = '", 24);
                strncat(input_sql, pwd, PWDlen-1);
                strncat(input_sql, "' WHERE id = '", 14);
                strncat(input_sql, id, IDlen-1);
                strncat(input_sql, "';", 2);
                printf("%s\n", input_sql);

                rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't PWD Update : %s\n", sqlite3_errmsg(db));
                    return 1;
                }
                else {
                    fprintf(stderr, "PWD update successfully\n");
                }
                break;

            case 3:
                printf("수정할 ACCESS(권한)값 입력:\n");
                fgets(access, ACCESSlen, stdin);
                strsize = strlen(access)+1;
                if(strsize > ACCESSlen)
                //secuBOF_gets(access, strsize, ACCESSlen);

                puts("수정할 PWD(비밀번호)값 입력:");
                fgets(src_pwd, PWDlen, stdin);
                strsize = strlen(pwd)+1;
                if(strsize > PWDlen)
                    //secuBOF_gets(pwd, strsize, PWDlen);;

                fflush(stdin);
                strncpy(input_sql, "UPDATE ADMIN SET (access, pwd) = ('", 35);
                strncat(input_sql, access, ACCESSlen-1);
                strncat(input_sql, "', '", 4);
                strncat(input_sql, pwd, PWDlen-1);
                strncat(input_sql, "') WHERE id = '", 15);
                strncat(input_sql, id, IDlen-1);
                strncat(input_sql, "';", 2);
                printf("%s\n", input_sql);

                rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Access & PWD Update : %s\n", sqlite3_errmsg(db));
                    return 1;
                }
                else {
                    fprintf(stderr, "Access & PWD update successfully\n");
                }
                break;

            default:
                break;
        }
    }
    sqlite3_free_table(result);

    sqlite3_close(db);

    return 0;
}

int upMAC() { // case 27

}

int upINFO() { // case 28
    sqlite3 *db;
   	char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char *sql;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };
    char name[NAMElen] = { 0, };
    char birth[BIRTHlen] = { 0, };
    char email[EMAILlen] = { 0, };
    char phone[PHONElen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    int menu; // 수정 항목을 선택하는 변수
    char tmp; //엔터키 삭제 변수

    char** result; // get table result
    int i, j, row, col; // get table row,column
    char metatype[256]; // get table loop

    char date[DATElen] = { 0, };
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    res = "Callback Function Called";

    printf("INFO TABLE's Update\n");
    puts("수정할 데이터의 id(기본키) 입력:");
    fgets(id, IDlen, stdin);

    __fpurge(stdin);
    strncpy(input_sql, "SELECT * FROM INFO WHERE id = '", SQLlen-1);
    strncat(input_sql, id, SQLlen-1);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);
    printf("\nID's data :\n");
    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print INFO Table : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print INFO Table successfully\n");
    }

    rc = sqlite3_get_table(db, input_sql, &result, &row, &col, &errmsg);
    //printf("%s\n", result[0]);  // id 를 정확히 입력하면 id 라고 화면에 출력됨.
    /* for(i = 0; i <= row; i++) {
        for(j = 0; j <= col; j++) {
            printf("%s", result[i * col +j]);
            printf("\t");
        }
        printf("\n");
    } */

    //strcpy(metatype, result[(i * col)]);
    //printf("%s", metatype);

    if(!strcmp(result[0], "id")) {
        void Itmenu() { // INFO table menu
            printf("1. NAME\n");
            printf("2. BIRTH\n");
            printf("3. EMAIL\n");
            printf("4. PHONE\n");
            printf("0. EXIT\n");
        }
        puts("input number:");
        scanf("%d", &menu);
        removeEnter();

        switch(menu) {
            case 1:
                puts("name 수정:");
                fgets(name, NAMElen, stdin);
                printf("%s\n", str_now);
                strcpy(date, str_now);

                __fpurge(stdin);
                strncpy(input_sql, "UPDATE INFO SET (name, date) = ('", SQLlen-1);
                strncat(input_sql, name, SQLlen-1);
                strncat(input_sql, "','", SQLlen-1);
                strncat(input_sql, date, SQLlen-1);
                strncat(input_sql, "') WHERE id = '", SQLlen-1);
                strncat(input_sql, id, SQLlen-1);
                strcat(input_sql, "';");
                //printf("%s\n", input_sql);

                rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Name Update : %s\n", sqlite3_errmsg(db));
                    return 1;
                }
                else {
                    fprintf(stderr, "Name update successfully\n");
                }
                break;

            case 2:
                puts("birth 수정:");
                fgets(birth, BIRTHlen, stdin);
                printf("%s\n", str_now);
                strncpy(date, str_now, DATElen);

                __fpurge(stdin);
                strncpy(input_sql, "UPDATE INFO SET (birth, date) = ('", SQLlen-1);
                strncat(input_sql, birth, SQLlen-1);
                strncat(input_sql, "','", SQLlen-1);
                strncat(input_sql, date, SQLlen-1);
                strncat(input_sql, "') WHERE id = '", SQLlen-1);
                strncat(input_sql, id, SQLlen-1);
                strcat(input_sql, "';");
                printf("%s\n", input_sql);

                rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Birth Update : %s\n", sqlite3_errmsg(db));
                    return 1;
                }
                else {
                    fprintf(stderr, "Birth update successfully\n");
                }
                break;

            case 3:
                puts("email 수정:");
                fgets(email, EMAILlen, stdin);
                printf("%s\n", str_now);
                strncpy(date, str_now, DATElen);

                __fpurge(stdin);
                strncpy(input_sql, "UPDATE INFO SET (email, date) = ('", SQLlen-1);
                strncat(input_sql, name, SQLlen-1);
                strncat(input_sql, "','", SQLlen-1);
                strncat(input_sql, date, SQLlen-1);
                strncat(input_sql, "') WHERE id = '", SQLlen-1);
                strncat(input_sql, id, SQLlen-1);
                strcat(input_sql, "';");
                printf("%s\n", input_sql);

                rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Email Update : %s\n", sqlite3_errmsg(db));
                    return 1;
                }
                else {
                    fprintf(stderr, "Email update successfully\n");
                }
                break;

            case 4:
                puts("phone 수정:");
                fgets(phone, PHONElen, stdin);
                printf("%s\n", str_now);
                strncpy(date, str_now, DATElen);

                __fpurge(stdin);
                strncpy(input_sql, "UPDATE INFO SET (phone, date) = ('", SQLlen-1);
                strncat(input_sql, phone, SQLlen-1);
                strncat(input_sql, "','", SQLlen-1);
                strncat(input_sql, date, SQLlen-1);
                strncat(input_sql, "') WHERE id = '", SQLlen-1);
                strncat(input_sql, id, SQLlen-1);
                strcat(input_sql, "';");
                printf("%s\n", input_sql);

                rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Phone Update : %s\n", sqlite3_errmsg(db));
                    return 1;
                }
                else {
                    fprintf(stderr, "Name Phone successfully\n");
                }
                break;

            default:
                break;
        }
    }
    sqlite3_free_table(result);

    sqlite3_close(db);

    return 0;
}

int upWL() { // case 29
    sqlite3 *db;
   	char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char *sql;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    char src_white[WLlen] = { 0, }; // 수정할 화이트리스트 ->다른 화이트리스트로 변경됨.
    char whitelist[WLlen] = { 0, };

    char date[DATElen] = { 0, };
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    res = "Callback Function Called";

    puts("WHITELIST TABLE's Update\n");
    puts("update source whitelist:");
    gets(src_white);
    puts("update whitelist:");
    gets(whitelist);

    puts("date Enter:");
    printf("%s\n", str_now);
    strncpy(date, str_now, DATElen);

    __fpurge(stdin);
    strncpy(input_sql, "UPDATE WHITELIST SET (date, whitelist) = ('", SQLlen-1);
    strncat(input_sql, date, SQLlen-1);
    strncat(input_sql, "', '", SQLlen-1);
    strncat(input_sql, whitelist, SQLlen-1);
    strncat(input_sql, "') ", SQLlen-1);
    strncat(input_sql, "where whitelist = '", SQLlen-1);
    strncat(input_sql, src_white, SQLlen-1);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);

    rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't whitelist Update : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Whitelist update successfully\n");
    }

    sqlite3_close(db);

    return 0;
}

int delADMIN() { // case 36
    sqlite3 *db;
    char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char input_sql[SQLlen-1] = { 0, };
    char id[IDlen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    char** result; // get table result
    int row, col; // get table row,column

    int access;

    char del_pwd[PWDlen] = { 0, }; // 삭제되는 PWD
    char pwd[PWDlen] = { 0, };

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    res = "Callback Function Called";

    printf("ADMIN TABLE's Update\n");

    puts("삭제하는 데이터의 id(기본키) 입력:");
    fgets(id, IDlen, stdin);
    puts("삭제하는 id의 PWD 입력(PWD 틀리면 종료):");
    fgets(del_pwd, PWDlen, stdin);

    __fpurge(stdin);
    strncpy(input_sql, "SELECT id, access FROM ADMIN WHERE pwd = '", SQLlen-1);
    strncat(input_sql, del_pwd, SQLlen-1);
    strncat(input_sql, "' AND id = '", SQLlen-1);
    strncat(input_sql, id, SQLlen-1);
    strcat(input_sql, "';");

    //printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print Admin Table : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print Admin Table successfully\n");
    }

    rc = sqlite3_get_table(db, input_sql, &result, &row, &col, &errmsg);
    //printf("%s\n", result[0]);  // id 와 pwd 를 정확히 입력하면 id 라고 화면에 출력됨.
    if(!strcmp(result[0], "id")) {
        __fpurge(stdin);
        strncpy(input_sql, "DELETE from ADMIN where id = '", SQLlen-1);
        strncat(input_sql, id, SQLlen-1);
        strcat(input_sql, "';");
        //printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
        if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Delete successfully\n");
        }
    }
    sqlite3_free_table(result);

    sqlite3_close(db);

    return 0;
}

int delMAC() { // case 37

}

int delINFO() { // case 38
    sqlite3 *db;
    char *errmsg;
    int rc;
    sqlite3_stmt *res;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };
    char name[NAMElen] = { 0, };
    char birth[BIRTHlen] = { 0, };
    char email[EMAILlen] = { 0, };
    char phone[PHONElen] = { 0, };
    char date[DATElen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    puts("삭제하는 데이터의 id(기본키) 입력:");
    fgets(id, IDlen, stdin);

    __fpurge(stdin);;
    strncpy(input_sql, "SELECT * FROM INFO WHERE id = '", SQLlen-1);
    strncat(input_sql, id, SQLlen-1);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print Info Table : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print Info Table successfully\n");
    }

    __fpurge(stdin);
    strncpy(input_sql, "DELETE from INFO where id = '", SQLlen-1);
    strncat(input_sql, id, SQLlen-1);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
    if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't delete : %s\n", sqlite3_errmsg(db));
            return 1;
    }
    else {
        fprintf(stderr, "Delete successfully\n");
    }
    sqlite3_close(db);

    return 0;
}

int delWL() { // case 39
    sqlite3 *db;
   	char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char input_sql[SQLlen] = { 0, };
    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    res = "Callback Function Called";

    puts("WHITELIST TABLE's records delete.\n");
    puts("input whitelist:");
    fgets(whitelist, WLlen, stdin);
    puts("input id:");
    fgets(id, IDlen, stdin);

    __fpurge(stdin);
    strncpy(input_sql, "DELETE from WHITELIST where whitelist = '", SQLlen-1);
    strncat(input_sql, whitelist, SQLlen-1);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't delete : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Delete successfully\n");
    }

    sqlite3_close(db);

    return 0;
}

int selADMIN() { // case 46
    sqlite3 *db;
    char *errmsg;
    int rc;
    char input_sql[SQLlen] = { 0, };
   	char *sql; // table schema sql
    char id[IDlen] = { 0, };
    char pwd[PWDlen] = { 0, };
    char cmp_pwd[PWDlen]; // 원본 pwd값과 비교 할 변수
    int access;
    char tmp; //엔터키 삭제 변수
    char buf_access[2]; // int형의 access값을 문자로 받을 변수
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    int menu; // search menu

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    //ADMIN Table 출력
    sql = "select id, access from ADMIN";
    rc = sqlite3_exec(db, sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print schema : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print schema successfully\n");
    }

    int sel_at_id() { // case 1
        puts("search id:");
        fgets(id, IDlen, stdin);

        __fpurge(stdin);
        strncpy(input_sql, "SELECT id, access FROM ADMIN WHERE id = '", SQLlen-1);
        strncat(input_sql, id, SQLlen-1);
        strcat(input_sql, "';");
        //printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    int sel_at_access() { // case 2
        puts("search access:");
        scanf("%d", &access);
        while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
            putchar(tmp);
        }
        sprintf(buf_access, "%d", access); // access를 문자로 변환

        __fpurge(stdin);
        strncpy(input_sql, "SELECT id, access FROM ADMIN WHERE access = ", SQLlen-1);
        strncat(input_sql, buf_access, SQLlen-1);
        strcat(input_sql, ";");
        //printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    int sel_at_pwd() { // case 3 // int searchPWD(const char search_id[IDlen], const char seearch_pwd[PWDlen]) pwd 인증
        puts("seach pwd's ID:");
        fgets(id, IDlen, stdin);
        puts("search pwd:");
        fgets(cmp_pwd, PWDlen, stdin);

        __fpurge(stdin);
        strncpy(input_sql, "SELECT id, access FROM ADMIN WHERE id = '", SQLlen-1);
        strncat(input_sql, id, SQLlen-1);
        strncat(input_sql, "' AND pwd = '", SQLlen-1);
        strncat(input_sql, cmp_pwd, SQLlen-1);
        strcat(input_sql, "';");
        //printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    void Itmenu() { // ADMIN table menu
        printf("1. ID\n");
        printf("2. ACCESS\n");
        printf("3. PWD\n");
        printf("0. EXIT\n");
        printf("검색하려는 항목의 번호를 고르세요:");
        scanf("%d", &menu);
        while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
            putchar(tmp);
        }

        switch(menu) {
            case 1:
                sel_at_id();
                break;

            case 2:
                sel_at_access();
                break;

            case 3:
                sel_at_pwd();
                break;

            default:
                break;
        }
    }
    Itmenu();

    sqlite3_close(db);

    return 0;
}

int selMAC() { // case 47

}

int selINFO() { // case 48
    sqlite3 *db;
    char *errmsg;
    int rc;
    char input_sql[SQLlen] = { 0, };
   	char *sql; // table schema sql
    char id[IDlen] = { 0, };
    char name[NAMElen] = { 0, };
    char birth[BIRTHlen] = { 0, };
    char email[EMAILlen] = { 0, };
    char phone[PHONElen] = { 0, };
    char date[DATElen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    int menu; // search menu
    char tmp; // Enter Key remove

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    //INFO Table 출력
    sql = "select * from INFO";
    rc = sqlite3_exec(db, sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print schema : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print schema successfully\n");
    }

    int sel_it_id() { // case 1
        puts("search id:");
        fgets(id, IDlen, stdin);

        __fpurge(stdin);
        strncpy(input_sql, "SELECT * FROM INFO WHERE id = '", SQLlen-1);
        strncat(input_sql, id, SQLlen-1);
        strcat(input_sql, "';");
        //printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    int sel_it_name() { // case 2
        puts("search name:");
        fgets(name, NAMElen, stdin);

        __fpurge(stdin);
        strncpy(input_sql, "SELECT * FROM INFO WHERE name like '%", SQLlen-1);
        strncat(input_sql, name, SQLlen-1);
        strcat(input_sql, "%';");
        printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    int sel_it_birth() { // case 3
        puts("search birth:");
        fgets(birth, BIRTHlen, stdin);

        __fpurge(stdin);
        strncpy(input_sql, "SELECT * FROM INFO WHERE birth like '%", SQLlen-1);
        strncat(input_sql, birth, SQLlen-1);
        strcat(input_sql, "%';");
        printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    int sel_it_email() { // case 4
        puts("search email:");
        fgets(email, EMAILlen, stdin);

        __fpurge(stdin);
        strncpy(input_sql, "SELECT * FROM INFO WHERE email like '%", SQLlen-1);
        strncat(input_sql, email, SQLlen-1);
        strcat(input_sql, "%';");
        printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    int sel_it_phone() { // case 5
        puts("search phone:");
        fgets(phone, PHONElen, stdin);

        __fpurge(stdin);
        strncpy(input_sql, "SELECT * FROM INFO WHERE phone like '%", SQLlen-1);
        strncat(input_sql, phone, SQLlen-1);
        strcat(input_sql, "%';");
        printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    void Itmenu() { // INFO table menu
        printf("1. ID\n");
        printf("2. NAME\n");
        printf("3. BIRTH\n");
        printf("4. EMAIL\n");
        printf("5. PHONE\n");
        printf("0. EXIT\n");
        printf("검색하려는 항목의 번호를 고르세요:");
        scanf("%d", &menu);
        while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
            putchar(tmp);
        }

        switch(menu) {
            case 1:
                sel_it_id();
                break;

            case 2:
                sel_it_name();
                break;

            case 3:
                sel_it_birth();
                break;

            case 4:
                sel_it_email();
                break;

            case 5:
                sel_it_phone();
                break;

            default:
                break;
        }
    }
    Itmenu();

    sqlite3_close(db);

    return 0;
}

int selWL() { // case 49
    sqlite3 *db;
   	char *errmsg;
   	char *sql; // table schema sql
    int rc;
    char input_sql[SQLlen] = { 0, };
    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    int menu; // search menu
    char tmp; // Enter Key remove

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    // WHITELIST Table 출력
    sql = "select * from WHITELIST";
    rc = sqlite3_exec(db, sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print schema : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print schema successfully\n");
    }

    int sel_wl_wl() { // case 1
        puts("search whitelist:");
        fgets(whitelist, WLlen, stdin);

        __fpurge(stdin);
        strncpy(input_sql, "SELECT * FROM WHITELIST WHERE WHITELIST like '%", SQLlen-1);
        strncat(input_sql, whitelist, SQLlen-1);
        strcat(input_sql, "%';");
        printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    int sel_wl_id() { // case 2
        puts("search id:");
        fgets(id, IDlen, stdin);

        __fpurge(stdin);
        strncpy(input_sql, "SELECT * FROM WHITELIST WHERE ID = '", SQLlen-1);
        strncat(input_sql, id, SQLlen-1);
        strcat(input_sql, "';");
        //printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    void Itmenu() { // WHITELIST table menu
        printf("1. WHITELIST\n");
        printf("2. ID\n");
        printf("0. EXIT\n");
        printf("검색하려는 항목의 번호를 고르세요:\n");
        scanf("%d", &menu);
        while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
            putchar(tmp);
        }
        switch(menu) {
            case 1:
                sel_wl_wl();
                break;

            case 2:
                sel_wl_id();
                break;

            default:
                break;
        }
    }
    Itmenu();

    sqlite3_close(db);

    return 0;
}