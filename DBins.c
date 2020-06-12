#define _CTR_SECURE_NO_WARNINGS

#pragma foreign_keys = 1 // 참조키 활성화

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio_ext.h>

#include "sqlite3.h"
#include "DBCreate.h"
#include "DBPrintModule.h"
#include "DBSearchModule.h"
#include "DBManage.h"
#include "DBManagement.h"
#include "DBInsertModule.h"
#include "DBUpdateModule.h"
#include "DBDeleteModule.h"
#include "DBlen.h"
#include "DB.h"
#include "DBLogin.h"
#include "DBBackupR.h"
#include "BaseDefine.h"
#include "DBProgram.h"
#include "DBd.h"
#include "DBins.h"
#include "DBs.h"
#include "DBup.h"

int iw() { // int insWhiteListTable(struct WhiteListTable ins_wt); // 화이트리스트 등록 // case 16
    struct WhiteListTable wt;

    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };
    char W_date[DATElen] = { 0, };

    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t); // DBManage.c

    while(1) { // whitelist 필수로 입력받음
        whitelist[0] = '\0';
        str[0] = '\0';
        printf("\nwhitelist는 등록 후에 수정이 불가능합니다!!!\n");
        printf("whitelist는 필수입력 정보입니다.\n");
        puts("input whitelist (3~30bytes 길이로 입력하세요.):");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize >=4 && strsize <= WLlen)
            break;
        else if(strsize < 4) {
            printf("3 btyes 보다 길게 입력하세요!\n");
            continue;
        }
        else
            printBOF_gets(str, strsize, WLlen); // DBPrintModule.c
    }
    strncpy(wt.whitelist, str, WLlen-1);

    while(1) { // id 필수로 입력받음
        id[0] = '\0';
        str[0] = '\0';
        printf("\nID는 등록 후에 수정이 불가능합니다!!!\n");
        printf("ID는 필수입력 정보입니다.\n");
        puts("input id (5~9bytes 길이로 입력하세요):");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize >= 6 && strsize <= IDlen) // id는 5~9bytes 길이 제한
            break;
        else if(strsize < 6) {
            printf("5 btyes 보다 길게 입력하세요!\n");
            continue;
        }
        else
            printBOF_gets(str, strsize, IDlen); // DBPrintModule.c
    }
    strncpy(wt.id, str, IDlen-1);

    strncpy(wt.W_date, str_now, DATElen);

    insWhiteListTable(wt);

    return 0;
}

