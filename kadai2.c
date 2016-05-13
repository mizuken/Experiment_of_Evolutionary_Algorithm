//情報メディア実験B 進化的アルゴリズム//

/*///////////////////////////////////////////////////////////////////////
実験２
「山登り」法のプログラムを作成し、グラフ色ぬり問題を解いてください解いてください	
*////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h> //現在時刻を取得するため

#define N 36 //nの設定

int main(void){
 
 int n[N][N]; //n:N個の２次元配列を用意する
 int i,j; // 初期化するforループ用の変数
 int m1 = N*(N-1)/4; //密結合問題用のm
 int m2 = 3*N;       //疎結合問題用のm

/*二次元配列を0で初期化する*/
for (i = 0; i < N ; i++){
	for (j = 0; j < N ; j++){
		n[i][j] = 0; //０を代入して初期化する
	}
}

 /* seedの設定 (現在時刻をseedとして取得してランダムに値を変える)*/
 srand((unsigned) time(NULL));

int area,p,q; //ランダムな値をとる変数
int count = 0; //リンクの本数を数える;
int num = N/3;

//m本のリンクを貼るまで配列の数字を１に変更していく

while( count < m1){
	//乱数を三つとり、どの領域、そして２次元座標を取得する
	area  = (int)((double)3*rand()/(RAND_MAX+1.0)); //0,1,2のいずれかの数字を取得
	p = (int)((double)num*rand()/(RAND_MAX+1.0));
	q = (int)((double)num*rand()/(RAND_MAX+1.0));
	switch(area){
		case 0: //(0,1)のエリアに１を配置
		if(n[p][num+q] != 1){
			n[p][num+q] = 1;
			count++;
		}
		break;
		case 1: //(0,2)のエリアに１を配置
		if(n[p][2*num+q] != 1){
			n[p][2*num+q] = 1;
			count++;
		}
		break;
		case 2: //(1,2)のエリアに１を配置
		if(n[num+p][2*num+q] != 1){
			n[num+p][2*num+q] = 1;
			count++;
		}
		break;
		default:
		break;
	}
	//１に変更できたらcountを１増やす
}

//完成した問題を出力する

printf("実行結果\n\n");
for(i = 0; i < N ; i++){
	for (j = 0; j < N ; j++){
		printf(" %d", n[i][j]);
		//n列だけ表示したら改行する
		if((j+1)%N == 0){
			printf("\n");
		}
	}
}

printf("\n");

///////////////////////////////////////////////////////////////////////////

//問題設定
int solution[N]; //解候補
int solution_v[N]; //違反箇所(違反している場合は1を、していない場合は0を代入する)
int violation = 0; //違反点数
int violation_before = 0; //前回の違反点数
int challenge = 0; //試行回数
int restart = 0; //リスタート判定回数
int D; //色


//リスタートの設定
while(restart < 5){
	restart++;
	printf("%d回目のスタート\n",restart);

//step1:初期値の作成

	//色の設定:これより0:赤,1:緑,2:青という設定の元ランダムに代入し候補解とする
	for (i = 0; i < N ; i++){
		D = (int)((double)3*rand()/(RAND_MAX+1.0)); //赤緑青の色をランダムに取得
		solution[i] = D; //解候補の代入
		solution_v[i] = 0; //違反していない
	}

//step2:終了判定

			while (challenge < 100){
				for(i = 0 ; i < N; i++){
					solution_v[i] = 0;
				}
			//違反点数を計算する
			for (i = 0; i < N ; i++){
				for (j = i+1; j < N ; j++){
					if(n[i][j] == 1 && solution[i] == solution[j]){
						violation++; //違反点数を増やす
						solution_v[i] = 1; //違反しているのでチェックを入れる
						solution_v[j] = 1; //違反しているのでチェックを入れる
					}
				}
			}

			printf("challenge = %d, violation = %d-----------------------------\n",challenge,violation); //違反回数の確認

			//違反点数が前回より少なくなっているのならchallengeをリセットする
			/*
			if(violation < violation_before){
				challenge = 0;
			}
			*/

			//違反点数が0ならば解を見つけたので探索を終了する
			if(violation == 0){
				printf("探索終了!\n");
				restart = 5;
				break;
			}
			violation_before = violation;
			//step3:変数の選択

			//違反箇所である場所をランダムに取得する
			int v_point;
			int b_point = 0;

			while(b_point == 0){
				v_point = (int)((double)N*rand()/(RAND_MAX+1.0));//ランダムに値を取得する
				//取得した値が違反箇所ならばそこを変数として選択する
				if(solution_v[v_point] == 1){
					b_point = 1;
				}
			}
			//printf("v_point = %d\n",v_point ); //変数の選択箇所確認

			//step4:値の選択
			int violation_1 = 0, violation_2 = 0; //値の選択のために違反箇所を考える
			int check_1 = 0, check_2 = 0; //一時的に違反数を記憶する

			//使っていない色で考えるためにその色を取得する
			switch(solution[v_point]){
				case 0:
				//1,2でどちらが違反しているか考える
				violation_1 = 1;
				violation_2 = 2;
				break;
				case 1:
				//0,2でどちらが違反しているか考える
				violation_1 = 0;
				violation_2 = 2;
				break;
				case 2:
				//0,1でどちらが違反しているか考える
				violation_1 = 0;
				violation_2 = 1;
				break;
				default:
				break;
			}

			//２つの色の場合について違反数を数え少ないものを採用する

			//まず一つ目の場合について考える
			solution[v_point] = violation_1;
			for (i = 0; i < N ; i++){
				for (j = i+1; j < N ; j++){
					if(n[i][j] == 1 && solution[i] == solution[j]){
						check_1++; //違反点数を増やす
					}
				}
			}
			//次に二つ目について考える
			solution[v_point] = violation_2;
			for (i = 0; i < N ; i++){
				for (j = i+1; j < N ; j++){
					if(n[i][j] == 1 && solution[i] == solution[j]){
						check_2++; //違反点数を増やす
					}
				}
			}

			//数の比較する
			printf("violation = %d check_1 = %d check_2 = %d\n",violation,check_1,check_2);
			if(violation < check_1 && violation < check_2){
				//色を変更して違反点数が増えるなら変えないで探索を進める
				//printf("変える必要なし\n\n");
			}else if(check_1 == check_2){
				//数が一緒ならランダムに決める
				//printf("ランダムに決定\n\n");
				int A = (int)((double)2*rand()/(RAND_MAX+1.0)); //赤緑青の色をランダムに取得
				if(A == 0){
					solution[v_point] = violation_1;
				}else{
					solution[v_point] = violation_2;
				}
			}else if(check_1 < check_2){
				//printf("check1に決定\n\n");
				solution[v_point] = violation_1;
			}else{
				//printf("check2に決定\n\n");
				solution[v_point] = violation_2;
			}

//step5:山登り
			challenge++;
			violation = 0;
		} //変数を変えながら100回繰り返す。それで減少しなければリスタートする

//step6リスタート
	} //challenge:試行回数が100になるまでに違反回数が0にならなければ初めからやり直す

printf("実行結果\n\n");
for(i = 0; i < N ; i++){
	for (j = 0; j < N ; j++){
		printf(" %d", n[i][j]);
		//n列だけ表示したら改行する
		if((j+1)%N == 0){
			printf("\n");
		}
	}
}
printf("\n\n");

for (i = 0; i < N ; i++){
		printf(" %d",solution[i]);
	}
printf("\n\n");
return 0;

}
