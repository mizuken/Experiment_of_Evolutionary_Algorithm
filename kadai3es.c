//////////////////情報メディア実験B 進化的アルゴリズム///////////////////////////

///////////////////////////実験3 進化的戦略//////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/////////////////////////////各種変数定義////////////////////////////////////

#define N 120		//nの設定
#define MYU 100 	//μの設定
#define LAMDA 500 	//λの設定
#define LANK 200 	//世代ループ数の設定
#define ES 0 		//親世代決定方法 0:(μ,λ)-ES 1:(μ+λ)-ES
#define M 0 		//疎・密問題の決定 0:密結合問題 1:疎結合問題

//構造体で親個体を定義する
typedef struct {
	int data[N]; 		//染色体
	int violation; 		//違反点数
	double fitness; 	//適応度
}chrom;

///////////////////////////////関数定義///////////////////////////////////////

void make_problem(chrom *n, int g[N][N],int m); //親個体を生成する関数
int vio_sort(const void *a, const void *b);//qsortするための関数
void violation_data(chrom *n, int m, int *max,
						 int *min, int *avg); //違反点数の最小・最大・平均を計算する
//int myu_check(chrom *n, int num);

int main(void){

///////////////////////////記録用ファイルの準備///////////////////////////////////
FILE *fp;
fp = fopen("recoad3es.csv","w");
if(fp == NULL){
	printf("Can't open flie\n");
	return 1;
}
fprintf(fp,"LANK,MAX,MIN,AVG\n");

//seedの設定 (現在時刻をseedとして取得してランダムに値を変える)
 srand((unsigned) time(NULL));

/////////////////////////////////問題を生成する///////////////////////////////////

//密・疎の決定
int m = 0;
if(M == 0){
	m = N*(N-1)/4; //密結合問題に設定
}else
if(M == 1){
	m = 3*N;       //疎結合問題に設定
	}
else{}
int i,j,k,s,t,l;
int max=0,min=N*N,avg=0; //最大値max・最小値min・平均値avg
int graph[N][N]; //問題を設定
int area,p,q; //ランダムな値をとる変数
int count = 0; //リンクの本数を数える;
int num = N/3;
int violation; //違反点数を保存
int ok = 0; //解が見つかれば1にして探索終了
int count_all=0;

/*二次元配列を0で初期化する*/
for (i = 0; i < N ; i++){
	for (j = 0; j < N ; j++){
		graph[i][j] = 0; //０を代入して初期化する
	}
}

//m本のリンクを貼るまで配列の数字を１に変更していく
while( count < m){
	//乱数を三つとり、どの領域、そして２次元座標を取得する
	area  = (int)((double)3*rand()/(RAND_MAX+1.0)); //0,1,2の数字を取得
	p = (int)((double)num*rand()/(RAND_MAX+1.0));
	q = (int)((double)num*rand()/(RAND_MAX+1.0));
	switch(area){
		case 0: //(0,1)のエリアに１を配置
		if(graph[p][num+q] != 1){
			graph[p][num+q] = 1;
			count++;
		}
		break;
		case 1: //(0,2)のエリアに１を配置
		if(graph[p][2*num+q] != 1){
			graph[p][2*num+q] = 1;
			count++;
		}
		break;
		case 2: //(1,2)のエリアに１を配置
		if(graph[num+p][2*num+q] != 1){
			graph[num+p][2*num+q] = 1;
			count++;
		}
		break;
		default:
		break;
	}
}

///////////////////////////親個体を生成する///////////////////////////////////

///////////////////構造体を用いて実験用のデータを生成する/////////////////////////

chrom myu[MYU]; //μの設定分親個体を生成する
chrom lamda[LAMDA]; //λ分の子孫用の個体を生成する
chrom m_l[MYU+LAMDA]; //(μ+λ)-ES用の一度全て保存する
chrom ans; //解が見つかったらそれを保存する

	///////////////μ個の親個体をランダムに生成する//////////////////////
	for(i = 0; i < MYU; i++){
		make_problem(&myu[i],graph,m);
	}

	//////////////////問題の最初の値を記録////////////////////////////
	violation_data(myu,MYU,&max,&min,&avg);
	printf("LANK = 0 max = %d min = %d avg = %d\n",max,min,avg);
	fprintf(fp, "%d,%d,%d,%d\n",0,max,min,avg );
	
	////////////////世代:LANK分だけループを回す///////////////////////
	for(i = 0; i < LANK; i++){
	
	/////////////λ分だけループを回して小集団を生成//////////////////////
		for(j = 0; j < LAMDA; j++){
	
	//////////////親集団からランダムに1個体を選択する///////////////////
			int A = (int)((double)MYU*rand()/(RAND_MAX+1.0));
	
	/////////一度何もせず子世代にコピーする.(親世代を残すため)////////////
			lamda[j] = myu[A];

	/////突然変異を起こし、ランダムに遺伝子座を選択して対立遺伝子を変更する////

			int X = (int)((double)N*rand()/(RAND_MAX+1.0)); //遺伝子座
			int R = (int)((double)2*rand()/(RAND_MAX+1.0)); //変更値
			switch(lamda[j].data[X]){
				case 0:
				if(R == 0){
					lamda[j].data[X] = 1;
				}else{
					lamda[j].data[X] = 2;
				}
				break;
				case 1:
				if(R == 1){
					lamda[j].data[X] = 0;
				}else{
					lamda[j].data[X] = 2;
				}
				break;
				case 2:
				if(R == 2){
					lamda[j].data[X] = 0;
				}else{
					lamda[j].data[X] = 1;
				}
				break;
			}
	
	//////////////////////違反点数を計算する//////////////////////////////
			violation = 0;		
			for (s = 0; s < N ; s++){
				for (t = s+1; t < N ; t++){
					if(graph[s][t] == 1 && lamda[j].data[s] == lamda[j].data[t]){
						violation++; //違反点数を増やす
					}
				}
			}
			count_all++;
			lamda[j].violation = violation; //違反点数を記録する
	
	///////////////////////適応度を計算する///////////////////////////////	
	
			lamda[j].fitness = (double)lamda[j].violation/m;
			lamda[j].fitness = 1 - lamda[j].fitness;
		}

	//////////////////////親世代へと残すものを決める////////////////////////
	
		if(ES == 0){   //(μ,λ)-ESによる親世代決定

			//違反点数の低い順に並び変える
			qsort(lamda, LAMDA, sizeof(chrom), vio_sort);
		
			//並び替えたのち上位μ分を次の親世代として残す
			for(k = 0; k < MYU; k++){
				myu[k] = lamda[k];
			}

		}else

		if(ES == 1){    //(μ+λ)-ESによる親世代決定
			
			//一度親と子を合わせたものを作る
			for(k = 0; k < MYU; k++){
				m_l[k] = myu[k];
			}
			for(k = MYU; k < MYU+LAMDA; k++){
				m_l[k] = lamda[k];
			}
			//合わせたもので並び替える
			qsort(m_l, LAMDA, sizeof(chrom), vio_sort);

			//並び替えたのち上位μ分を次の親世代として残す
			for(k = 0; k < MYU; k++){
				myu[k] = m_l[k];
			}

		}else{
			//エラー処理
		}

		////////////////実験結果を計算して表示する///////////////////////////
		violation_data(myu,MYU,&max,&min,&avg);
		//printf("LANK = %d max = %d min = %d avg = %d\n",i+1,max,min,avg);
		fprintf(fp, "%d,%d,%d,%d\n",i+1,max,min,avg ); //ファイルに記録する
		
		//////////////ここで解がなかったかどうか確認する//////////////////////

		for(l = 0; l < MYU; l++){
			if(myu[l].violation == 0){ //違反点数が0だったらそらが解となり終了
				
				printf("解を発見しました\n");
				ans = myu[l]; //解の保存
				//break処理
				printf("lank = %d\n",i);
				ok = 1;
				l = MYU;
				i = LANK;
				j = LAMDA;
			
			}
		}

	}//LANKのループはここまで

	///////////////////////解を表示する//////////////////////////
	
	if(ok == 1){
		printf("解が発見されました\n");
		printf("課題5 総違反点数計測回数 = %d\n",count_all);
		/*
		for(i = 0; i < N; i++){
			printf(" %d",ans.data[i]);
		}
		printf("\n");
		*/
	}else{
		printf("解は発見されませんでした\n");
		printf("課題5 総違反点数計測回数 = %d\n",count_all);
		printf("\n");
	}

	fclose(fp);
	return 0;
}