int ia() { // int insAdminInfo(struct InsInfo ins_ai); // 관라자 정보 등록 // case 17
    struct InsInfo ai;

    char id[IDlen] = { 0, };
    char pwd[PWDlen] = { 0, };
    char access[ACCESSlen] = { 0, 0};
    char name[NAMElen] = { 0, };
    char birth[BIRTHlen] = { 0, };
    char email[EMAILlen] = { 0, };
    char phone[PHONElen] = { 0, };
    char I_date[DATElen] = { 0, };

    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    int b_date = 0; // birth 문자열을 정수형으로 받을 변수
    int i = 0;
    int pos_at = 0; // email 문자열의 @ 위치
    int pos_dot = 0; // email 문자열의 . 위치
    int p_number = 0; // phone 문자열이 숫자가 아닐 때마다 1씩 증가되는 변수

    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t); // DBManage.c

    while(1) { // id 필수로 입력받음
        id[0] = '\0';
        str[0] = '\0';
        printf("\nID는 등록 후에 수정이 불가능합니다!!!\n");
        printf("ID는 필수입력 정보입니다.\n");
        puts("input id (5~9bytes 길이로 입력하세요):");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize >= 6 && strsize <= IDlen) // id는 5~9bytes 길이 제한
            break;
        else if(strsize < 6) {
            printf("5 btyes 보다 길게 입력하세요!\n");
            continue;
        }
        else
            printBOF_gets(str, strsize, IDlen); // DBPrintModule.c
    }
    strncpy(ai.id, str, IDlen-1);

    while(1) { // access 입력받음
        access[0] = 'N';
        str[0] = '\0';
        printf("\nACCESS는 수정이 가능합니다.\n");
        puts("권한 취득 전이면 엔터키를 누르세요->기본 값(N)으로 등록됩니다.");
        puts("input access (E, C, R):");
        puts("E : EV3 ROBOT 관리자");
        puts("C : CPS 관리자");
        puts("R : ROOT");
        puts("N : default 아직 권한 취득 전");
        gets(str);

        strsize = strlen(str)+1;
        if(strsize > ACCESSlen) {
             printBOF_gets(str, strsize, ACCESSlen); // DBPrintModule.c
             continue;
        }

        if(str[0] == 'E' || str[0] == 'C' || str[0] == 'R' || str[0] == 'N')
            break;
        if(str[0] == '\0') {
            str[0] = 'N';
            break;
        }
        else
            printf("잘못 입력하셨습니다.\n"); // 엔터키(N), E, C, R 을 제외하고는 무한루프
    }
    strncpy(ai.access, str, ACCESSlen-1);

    while(1) { // pwd 필수로 입력받음
        pwd[0] = '\0';
        str[0] = '\0';
        printf("\nPWD는 수정이 가능합니다.\n");
        printf("PWD는 필수입력 정보입니다.\n");
        puts("input pwd (513bytes 보다 길면 다시 입력함):");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize <= PWDlen)
            break;

        printBOF_gets(str, strsize, PWDlen); // DBPrintModule.c
    }
    strncpy(ai.pwd, str, PWDlen-1);

    printf("\n지금 입력하는 id가 %s인 정보는 id, pwd 분실 시 확인 정보로 사용됩니다!!!\n", ai.id);
    printf("등록하려면 입력 후 EnterKey,\n");
    printf("등록을 건너띄려면 바로 EnterKey를 누르세요.\n");

    while(1) { // name 입력받음
        name[0] = '\0';
        str[0] = '\0';
        printf("\n지금 입력하는 정보는 id, pwd 분실 시 확인 정보로 사용됩니다!!!\n");
        printf("NAME은 수정이 가능합니다.\n");
        printf("등록을 건너띄려면 바로 EnterKey를 누르세요.\n");
        puts("input name (2~30bytes 길이로 입력하세요):");
        gets(str);

        str[0] == '\n'; // EnterKey를 누르면 무한푸프 탈출
            break;
        strsize = strlen(str)+1;

        if(strsize >= 3 && strsize <= NAMElen)
            break;
        else if(strsize < 3) {
            printf("2bytes 보다 길게 입력하세요!\n");
            continue;
        }
        else
            printBOF_gets(str, strsize, NAMElen); // DBPrintModule.c
    }
    strncpy(ai.name, str, NAMElen-1);

    while(1) { // 생년월일은 yyyymmdd 8개의 유효 날짜로 입력받음
        birth[0] = '\0';
        str[0] = '\0';
        printf("\n지금 입력하는 정보는 id, pwd 분실 시 확인 정보로 사용됩니다!!!\n");
        printf("BIRTH는 수정이 가능합니다.\n");
        printf("등록을 건너띄려면 바로 EnterKey를 누르세요.\n");
        puts("input birth (yyyymmdd 8개 유효 날짜(숫자)가 아니면 다시 입력함):");
        gets(str);

        str[0] == '\n'; // EnterKey를 누르면 무한푸프 탈출
            break;
        strsize = strlen(str)+1;

        if(strsize == BIRTHlen) {
            b_date = atoi(str);
            if(checkDate(b_date) == 1) // DBManage.c : 1이면 참(유효날짜)
                 break;
            else {
                printf("유효 날짜가 아닙니다!!!\n");
                continue;
            }
        }

        else
            printf("yyyymmdd 8개의 유효 날짜(숫자)로 입력해야 합니다!\n");
    }
    strncpy(ai.birth, str, BIRTHlen-1);

    while(1) { // 이메일 : @는 [1] ~ [끝-2] && .은 [@위치+2] ~ [끝]
        email[0] = '\0';
        str[0] = '\0';
        printf("\n지금 입력하는 정보는 id, pwd 분실 시 확인 정보로 사용됩니다!!!\n");
        printf("EMAIL은 수정이 가능합니다.\n");
        printf("EMAIL은 필수 입력정보입니다..\n");
        puts("input email (50bytes 보다 길면 다시 입력함):");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize > EMAILlen) {
            printBOF_gets(str, strsize, EMAILlen); // DBPrintModule.c
            continue;
        }

        for(i = 0; i < strsize; i++) { // 입력받은 이메일의 한 글자씩 확인하는 루프
            if(i > 0 && str[i] == '@') // @는 맨 앞에 올 수 없음
                pos_at = i+1;
            if(i > pos_at && str[i] =='.') // .은 @+1 보다 뒤에 있음
                pos_dot = i+1;
        }

        if(pos_at > 0 && pos_dot > 3 && pos_dot < i) // @는 맨 앞이면 안됨 && .은 4번째~끝(i) 사이
            break;
        else {
            printf("이메일 형식 -@-.- 에 맞지 않습니다!\n");
            continue;
        }
    }
    strncpy(ai.email, str, EMAILlen-1);

    while(1) { // 전화번호는 자연수만 입력 가능
        phone[0] = '\0';
        str[0] = '\0';
        printf("\n지금 입력하는 정보는 id, pwd 분실 시 확인 정보로 사용됩니다!!!\n");
        printf("PHONE 번호는 수정이 가능합니다.\n");
        printf("등록을 건너띄려면 바로 EnterKey를 누르세요.\n");
        printf("-표시 없이 숫자만 입력하세요.\n");
        puts("input phone (2~20개의 숫자로만 입력하세요):");
        gets(str);
        if(str[0] == '\0') // EnterKey를 누르면 무한루프 탈출
            break;
        strsize = strlen(str)+1;
        if(strsize > PHONElen) {
            printBOF_gets(str, strsize, PHONElen); // DBPrintModule.c
            continue;
        }

        if(strsize < 3) { // 숫자 1개만 입력하면 반복
            printf("최소 2개의 숫자를 입력하세요!!!\n");
            continue;
        }

        for(p_number = 0, i = 0; i < strsize; i++) { // 입력받은 숫자를 하나씩 확인하는 루프
            if((str[i] < '0') || (str[i] > '9')) // 숫자가 아닌 문자가 입력된 경우 반복
                p_number++; // 숫자가 아니면 p_number 변수 값에 1을 더함
        }
        if(p_number == 1) // 전화번호가 길이제한을 넘기지 않고, 모두 숫자로만 입력된 경우는 무한루프 탈출
            break;
    }
    strncpy(ai.phone, str, PHONElen-1);

    strncpy(ai.I_date, str_now, DATElen);

    insAdminInfo(ai);

    return 0;
}

int im() { // int insMAC(const char insmac_id[IDlen], const char isnmac_mac[MAClen]); // 맥 등록 // case 18

}