#include <stdio.h>
#include "sqlite3.h"
#include <string.h>
#include "BaseDefine.h"
#include "DB.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName) { // callback
    int i;
    for(i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i]:"NULL");
    }
    printf("\n");

    return 0;
}

char *dateNow(struct tm *t) { // date 가져오는 함수
    static char now[DATElen] = { 0, };

    sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900,
                t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    return now;
}

int openDB() { // CPS.db open
    sqlite3 *db;
   	char *errmsg;
    int rc;
    //char *sql_fk; // Foreign Key 활성화 변수

    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
    	fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정
/*
    sql_fk = "PRAGMA foreign_keys = 1"; // Foreign Key 활성화(1) 시킴
    rc = sqlite3_exec(db, sql_fk, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "참조키 활성화 에러 : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
    else
        return 0; */
}

int selWhitelist(const char sel_wl[WLlen], OUT struct WhiteListTable sel_wt) { // case 46
    //char whitelist[WLlen];
    OUT struct WhiteListTable wt;

    sqlite3 *db;
   	char *errmsg;
   	char *sql; // table schema sql
    int rc;
    char input_sql[SQLlen] = { 0, };
    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };

    int menu; // search menu
    char tmp; // Enter Key remove

    openDB();

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

        fflush(stdin);
        strncpy(input_sql, "SELECT * FROM WHITELIST WHERE WHITELIST like '%", SQLlen-1);
        strncat(input_sql, whitelist, SQLlen-1);
        strcat(input_sql, "%';");
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

    int sel_wl_id() { // case 2
        puts("search id:");
        fgets(id, IDlen, stdin);

        fflush(stdin);
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

int selAdminInfo(const char sel_id[IDlen], const char sel_pwd[PWDlen], OUT struct InfoTable sel_it) { // case 48 & 49 참고
    char id[IDlen];
    char pwd[PWDlen];
    OUT struct InfoTable it;
}

int selPublicKey(const char sel_id[IDlen], const char sel_pwd[PWDlen], OUT char sel_pk[PKlen]) {
    char id[IDlen];
    char pwd[PWDlen];
    OUT char public_key[PKlen]; // OUT publickey를 반환함
}

int searchPWD(const char search_id[IDlen], const char seearch_pwd[PWDlen]) { // case 48 참고 // pwd 인증:pwd를 알고 있는 경우
    char id[IDlen];
    char pwd[PWDlen];
}

int delAdmin(const char del_id[IDlen], const char del_pwd[PWDlen]) { // case 38 & 39 참고
    char id[IDlen];
    char pwd[PWDlen];
}

int delWhiteList(char del_wl[WLlen]) { // case 36
    //char whitelist[WLlen];

    sqlite3 *db;
   	char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char input_sql[SQLlen] = { 0, };
    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };

    openDB();

    res = "Callback Function Called";

    puts("WHITELIST TABLE's records delete.\n");
    puts("input whitelist:");
    fgets(whitelist, WLlen, stdin);
    puts("input id:");
    fgets(id, IDlen, stdin);

    fflush(stdin);
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

int insWhiteListTable(struct WhiteListTable ins_wt) { // case 16
    struct WhiteListTable wt;

    sqlite3 *db;
   	char *errmsg;
    int rc;
    char input_sql[SQLlen] = { 0, };
    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };

    char date[DATElen] = { 0, };
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    openDB();

    puts("WHITELIST TABLE's insert\n");
    puts("input whitelist:");
    fgets(whitelist, WLlen, stdin);
    puts("input id:");
    fgets(id, IDlen, stdin);
    puts("date Enter:");
    printf("%s\n", str_now);
    strncpy(date, str_now, DATElen);

    fflush(stdin);
    strncpy(input_sql, "insert into WHITELIST values('", SQLlen-1);
    strncat(input_sql, whitelist, SQLlen-1);
    strncat(input_sql, "','", SQLlen-1);
    strncat(input_sql, id, SQLlen-1);
    strncat(input_sql, "','", SQLlen-1);
    strncat(input_sql, date, SQLlen-1);
    strcat(input_sql, "');");
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

int insAdminInfo(struct InsInfo ins_ai) { // case 18 & 19 참고
    struct InsInfo ai;
}

int insAdminTable(struct AdminTable ins_at) { // case 18
    struct AdminTable at;

    sqlite3 *db;
    char *errmsg;
    int rc;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };
    char pwd[PWDlen] = { 0, };
    int access;
    char tmp; //엔터키 삭제 변수
    char buf_access[2]; // int형의 access값을 문자로 받을 변수

    openDB();

    puts("ADMIN TABLE's insert\n");
    puts("input id:");
    fgets(id, IDlen, stdin);
    puts("input access:");
    scanf("%d", &access);
    while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
        putchar(tmp);
    }
    puts("pwd:");
    fgets(pwd, PWDlen, stdin);

    sprintf(buf_access, "%d", access); // access를 문자로 변환

    fflush(stdin);
    strncpy(input_sql, "insert into ADMIN values('", SQLlen-1);
    strncat(input_sql, id, SQLlen-1);
    strncat(input_sql, "',", SQLlen-1);
    strncat(input_sql, buf_access, SQLlen-1);
    strncat(input_sql, ",'", SQLlen-1);
    strncat(input_sql, pwd, SQLlen-1);
    strcat(input_sql, "');");
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

