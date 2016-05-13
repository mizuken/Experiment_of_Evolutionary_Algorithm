//////////////////情報メディア実験B 進化的アルゴリズム///////////////////////////

///////////////////////////実験4 進化的戦略//////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/////////////////////////////各種変数定義////////////////////////////////////

#define N 150		//nの設定
#define MOM 100 	//親個体の設定
#define LANK 1000	//世代ループ数の設定
#define M 1		//疎・密問題の決定 0:密結合問題 1:疎結合問題
#define SCALE 0		//線形スケーリング:0 べき乗スケーリング:1
#define ALPHA 2     //べき乗スケーリングの指数
#define HAPPEN 1    //突然変異率

//構造体で親個体を定義する
typedef struct {
	int data[N]; 		//染色体
	int violation; 		//違反点数
	double fitness; 	//適応度
	double scale_fitness;  //スケーリング後の適応度
	double p; //選択確率
}chrom;

///////////////////////////////関数定義///////////////////////////////////////

void make_problem(chrom *n, int g[N][N],int m); //親個体を生成する関数
int vio_sort(const void *a, const void *b);//qsortするための関数
void fitness_data(chrom *n, int m, double *max,
						 double *min, double *avg); //違反点数の最小・最大・平均を計算する

int main(void){

///////////////////////////記録用ファイルの準備///////////////////////////////////
FILE *fp;
fp = fopen("recoad4ga.csv","w");
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

int i,j,k,l,s,t;
double max=0,min=2,avg=0; //最大値max・最小値min・平均値avg
int graph[N][N]; //問題を設定
int area,p,q; //ランダムな値をとる変数
int count = 0; //リンクの本数を数える;
int num = N/3;
int violation; //違反点数を保存
int ok = 0; //解が見つかれば1にして探索終了
int ans_lank; //解が出た時の世代数
int ans_num; //解が出来た時の個体番号
int ccc=0;

double all_fitness = 0.0; //全個体の適応度の合計
double all_scale_fitness = 0.0; //スケーリング後の全個体の適応度合計	
double max_fitness; //全個体中の適応度の最大値
double min_fitness; //全個体中の適応度の最小値
double avg_fitness; //全個体中の適応度の平均値

double max_fitness_result = 0.0; //実験結果中の適応度最大値

int count_all = 0;

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

chrom mom[MOM]; //#define MOMの設定分親個体を生成する
chrom son[MOM]; //#define 子の個体
chrom c_temp; //一時記憶
chrom ans; //解が見つかったらそれを保存する

	///////////////MOM個の親個体をランダムに生成する//////////////////////
	for(i = 0; i < MOM; i++){
		make_problem(&mom[i],graph,m);
	}

	////////////////世代:LANK分だけループを回す///////////////////////
	for(i = 0; i < LANK; i++){

		////////////////全個体の適応度を計算/////////////////////////////	
	
			for(j = 0 ; j < MOM; j++){

				violation = 0; //違反点数の初期化		
				for (s = 0; s < N ; s++){
					for (t = s+1; t < N ; t++){
						if(graph[s][t] == 1 && mom[j].data[s] == mom[j].data[t]){
							violation++; //違反点数を増やす
						}
					}
				}
				count_all++;	
				mom[j].violation = violation; //違反点数を記録する
				mom[j].fitness = (double)mom[j].violation/m;
				mom[j].fitness = 1 - mom[j].fitness; //適応度を記録する
				all_fitness += mom[j].fitness; //全個体の適応度の合計
				if(violation == 0){
					ans_num = j; //解の個体番号の記録
				}
				//printf("violation before = %d \n",violation );
			} //全個体の適応度を記録した

			fitness_data(mom,MOM,&max,&min,&avg); //各種データを取得する
			//printf("全個体の適応度の合計 = %f\n",all_fitness);
			max_fitness = max;
			min_fitness = min;
			avg_fitness = avg;
			//printf("i=%d max = %f min = %f avg_fitness = %f\n",i,max_fitness,min_fitness,avg_fitness);
			fprintf(fp,"%d,%f,%f,%f\n",i,max_fitness,min_fitness,avg_fitness);

			
			//適応度の最大値が1.0になった場合探索終了
			if(max_fitness == 1.0){
				ok = 1; //探索終了の目標
				ans_lank = i;
				i = LANK;
				j = MOM;
			}


	/////////////集団サイズ分回して次世代を生成する//////////////////////
		
			for(j = 0; j < MOM; j++){
	
	////////////////適応度に応じて個体を選択///////////////////////

	////////////////スケーリングを行う////////////////////////////
				
			if(SCALE == 0){ //線形スケーリング

				//スケーリングで使う値を設定する
				max_fitness = max;
				min_fitness = min;
				all_scale_fitness = 0;
				//全個体の値についてスケーリングを行う
				for(k = 0; k < MOM; k++){
					mom[k].scale_fitness = (mom[k].fitness - min_fitness) / (max_fitness - min_fitness);
					all_scale_fitness += mom[k].scale_fitness;
				}

			}else
			if(SCALE == 1){ //べき乗スケーリング
				double alp = 0;
				for(k = 0; k < MOM; k++){
					alp = mom[k].fitness;
					for(l = 1; l < ALPHA; l++){
						alp *= alp; //アルファ乗
					}
					mom[k].scale_fitness = alp;
					all_scale_fitness += mom[k].scale_fitness;
				}

			}else{}	

			//printf("all_scale_fitness = %f\n", all_scale_fitness );	
	//////////////////////親世代へと残すものを決める////////////////////////
	
	/////////////////////ルーレット戦略による個体の決定/////////////////////

		int count; //個体番号を求める
		double dart1,dart2; //個体番号を求める
		int oya1,oya2;
		double all_p = 0; //確率の和
		double temp = 0;

		for (k = 0; k < MOM; k++){
			mom[k].p = mom[k].scale_fitness / all_scale_fitness;
			all_p += mom[k].p; //確率の総和
			//p[k] = mom[k].p; //確率を保存する
		}
		
		//一つ目の親の決定
		dart1 = ((double)all_p*rand()/(RAND_MAX+1.0)); //場所を決める
		temp = 0;
		for(count = 0; count < MOM; count++){
			temp += mom[count].p;
			if(temp >= dart1){
				oya1 = count; //保存
				count = MOM; //ループの終了
			}
		}

		//二つ目の親の決定
		dart2 = ((double)all_p*rand()/(RAND_MAX+1.0)); //場所を決める
		temp = 0;
		for(count = 0; count < MOM; count++){
			temp += mom[count].p;
			if(temp >= dart2){
				oya2 = count; //保存
				count = MOM; //ループの終了
			}
		}

	////////////////////交叉で子個体の生成//////////////////////////////

		//マスクビットの作成
		int mask[N];
		for(k = 0; k < N; k++){
			int A = (int)((double)2*rand()/(RAND_MAX+1.0));
			mask[k] = A; //0,1のどちらかが入る
		}

		//一様交叉を行う
		for(k = 0; k < N; k++){
			if(mask[k] == 0){
				c_temp.data[k] = mom[oya1].data[k];
			}else if(mask[k] == 1){
				c_temp.data[k] = mom[oya2].data[k];
			}else{
				//エラーチェック
				printf("ERROR\n");
			}
		}

		//////////////子世代に残す////////////////////

		//一定確率で突然変異させる
		
		for(k = 0; k < N; k++){
			int hap = (int)((double)1000*rand()/(RAND_MAX+1.0)); //確率のために取る
			//printf("hap = %d\n",hap);
			//発生確率 HAPPEN:1 = 1% HAPPEN:100 = 100%
				if(hap < HAPPEN){
					//printf("%d突然変異!\n",ccc++);
				//c_tempのk番目の遺伝子座が突然変異する
					int Z = (int)((double)2*rand()/(RAND_MAX+1.0)); //変更値
					switch(c_temp.data[k]){
					case 0:
					if(Z == 0){
						c_temp.data[k] = 1;
					}else{
						c_temp.data[k] = 2;
					}
					break;
					case 1:
					if(Z == 0){
						c_temp.data[k] = 0;
					}else{
						c_temp.data[k] = 2;
					}
					break;
					case 2:
					if(Z == 0){
						c_temp.data[k] = 0;
					}else{
						c_temp.data[k] = 1;
					}
					break;
				}
			}
		}

		//子孫として保存する
		son[j] = c_temp;

		} //集団サイズループ終了

		////生成された集団を次世代集団とする/////
		for(k = 0; k < MOM; k++){
			mom[k] = son[k];
		}

	}//LANKのループはここまで

	//最終世代最大値
	max_fitness_result = max_fitness;

	///////////////////////結果判定//////////////////////////
	if(ok == 1){
		//解が見つかった場合
		printf("探索が終了しました\n");
		printf("lank = %d max_fitness_result = %f\n",ans_lank,max_fitness_result);
		printf("課題5 違反点数回数 = %d\n",count_all);
	}else{
		//解が見つからなかった場合
		printf("解を発見できませんでした\n");
		printf("max_fitness_result = %f\n",max_fitness_result);
		printf("課題5 違反点数回数 = %d\n",count_all);
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

//////////////////////違反点数に関するデータを取得する///////////////////////////////////

void fitness_data(chrom *n, int m, double *max, double *min, double *avg){

	int i,j;
	double s=0,t=2,r=0;
	double v=0;

	//最大値、最小値を取得する
		for(i = 0; i < m; i++){
			if(s < n[i].fitness){
				s = n[i].fitness;
			}
			if(t > n[i].fitness){
				t = n[i].fitness;
			}
			v = n[i].fitness;
			r += v;
		}

	 *max = s; //最大値の保存
	 *min = t; //最小値の保存
	 r = r/m;  
	 *avg = r; //平均値の保存

}
