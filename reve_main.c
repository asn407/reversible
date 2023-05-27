#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

//盤面 空白(0) 黒(-1) 白(1) 番兵(2)
int board[10][10] = {0};
//手番
int player = -1;

//重み付きデータ
int weightdata[10][10] = {
    {  0,  0,  0,  0,  0,  0,   0,   0,  0,  0},
    {  0, 30,-12,  0, -1, -1,  0, -12,  30,  0},
    {  0,-12,-15, -3, -3, -3, -3, -15, -12,  0},
    {  0,  0, -3,  0, -1, -1,  0,  -3,   0,  0},
    {  0, -1, -3, -1, -1, -1, -1,  -3,  -1,  0},
    {  0, -1, -3, -1, -1, -1, -1,  -3,  -1,  0},
    {  0,  0, -3,  0, -1, -1,  0,  -3,   0,  0},
    {  0,-12,-15, -3, -3, -3, -3, -15, -12,  0},
    {  0, 30,-12,  0, -1, -1,  0, -12,  30,  0},
    {  0,  0,  0,  0,  0,  0,   0,   0,  0,  0}
};

//盤面の生成
void make_board(){
	//番兵
	for(int i = 0; i < 10; i++){
		board[0][i] = 2;
		board[9][i] = 2;
		board[i][0] = 2;
		board[i][9] = 2;
	}
	//基本位置
	board[4][4] = 1;
	board[5][5] = 1;
	board[4][5] = -1;
	board[5][4] = -1;
}

//盤面の表示
void show_board(){
	//番兵も含めて表示
    printf(" １２３４５６７８ -> x\n");
	for(int i = 1; i < 9; i++){
        printf("%d", i);
		for(int j = 1; j < 9; j++){
			switch(board[i][j]){
				case 0:
					printf("ー");
					break;
				case 1:
					printf("●");
					break;
				case -1:
					printf("○");
					break;
				default:
					break;
			}
		}
		putchar('\n');
	}
    printf("|\n");
    printf("y\n");
}

//手番の表示
void show_player(){
	switch(player){
		case -1:
			printf("黒（あなた）の手番です\n");
			break;
		case 1:
			printf("白（ＰＣ）の手番です\n");
			break;
		default:
		    //puts("error");
			break;
	}
}

//特定の座標から特定の方向に挟めるか判定
int check_dir(int i, int j, int dir_i, int dir_j, int now_board[10][10]){
	//指定方向に相手の石がある場合は次のマスを探索する
	int times = 1;
	while(now_board[i+dir_i*times][j+dir_j*times] == player*-1){
		times++;
	}
	//指定方向の最後に自分の石がある場合
	if(now_board[i+dir_i*times][j+dir_j*times] == player){
		//指定方向に相手の石が何個あるかを返す
		return times-1;
	}
	//指定方向の最後に自分の石がなければ0を返す
	return 0;
}

//特定の場所に置くことができるか判定
bool check_plc(int i, int j, int now_board[10][10]){
	//場所が空であるかどうか
	if(board[i][j] == 0){
		//全方向を探索
		for(int dir_i = -1; dir_i < 2; dir_i++){
			for(int dir_j = -1; dir_j < 2; dir_j++){
				if(check_dir(i,j,dir_i,dir_j,now_board)){
					//配置可能であればtrueを返す
					return true;
				}
			}
		}
	}
	return false;
}

//終了判定
bool flag_fin(){
	//置ける場所があるか判定
	for(int i = 1; i < 9; i++){
		for(int j = 1; j < 9; j++){
			if(check_plc(i,j,board)){
				return true;
			}
		}
	}
	//プレイヤーを変えて置ける場所があるか判定
	player *= -1;
	for(int i = 1; i < 9; i++){
		for(int j = 1; j < 9; j++){
			if(check_plc(i,j,board)){
				printf("置く場所がないためPlayerを変更しました\n");
				return true;
			}
		}
	}
	return false;
}