int insInfoTable(struct InfoTable ins_it) { // case 19
    struct InfoTable it;

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

    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    openDB();

    printf("\n등록 데이터는 입력 후 EnterKey,\n");
    printf("등록을 건너띄려면 바로 EnterKey를 누르세요.\n");

    puts("\nINFO TABLE's insert\n");
    puts("input id:");
    fgets(id, IDlen, stdin);
    puts("input name:");
    fgets(name, NAMElen, stdin);
    puts("input birth:");
    fgets(birth, BIRTHlen, stdin);
    puts("input email:");
    fgets(email, EMAILlen, stdin);
    puts("input phone:");
    fgets(phone, PHONElen, stdin);
    puts("date EnterKey를 누르세요:");
    printf("%s\n", str_now);
    strncpy(date, str_now, DATElen);

    fflush(stdin);
    strncpy(input_sql, "insert into INFO values('", SQLlen-1);
    strncat(input_sql, id, SQLlen-1);
    strncat(input_sql, "','", SQLlen-1);
    strncat(input_sql, name, SQLlen-1);
    strncat(input_sql, "','", SQLlen-1);
    strncat(input_sql, birth, SQLlen-1);
    strncat(input_sql, "','", SQLlen-1);
    strncat(input_sql, email, SQLlen-1);
    strncat(input_sql, "','", SQLlen-1);
    strncat(input_sql, phone, SQLlen-1);
    strncat(input_sql, "','", SQLlen-1);
    strncat(input_sql, date, SQLlen-1);
    strcat(input_sql, "');");
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

int updateAdmin(struct InsInfo up_a) { // case 28 & 29 참고
    struct InsInfo a;
}

int updateInfoTable(struct InfoTable up_it) { // case 29
    struct AdminTable it;

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

    openDB();

    res = "Callback Function Called";

    printf("INFO TABLE's Update\n");
    puts("수정할 데이터의 id(기본키) 입력:");
    fgets(id, IDlen, stdin);

    fflush(stdin);
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
        while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
            putchar(tmp);
        }
        switch(menu) {
            case 1:
                puts("name 수정:");
                fgets(name, NAMElen, stdin);
                printf("%s\n", str_now);
                strcpy(date, str_now);

                fflush(stdin);
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

                fflush(stdin);
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

                fflush(stdin);
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

                fflush(stdin);
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

int updateAdminTable(struct AdminTable up_at) { // case 28
    struct AdminTable at;

    sqlite3 *db;
    char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char *sql;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };
    int menu; // 수정 항목을 선택하는 변수
    char tmp; //엔터키 삭제 변수

    char** result; // get table result
    int row, col; // get table row,column

    int access;

    char src_pwd[PWDlen] = { 0, }; // 수정할 PWD -> 다른 PWD로 변경됨.
    char pwd[PWDlen] = { 0, };

    char buf_access[2]; // int형의 access값을 문자로 받을 변수

    openDB();

    res = "Callback Function Called";

    printf("ADMIN TABLE's Update\n");

    puts("수정할 데이터의 id(기본키) 입력:");
    fgets(id, IDlen, stdin);
    puts("수정할 id의 PWD 입력(PWD 틀리면 종료):");
    fgets(src_pwd, PWDlen, stdin);

    fflush(stdin);
    strncpy(input_sql, "SELECT id, access FROM ADMIN WHERE pwd = '", SQLlen-1);
    strncat(input_sql, src_pwd, SQLlen-1);
    strncat(input_sql, "' AND id = '", SQLlen-1);
    strncat(input_sql, id, SQLlen-1);
    strcat(input_sql, "';");
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
    //printf("%s\n", result[0]);  // id 와 pwd 를 정확히 입력하면 id 라고 화면에 출력됨.

    if(!strcmp(result[0], "id")) {
        printf("1. ACCESS(권한)만 수정\n");
        printf("2. PWD(비밀번호)만 수정\n");
        printf("3. ACCESS(권한)과 PWD(비밀번호) 모두 수정\n");
        puts("input number:");
        scanf("%d", &menu);
        while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
            putchar(tmp);
        }
        switch(menu) {
            case 1:
                printf("수정할 ACCESS(권한)값 입력:\n");
                scanf("%d", &access);
                while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
                    putchar(tmp);
                }
                sprintf(buf_access, "%d", access); // access를 문자로 변환

                fflush(stdin);
                strncpy(input_sql, "UPDATE ADMIN SET access = ", SQLlen-1);
                strncat(input_sql, buf_access, SQLlen-1);
                strncat(input_sql, " WHERE id = '", SQLlen-1);
                strncat(input_sql, id, SQLlen-1);
                strcat(input_sql, "';");
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
                fgets(pwd, PWDlen, stdin);

                fflush(stdin);
                strncpy(input_sql, "UPDATE ADMIN SET pwd = '", SQLlen-1);
                strncat(input_sql, pwd, SQLlen-1);
                strncat(input_sql, "' WHERE id = '", SQLlen-1);
                strncat(input_sql, id, SQLlen-1);
                strcat(input_sql, "';");
                // printf("%s\n", input_sql);

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
                scanf("%d", &access);
                while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
                    putchar(tmp);
                }
                sprintf(buf_access, "%d", access); // access를 문자로 변환

                puts("수정할 PWD(비밀번호)값 입력:");
                fgets(pwd, PWDlen, stdin);

                fflush(stdin);
                strncpy(input_sql, "UPDATE ADMIN SET (access, pwd) = (", SQLlen-1);
                strncat(input_sql, buf_access, SQLlen-1);
                strncat(input_sql, ", '", SQLlen-1);
                strncat(input_sql, pwd, SQLlen-1);
                strncat(input_sql, "') WHERE id = '", SQLlen-1);
                strncat(input_sql, id, SQLlen-1);
                strcat(input_sql, "';");
                // printf("%s\n", input_sql);

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

int updateAdminwl(struct WhiteListTable up_w) { // case 26
    struct WhiteListTable wt;

    sqlite3 *db;
   	char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char *sql;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };

    char src_white[WLlen] = { 0, }; // 수정할 화이트리스트 ->다른 화이트리스트로 변경됨.
    char whitelist[WLlen] = { 0, };

    char date[DATElen] = { 0, };
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    openDB();

    res = "Callback Function Called";

    puts("WHITELIST TABLE's Update\n");
    puts("update source whitelist:");
    gets(src_white);
    puts("update whitelist:");
    gets(whitelist);

    puts("date Enter:");
    printf("%s\n", str_now);
    strncpy(date, str_now, DATElen);

    fflush(stdin);
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

int insMAC(const char insmac_id[IDlen], const char isnmac_mac[MAClen]) {
    char id[IDlen];
    char mac[MAClen];
}

int updateMac(const char upmac_id[IDlen], const char upmac_mac[MAClen]) {
    char id[IDlen];
    char mac[MAClen];
}

int updatePWD(const char uppwd_id[IDlen], const char uppwd_pwd[PWDlen]) {
    char id[IDlen];
    char pwd[PWDlen];
}

int updateAccess(const char upacc_id[IDlen], int upacc_acc) {
    char id[IDlen];
    int access;

    return 0;
}

int delAdminTable(struct AdminTable del_at) { // case 38
    struct AdminTable at;

    sqlite3 *db;
    char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char input_sql[SQLlen-1] = { 0, };
    char id[IDlen] = { 0, };

    char** result; // get table result
    int row, col; // get table row,column

    int access;

    char del_pwd[PWDlen] = { 0, }; // 삭제되는 PWD
    char pwd[PWDlen] = { 0, };

    openDB();

    res = "Callback Function Called";

    printf("ADMIN TABLE's Update\n");

    puts("삭제하는 데이터의 id(기본키) 입력:");
    fgets(id, IDlen, stdin);
    puts("삭제하는 id의 PWD 입력(PWD 틀리면 종료):");
    fgets(del_pwd, PWDlen, stdin);

    fflush(stdin);
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
        fflush(stdin);
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

int selAdminTable(struct AdminTable sel_at) { // case 48
    struct AdminTable at;

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

    int menu; // search menu

    openDB();

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

        fflush(stdin);
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

        fflush(stdin);
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

        fflush(stdin);
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

int delInfoTable(struct InfoTable del_it) { // case 39
    struct InfoTable it;

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

    openDB();

    puts("삭제하는 데이터의 id(기본키) 입력:");
    fgets(id, IDlen, stdin);

    fflush(stdin);
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

    fflush(stdin);
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

int selInfoTable(struct InfoTable sel_it) { // case 49
    struct InfoTable it;

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

    int menu; // search menu
    char tmp; // Enter Key remove

    openDB();

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

        fflush(stdin);
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

        fflush(stdin);
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

        fflush(stdin);
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

        fflush(stdin);
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

        fflush(stdin);
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
