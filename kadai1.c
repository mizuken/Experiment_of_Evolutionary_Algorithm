//情報メディア実験B 進化的アルゴリズム//

/*///////////////////////////////////////////////////////////////////////
実験１
2.2節の方法でグラフの色ぬり問題を生成してください。ただし、nodeの数はnの３倍数とします。
リンク数は次のようにしてください
 密結合問題 m = n(n-1)/4
 疎結合問題 m = 3n
*////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <time.h> //現在時刻を取得するため

#define N 9 //nの設定

int main(void){

//step1: n*nの２次元配列を作成する(0で初期化)
 
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

//step2: 作成した２次元配列を3×3の格子に分ける。

//step3: (0,1),(0,2),(0,3)の領域内にランダムに1に変える

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

return 0;

}
