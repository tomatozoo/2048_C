/* programming practice term project */
/*       author : Park YongJu        */


// import header files
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>

#define SIZE 4

int board[SIZE][SIZE] = {0};
int score = 0;
int scheme = 0;
typedef enum {F, T} boolean;
boolean GAMEOVER = F;
boolean ADDRANDOM = T;

typedef enum{
    up = 0,
    down, 
    right,
    left
} status;

typedef enum{
    yellow = 0,
    purple,
    red
}color;

int getch(void){
    // int c
    int c; 
    // termios 구조체
    // POSIX가 지정하는 표준 인터페이스
    struct termios origin; 
    struct termios reset; 

    // 구조체 초기화
    tcgetattr(0, &origin); 

    // new에 old 할당
    reset = origin; 

    // c_lflag 멤버의 플래그
    reset.c_lflag &= ~(ICANON|ECHO); 
    // 지원하는 대응 문자 - VMIN MIN 문자, VTIME 

    reset.c_cc[VMIN] = 1; // read는 읽어들일 문자가 있거나
    reset.c_cc[VTIME] = 0; // TIME/10초 경과시 리턴

    // 구조체 초기화
    tcsetattr(0, TCSAFLUSH, &reset); 
    c = getchar(); // 얘를 변형함

    // 구조체 초기화 - 파라미터 설정
    // TCSAFLUSH : fd에 출력이 단말에 송신된 다음에 시작
    tcsetattr(0, TCSAFLUSH, &origin); 

    return c;
}

void initBoard(void){
    // 무조건 2 두 개로 시작함
    int x1 = rand() % 4;
    int y1 = rand() % 4;
    int x2 = rand() % 4;
    int y2 = rand() % 4;

    if ((x1==x2)&&(y1==y2)){
        x2 = rand() % 4;
        y2 = rand() % 4;
    }
    
    board[x1][y1] = 2;
    board[x2][y2] = 2;
}

void getColor(int value, int scheme) {
    int fore[] = {255,0};

    int origin[] = {8,1,2,3,4,5,6,7,9,10,11,12,13,14,255,255};
    int black[] = {232,234,236,238,240,242,244,246,248,249,250,251,252,253,254,255};
    int blue[] = {235,63,57,93,129,165,201,200,199,198,197,196,196,196,196,196};

    int *list[] = {origin, black, blue};

	int *background = list[scheme]+0; 
	int *foreground = fore+0; 

	if (value > 0) {
        while (value--) {
		if (background+2<list[scheme]+sizeof(list[scheme])) {
			background+=1; 
		}
	}
    }
    printf("\033[38;5;%d;48;5;%dm",*foreground,*background);
}

void plainBoard(int board[SIZE][SIZE]){
    printf("\033[H");
    
    printf("\n\n\t2048.c\t%dpts\n\n", score);
    
    for (int i=0;i<SIZE;i++){
        for (int j = 0;j<SIZE;j++){
            getColor(board[i][j], scheme);
            printf("       ");
            printf("\033[m");
        }
        printf("\n");
        for (int j = 0;j<SIZE;j++){
            getColor(board[i][j], scheme);
            if (board[i][j]==0){
                printf("   ・  ");
            }
            else {
                char s1[10];
                sprintf(s1, "%d", board[i][j]);
                int t = 7-strlen(s1);
                printf("%*s%s%*s", t-t/2, "", s1, t/2, "");
            }
            printf("\033[m");
        }
        printf("\n");
        for (int j = 0;j<SIZE;j++){
            getColor(board[i][j], scheme); 
            printf("       ");
            printf("\033[m");
        }
        printf("\n");
}
        printf("\n");
        printf("\t←,↑,→,↓ or q\n");
        printf("\033[A"); 
}

void moveboard(status tmp){
    switch(tmp){
        case 0:
            for (int column = 0;column<SIZE;column++){
                int index = 0;
                int tmp_row[4] = {0};
                for (int row = 0;row<SIZE;row++){
                    if (board[row][column]!=0){
                        tmp_row[index] = board[row][column];
                        index++;
                    }
                }
                for (int row=0;row<SIZE;row++){
                    board[row][column] = tmp_row[row];
                }
            }
            break;
        case 1:
            //down
            for (int column = 0;column<SIZE;column++){
                int index = 0;
                int tmp_row[4] = {0};
                for (int row = SIZE-1;row>=0;row--){
                    if (board[row][column]!=0){
                        tmp_row[index] = board[row][column];
                        index++;
                    }
                }
                index = 0;
                for (int row=SIZE-1;row>=0;row--){
                    board[row][column] = tmp_row[index];
                    index++;
                }
            }
            break;      
        case 2:
            //right
            for (int row = 0;row<SIZE;row++){ // col
                int index = 0;
                int tmp_col[4] = {0};
                for (int column = SIZE-1;column>=0;column--){ // row
                    if (board[row][column]!=0){
                        tmp_col[index] = board[row][column];
                        index++;
                    }
                }
                index = 0;
                for (int column=SIZE-1;column>=0;column--){ // row
                    board[row][column] = tmp_col[index];
                    index++;
                }
            }    
            break;   

        case 3:
            //left
            for (int row = 0;row<SIZE;row++){ //col
                int index = 0;
                int tmp_col[4] = {0};
                for (int column = 0;column<SIZE;column++){ //row
                    if (board[row][column]!=0){
                        tmp_col[index] = board[row][column];
                        index++;
                    }
                }
                index = 0;
                for (int column = 0;column<SIZE;column++){ //row
                    board[row][column] = tmp_col[index];
                    index++;
                }
            }
            break;         
    }
}

