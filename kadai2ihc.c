//情報メディア実験B 進化的アルゴリズム//

/*///////////////////////////////////////////////////////////////////////
実験２
「山登り」法のプログラムを作成し、グラフ色ぬり問題を解いてください解いてください	
*////////////////////////////////////////////////////////////////////////


////実験5用のメモ書き////
/*
IHC 山登り * 再スタート
山登り中の違反点数の計算 ３回

*/
///////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h> //現在時刻を取得するため

#define N 150 //nの設定
#define TIMES 100 //step5の指定回数
#define LOOP 100 //step6のリスタートの回数

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

while( count < m2){
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
printf("\n");

///////////////////////////////////////////////////////////////////////////

//問題設定
int solution[N]; //解候補
int solution_v[N]; //違反箇所(違反している場合は1を、していない場合は0を代入する)
int violation = 0,violation_recoad = 0; //違反点数
int violation_before = N*N; //前回違反回数
int k = 0;
int challenge = 0; //試行回数
int challenge_all = 0; //総合試行回数
int restart = 0;
int D; //色を記録
int recoad = 0; //記録用
int count_all = 0; //課題5のための違反点数計算回数

//結果記録ファイルの準備
FILE *fp;
fp = fopen("recoad.csv","w");
if(fp == NULL){
	printf("Can't open flie\n");
	return 1;
}

//リスタートの設定

while(restart < LOOP){
//printf("山登り%d回目 総合試行回数 %d\n",restart+1,challenge_all);

//step1:初期値の作成

	//変数の初期化
	challenge = 0;
	violation_before = N*N;
	//色の設定:これより0:赤,1:緑,2:青という設定の元ランダムに代入し候補解とする
	for (i = 0; i < N ; i++){
		D = (int)((double)3*rand()/(RAND_MAX+1.0)); //赤緑青の色をランダムに取得
		solution[i] = D; //解候補の代入
		solution_v[i] = 0; //違反していないとする
	}

//step2:終了判定

	while (challenge < TIMES){

		//違反箇所の初期化
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
		count_all++;

		//違反点数が０ならば試行を終了する
		if(violation == 0){
			recoad = 0; //違反点数を0とする
			restart = 10; //リスタートをスキップする
			fprintf(fp, "%d\n",violation );
			printf("成功\n");
			restart = LOOP;
			break;
		}

//step3:変数の選択

		//違反箇所である場所をランダムに取得する
		int v_point,v_number;
		int b_point = 0;

		while(b_point == 0){
			v_point = (int)((double)N*rand()/(RAND_MAX+1.0));//ランダムに値を取得する
			//取得した値が違反箇所ならばそこを変数として選択する
			if(solution_v[v_point] == 1){
				v_number = solution[v_point];
				b_point = 1;
			}
		}

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
	count_all++;
	//次に二つ目について考える
	solution[v_point] = violation_2;
	for (i = 0; i < N ; i++){
		for (j = i+1; j < N ; j++){
			if(n[i][j] == 1 && solution[i] == solution[j]){
				check_2++; //違反点数を増やす
			}
		}
	}
	count_all++;
//step5:山登り

	//変えないほうがいい場合
	 if(violation < check_1 && violation < check_2){
		solution[v_point] = v_number; //元の値を再代入 
	}else

	 if(check_1 == check_2){
		//数が一緒ならランダムに決める
		
		int A = (int)((double)2*rand()/(RAND_MAX+1.0)); //赤緑青の色をランダムに取得
		if(A == 0){
			solution[v_point] = violation_1;
		}else{
			solution[v_point] = violation_2;
		}

	}else{

	 	if(check_1 < check_2){
			solution[v_point] = violation_1;
		}else if(check_1 > check_2){
			solution[v_point] = violation_2;
		}else{
			//
		}
	}
	challenge++;
	challenge_all++;
	//前回と違反点数が変わったどうか
	
	if(violation_before > violation){
		challenge = 0;
	}
		
	fprintf(fp, "%d\n",violation );
	violation_before = violation;
	violation = 0;
	
	} //変数を変えながら繰り返す
	restart++;


}

	printf("総合試行回数 = %d \n",challenge_all);
	printf("課題5　総違反点数試行回数 = %d\n",count_all);
	if(restart == LOOP){
		printf("探索に失敗しました\n");
	}
	/*
	for (i = 0; i < N ; i++){
			printf(" %d",solution[i]);
		}
	*/	
	printf("\n\n");
	fclose(fp);
	
	return 0;

}