//石を配置する
void place_stn(int i, int j, int now_board[10][10]){
	//方向毎に走査
	for(int dir_i = -1; dir_i < 2; dir_i++){
		for(int dir_j = -1; dir_j < 2; dir_j++){
			//挟んだ石の数
			int change_num = check_dir(i,j,dir_i,dir_j,now_board);
			//挟んだ石の数だけ置き換える
			for(int k = 1; k < change_num+1; k++){
				now_board[i+dir_i*k][j+dir_j*k] = player;
			}
		}
	}
	//配置箇所を置き換える
	now_board[i][j] = player;
}

//勝敗判定
void judge_board(){
	int count_b = 0; //黒石の数
	int count_w = 0; //白石の数
	for(int i = 1; i < 9; i++){
		for(int j = 1; j < 9; j++){
			if(board[i][j] == -1){
				count_b++;
			}else if(board[i][j] == 1){
				count_w++;
			}
		}
	}
	//結果表示
	printf("黒:%d 白:%d\n", count_b, count_w);
	//勝敗判定
	if(count_b > count_w){
		printf("あなたの勝利\n");
	}else if(count_w > count_b){
		printf("ＰＣの勝利");
	}else{
		printf("引き分け");
	}
}

//////////////////////////////////////////////////
//コンピュータ思考ルーチン

//盤面コピー
void copydata(int tmpdata[10][10]){
    for(int y = 0; y < 10; y++){
        for(int x = 0; x < 10; x++){
            tmpdata[y][x] = board[y][x];
        }
    }
}

//重みづけ計算関数
int calcweight(int tmpdata[10][10]){
    int score = 0;
    for(int y = 0; y < 10; y++){
        for(int x = 0; x < 10; x++){
            if(tmpdata[y][x] == 2){
                continue;
            }
            if(tmpdata[y][x] == 1){
                score += weightdata[y][x];
            }
        }
    }
    return score;
}

//コンピュータ思考関数
void think(){
    int hightscore = -1000;
    int px, py;
    for(int y = 0; y < 10; y++){
        for(int x = 0; x < 10; x++){
            if(!check_plc(y, x, board)){
                continue;
            }
            int tmpdata[10][10] = {0};
            copydata(tmpdata);
            place_stn(y, x, tmpdata);

            int score = calcweight(tmpdata);
            if(score > hightscore){
                hightscore = score;
                px = x; py = y;
            }
        }
    }
    place_stn(py, px, board);
    printf("ＰＣは(x , y) = (%d , %d)に置きました\n", px, py); 
}

//コンピュータ停止時間
void mySleep(int second){
    time_t now = second * CLOCKS_PER_SEC + clock();
    while (now > clock()) ;
}

///////////////////////////////////////////////

int main(void){
	//盤面の生成
	make_board();
	//終了までループ
	while(flag_fin()){
        //コンピュータのターン
        if(player == 1){
			show_board();
            show_player();
			printf("ＰＣ思考中...\n");
			mySleep(3);
            think();
            player *= -1;
            continue;
        }
		//盤面の表示
		show_board();
		//手番の表示
		show_player();
		//入力受付
		int i,j,k;
		char x[256];
		char y[256];
		bool isNum = true;
		do{
			printf("配置場所を入力してください\n");
            printf("(x , y) = ");
			scanf("%s %s", x, y);
			for(k = 0; k < strlen(y); k++){
				if(!isdigit(y[k])){
					isNum = false;
					break;
				}
			}
			if(isNum){
				i = atoi(y);
			} else {
				i = 0;
			}
			for(k = 0; k < strlen(x); k++){
				if(!isdigit(x[k])){
					isNum = false;
					break;
				}
			}
			if(isNum){
				j = atoi(x);
			} else {
				j = 0;
			}
		}while(!check_plc(i,j,board));
		//石を配置する
		place_stn(i,j,board);
		//手番を入れ替える
		player *= -1;
	}
	//盤面の表示
	show_board();
	//勝利判定
	judge_board();
	return 0;
}