///////////１親個体についてランダムに生成する関数 make_problem///////////////////////////

void make_problem(chrom *n, int g[N][N],int m){

 int i,j; // 初期化するforループ用の変数
 int D; //解候補
 int violation = 0; //違反点数

	//対立遺伝子座の設定
	for (i = 0; i < N ; i++){
		D = (int)((double)3*rand()/(RAND_MAX+1.0)); //赤緑青の色をランダムに取得
		n->data[i] = D; //解候補の代入
	}

	for (i = 0; i < N ; i++){
			for (j = i+1; j < N ; j++){
				if(g[i][j] == 1 && n->data[i] == n->data[j]){
					violation++; //違反点数を増やす
				}
			}
		}
	n->violation = violation;
	//適応度を計算する	
	n->fitness = (double)n->violation/m;
	n->fitness = 1 - n->fitness;
}

//////////////sort用に関数を定義する vio_sort///////////////////////////////////////

int vio_sort(const void *a, const void *b){
	 chrom vio1 = *(chrom *)a;
  	 chrom vio2 = *(chrom *)b;
	 int tmp1 = vio1.violation; 
  	 int tmp2 = vio2.violation;
  return tmp1 - tmp2;
}

//////////////////////違反点数に関するデータを取得する///////////////////////////////////

void violation_data(chrom *n, int m, int *max, int *min, int *avg){

int i,j;
int s=0,t=N*N,r=0;
int v=0;

	//最大値、最小値を取得する
		for(i = 0; i < m; i++){
			if(s < n[i].violation){
				s = n[i].violation;
			}
			if(t > n[i].violation){
				t = n[i].violation;
			}
			v = n[i].violation;
			r += v;
		}

	 *max = s; //最大値の保存
	 *min = t; //最小値の保存
	 r = r/m;  
	 *avg = r; //平均値の保存

}