void findPair(status tmp){
    switch(tmp){
        case 0:  //up
            for (int column = 0;column<SIZE;column++){
                int tmp_row[4] = {0};
                for (int row = 0; row<SIZE; row++){
                    tmp_row[row] = board[row][column];
                }
                for (int row=0;row<SIZE-1;row++){
                     if (tmp_row[row] == tmp_row[row+1]) {
                        tmp_row[row] *= 2;
                        score += tmp_row[row];
                        tmp_row[row+1] = 0;
                        moveboard(tmp);
                        break;
                    }
                }
                for (int row = 0;row<SIZE;row++){
                    board[row][column] = tmp_row[row];
                }                
            }            
            break;
        case 1:  //down
            for (int column = 0;column<SIZE;column++){
                int tmp_row[4] = {0};
                for (int row = SIZE-1;row>=0;row--){
                    tmp_row[row] = board[row][column];
                }
                for (int row = SIZE-1;row>0;row--){
                    if (tmp_row[row] == tmp_row[row-1]){
                        tmp_row[row] *= 2;
                        score += tmp_row[row];
                        tmp_row[row-1] = 0;
                        moveboard(tmp);
                        break;
                    }
                }
                for (int row=SIZE-1;row>=0;row--){
                    board[row][column] = tmp_row[row];
                }
            }
            break;
        case 2: 
            //right
            for (int row = 0;row<SIZE;row++){ // col
                int tmp_col[4] = {0};
                for (int column = SIZE-1;column>=0;column--){ // row
                    tmp_col[column] = board[row][column];
                }
                for (int column = SIZE-1;column>0;column--){
                    if (tmp_col[column] == tmp_col[column-1]){
                        tmp_col[column] *= 2;
                        score += tmp_col[column];
                        tmp_col[column-1] = 0;
                        moveboard(tmp);
                        break;
                    }
                }
                for (int column=SIZE-1;column>=0;column--){ // row
                    board[row][column] = tmp_col[column];
                }
            }  
            break;
        case 3:  //left
            for (int row = 0;row<SIZE;row++){ //col
                int tmp_col[4] = {0};
                for (int column = 0;column<SIZE;column++){ 
                    tmp_col[column] = board[row][column];
                }
                for (int column = 0;column<SIZE-1;column++){ 
                    if (tmp_col[column] == tmp_col[column+1]){
                        tmp_col[column] *= 2;
                        score += tmp_col[column];
                        tmp_col[column+1] = 0;
                        moveboard(tmp);
                        break;
                    }
                }
                for (int column = 0;column<SIZE;column++){ 
                    board[row][column] = tmp_col[column];
                }
            }
            break;   
    }
}

void addPoints(){
    int len = 0;
    int isEmpty[SIZE*SIZE][2]; // 16 * 2
    int point = 2;

    for (int x = 0;x<SIZE;x++)
    {
        for (int y = 0;y<SIZE;y++){
            if (board[x][y]==0){
                isEmpty[len][0] = x;
                isEmpty[len][1] = y;
                len++;
            }
        }
    }


    if (len > 0){
        int random = (int)(rand() % len);
        int x = isEmpty[random][0];
        int y = isEmpty[random][1];
        board[x][y] = 2;
    }

}

boolean gameOver(void){
    boolean tmp = T;
    for (int i = 0; i<SIZE;i++){
        for (int j = 0;j<SIZE;j++){
            if (board[i][j]==0){
                tmp = F;
            }
        }
    }
    return tmp;
}

int main(int argc, char *argv[]){

    if (argc == 2) {
        if (strcmp(argv[1], "original")==0){
            scheme = 0;
        }
        if (strcmp(argv[1], "black")==0){
            scheme = 1;
        }
        if (strcmp(argv[1], "blue")==0){
            scheme = 2;
        }        
    }
    printf("\033[?25l\033[2J");
    
    boolean bool1;
    bool1 = T;
    initBoard();

    while (bool1==T){
        plainBoard(board);
        //keyboard input
        int c;
        c = getch();
        status tmp;
        switch (c){
            case 119:
            case 107: tmp = up; break;
            // 아래
            case 115:
            case 106: tmp = down; break;
            // 오
            case 100:
            case 108: tmp = right; break;
            // 왼
            case 104:
            case 97: tmp = left; break;

            case 27:{
                getch();
                char d = getch();
                switch (d){
                    case 65: tmp = up; break;
                    case 66: tmp = down; break;
                    case 68: tmp = left; break;
                    case 67: tmp = right; break;
                }

                break;
            }

            case 81:
            case 113:
                printf("        QUIT? (y/n)         \n");
                c = getch();
                if (c=='Y'||c=='y'){
                    printf("\tTERMINATED\n");
                    return 0;
                }
            default: printf("ERROR\n"); break;
        }
        moveboard(tmp);
        findPair(tmp);

        if (gameOver()==T){
            printf("\n\tGAME OVER\n");
            break;
        }
        addPoints();
        
    }
    return 0;
}