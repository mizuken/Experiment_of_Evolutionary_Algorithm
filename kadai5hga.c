//////////////////情報メディア実験B 進化的アルゴリズム///////////////////////////

/*
・実験の注意１
manabaでダウンロードできる実験５の説明に載っているＩＨＣの探索回数には誤りが
あります。
説明には、
ＩＨＣの探索回数＝山登りの回数×再スタート回数
とありますが、実際にはこれの２から３倍あります。

今回の実験では、説明にあるままの数値で行ってもよいですが、
できるだけ正しい値を用いてください。

・実験の注意２
ＨＧＡでは、ＧＡとＨＣが混ざっていますが、
ＨＣで出た解はＧＡに戻さないほうが一般的によい結果になります。
ＨＣのフェーズで正解が出なかったら、コピーする前の解を使ってＧＡを実行してく
ださい。
（なぜそうなるのかを考えてみても面白いかもしれません）

これまでの実験の集大成ともいえる実験なので、
期間は短めですが、
ぜひ意欲的に考察してください。
*/
///////////////////////////実験5 HGA//////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/////////////////////////////各種変数定義////////////////////////////////////

#define N 90		//nの設定
#define MOM 100 	//親個体の設定
#define LANK 100	//世代ループ数の設定
#define M 1 		//疎・密問題の決定 0:密結合問題 1:疎結合問題
#define SCALE 0		//線形スケーリング:0 べき乗スケーリング:1
#define ALPHA 2     //べき乗スケーリングの指数
#define HAPPEN 1    //突然変異率
#define NE 10    //上位に残す個数
#define HC 100  //山登り回数

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
/*
FILE *fp;
	fp = fopen("recoad5hga.csv","w");
	if(fp == NULL){
	printf("Can't open flie\n");
	return 1;
	}
	fprintf(fp,"LANK,MAX,MIN,AVG\n");
*/
	//seedの設定 (現在時刻をseedとして取得してランダムに値を変える)
	srand((unsigned) time(NULL));

/////////////////////////////////問題を生成する///////////////////////////////////
int test;
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
	int count_link = 0; //リンクの本数を数える;
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
	double max_fitness_hc; //ヒルクライム中適応度最大値

	double max_fitness_result = 0.0; //実験結果中の適応度最大値

	int count; //個体番号を求める
	double dart1,dart2; //個体番号を求める
	int oya1,oya2;
	double all_p = 0; //確率の和
	double temp = 0;

	int mask[N]; //マスクビット作成
	int A,Z,hap;

	int tansaku=0; //探索回数
	int count_all=0;


//山登り変数

	int hc_violation = 0; //山登り用の違反点数
	int hc,hc_i,hc_j; //山登り用の変数
	int hc_challenge = 0; //山登り回数
	int hc_solution[N]; //解の登録をしておく
	int hc_number,hc_location,hc_point=0;
	int hc_violation_1 = 0, hc_violation_2 = 0; //違反点数記録
	int num1,num2; //変更値
	int hc_A;
	int hc_ne[N]; //解候補保存しておく

/*二次元配列を0で初期化する*/
for (i = 0; i < N ; i++){
	for (j = 0; j < N ; j++){
		graph[i][j] = 0; //０を代入して初期化する
		}
	}

//m本のリンクを貼るまで配列の数字を１に変更していく
while( count_link < m){
		//乱数を三つとり、どの領域、そして２次元座標を取得する
		area  = (int)((double)3*rand()/(RAND_MAX+1.0)); //0,1,2の数字を取得
		p = (int)((double)num*rand()/(RAND_MAX+1.0));
		q = (int)((double)num*rand()/(RAND_MAX+1.0));
		switch(area){
		case 0: //(0,1)のエリアに１を配置
		if(graph[p][num+q] != 1){
			graph[p][num+q] = 1;
			count_link++;
		}
		break;
		case 1: //(0,2)のエリアに１を配置
		if(graph[p][2*num+q] != 1){
			graph[p][2*num+q] = 1;
			count_link++;
		}
		break;
		case 2: //(1,2)のエリアに１を配置
		if(graph[num+p][2*num+q] != 1){
			graph[num+p][2*num+q] = 1;
			count_link++;
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
chrom ans; //解が見つかったらそれを保存する
chrom ne[NE];

///////////////MOM個の初期集団を生成する//////////////////////
for(i = 0; i < MOM; i++){
make_problem(&mom[i],graph,m);
}

////////////////初期集団の適応度の計算////////////////////////

for(j = 0 ; j < MOM; j++){

	violation = 0; //違反点数の初期化		
	for (s = 0; s < N ; s++){
		for (t = s+1; t < N ; t++){
			if(graph[s][t] == 1 && mom[j].data[s] == mom[j].data[t]){
				violation++; //違反点数を増やす
			}
		}
	}	
	mom[j].violation = violation; //違反点数を記録する
	mom[j].fitness = (double)mom[j].violation/m;
	mom[j].fitness = 1 - mom[j].fitness; //適応度を記録する
	all_fitness += mom[j].fitness; //全個体の適応度の合計
	if(violation == 0){
		ans_num = j; //解の個体番号の記録
	}
} //全個体の適応度を記録した

fitness_data(mom,MOM,&max,&min,&avg); //各種データを取得する
max_fitness = max;
min_fitness = min;
avg_fitness = avg;

////////////////世代:LANK分だけループを回す///////////////////////

for(i = 0; i < LANK; i++){

///////////////////遺伝的演算(選択：交叉：突然変異)//////////////////////

///////////////集団サイズ分回して次世代を生成する//////////////////////

	for(j = 0; j < MOM; j++){

///////////////////////////選択///////////////////////////////////

//スケーリングを行う
		
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

//////////////////////親世代へと残すものを決める////////////////////////

/////////////////////ルーレット戦略による個体の決定/////////////////////
	//printf("i=%d ルーレット\n",i);
		
		all_p = 0; //確率の和
		temp = 0;
		for (k = 0; k < MOM; k++){
			mom[k].p = mom[k].scale_fitness / all_scale_fitness;
			all_p += mom[k].p; //確率の総和
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

/////////////////////////////交叉//////////////////////////////

		//マスクビットの作成
		
		for(k = 0; k < N; k++){
			A = (int)((double)2*rand()/(RAND_MAX+1.0));
			mask[k] = A; //0,1のどちらかが入る
		}

		//一様交叉を行う
		for(k = 0; k < N; k++){
			if(mask[k] == 0){
				son[j].data[k] = mom[oya1].data[k];
			}else if(mask[k] == 1){
				son[j].data[k] = mom[oya2].data[k];
			}else{
				//エラーチェック
				printf("ERROR\n");
			}
		}

		//////////////子世代に残す////////////////////

		//一定確率で突然変異させる
		
		for(k = 0; k < N; k++){
			hap = (int)((double)1000*rand()/(RAND_MAX+1.0)); //確率のために取る
			//発生確率 HAPPEN:1 = 0.1% HAPPEN:100 = 10%
				if(hap < HAPPEN){
				//c_tempのk番目の遺伝子座が突然変異する
					Z = (int)((double)2*rand()/(RAND_MAX+1.0)); //変更値
					switch(son[j].data[k]){
					case 0:
					if(Z == 0){
						son[j].data[k] = 1;
					}else{
						son[j].data[k] = 2;
					}
					break;
					case 1:
					if(Z == 0){
						son[j].data[k] = 0;
					}else{
						son[j].data[k] = 2;
					}
					break;
					case 2:
					if(Z == 0){
						son[j].data[k] = 0;
					}else{
						son[j].data[k] = 1;
					}
					break;
				}
			}
		}

	} //集団サイズループ終了

//生成された集団の適応度の計算

	///////////////新しい集団に対して適応度を計算する/////////////////////
		for(j = 0 ; j < MOM; j++){
			violation = 0; //違反点数の初期化		
			for (s = 0; s < N ; s++){
				for (t = s+1; t < N ; t++){
					if(graph[s][t] == 1 && son[j].data[s] == son[j].data[t]){
						violation++; //違反点数を増やす
					}
				}
			}
			count_all++;	
			son[j].violation = violation; //違反点数を記録する
			son[j].fitness = (double)son[j].violation/m;
			son[j].fitness = 1 - son[j].fitness; //適応度を記録する
			all_fitness += son[j].fitness; //全個体の適応度の合計
			
			if(violation == 0){
				ans_num = j; //解の個体番号の記録
			}

		} //////////////////全個体の適応度を記録した/////////////////////////

		fitness_data(son,MOM,&max,&min,&avg); //各種データを取得する
		max_fitness = max;
		min_fitness = min;
		avg_fitness = avg;

		if(max_fitness == 1.0){
			printf("発見した！\n");
			ok = 1; //探索終了の目標
			ans_lank = i;
			ans = mom[ans_num];
			i = LANK;
			j = MOM;
		}

		////生成された集団を次世代集団とする/////

		for(k = 0; k < MOM; k++){
			mom[k] = son[k];
		}
		
		//順番を変える//////
		qsort(son, MOM, sizeof(chrom), vio_sort);

		//上位Ne個だけ保存する
		for(k = 0; k < NE; k++){
			ne[k] = son[k];
		}

/////////////////////////山登りを開始する//////////////////////////////////////


	for(hc = 0; hc < NE; hc++){
		//それぞれで山登りを行う.HCまで山登り
		for(hc_i=0; hc_i < N; hc_i++){
				hc_ne[hc_i] = ne[hc].data[hc_i];
			}

		hc_challenge = 0;
		while(hc_challenge < HC){
			////////解の初期化////////
			for(hc_i=0; hc_i < N; hc_i++){
				hc_solution[hc_i] = 0;
			}

			//違反点数を計算する
			hc_violation = 0;

			for(hc_i = 0; hc_i < N; hc_i++){
				for(hc_j = hc_i+1; hc_j < N; hc_j++){
					if(graph[hc_i][hc_j] == 1 && hc_ne[hc_i] == hc_ne[hc_j]){
						hc_violation++; //違反点数を数える
						hc_solution[hc_i] = 1; //違反箇所記録
						hc_solution[hc_j] = 1;
					}
				}
			}
			count_all++;
			ne[hc].violation = hc_violation; //違反点数を記録する
			ne[hc].fitness = (double)ne[hc].violation/m;
			ne[hc].fitness = 1 - ne[hc].fitness; //適応度を記録する
			
			//違反点数が0ならそこで探索終了
			if(hc_violation == 0){
				printf("山登り成功しました\n");
				printf("第%d世代 ne = %d 山登り%d回目 fitness = %f \n",i,hc,hc_challenge,ne[hc].fitness);
				max_fitness = ne[hc].fitness;
				hc = NE;
				ans_lank = i;
				i = LANK;
				ok = 1;
				break;
			}

			//違反箇所をランダムに取得する
			hc_point=0;
			while(hc_point == 0){
				hc_location = (int)((double)N*rand()/(RAND_MAX+1.0));
				
			//取得した値が違反箇所ならばそこを変数として選択する
				if(hc_solution[hc_location] == 1){
					//hc_number = ne[hc].data[hc_location]; //そのまま
					hc_number = hc_ne[hc_location]; //一度保存ver
					hc_point = 1;
				}
			}
			
			//値の選択
			
			//switch(ne[hc].data[hc_location]){
			switch(hc_ne[hc_location]){	 //一度保存ver
					case 0:
					//1,2でどちらが違反しているか考える
					num1 = 1;
					num2 = 2;
					break;
					case 1:
					//0,2でどちらが違反しているか考える
					num1 = 0;
					num2 = 2;
					break;
					case 2:
					//0,1でどちらが違反しているか考える
					num1 = 0;
					num2 = 1;
					break;
					default:
					break;
				}

				//２つの色の場合について違反数を数え少ないものを採用する
					hc_violation_1 = 0;
					hc_violation_2 = 0;

					//まず一つ目の場合について考える
					hc_ne[hc_location] = num1;
					for(hc_i = 0; hc_i < N; hc_i++){
						for(hc_j = hc_i+1; hc_j < N; hc_j++){
							if(graph[hc_i][hc_j] == 1 && hc_ne[hc_i] == hc_ne[hc_j]){
								hc_violation_1++; //違反点数を数える
							}
						}
					}
					count_all++;

					//次に二つ目について考える
					
					hc_ne[hc_location] = num2;
					for(hc_i = 0; hc_i < N; hc_i++){
						for(hc_j = hc_i+1; hc_j < N; hc_j++){
							if(graph[hc_i][hc_j] == 1 && hc_ne[hc_i] == hc_ne[hc_j]){
								hc_violation_2++; //違反点数を数える
							}
						}
					}
					count_all++;

			//数の比較をして違反点数が少なくなるように入れ替える
		if(hc_violation < hc_violation_1 && hc_violation < hc_violation_2){
			hc_ne[hc_location] = hc_number; //元の値を再代入 
		}else

		 if(hc_violation_1 == hc_violation_2){
			//数が一緒ならランダムに決める
			hc_A = (int)((double)2*rand()/(RAND_MAX+1.0));
			if(A == 0){
				hc_ne[hc_location] = num1;
			}else{
				hc_ne[hc_location] = num2;
			}
		}else{
		 	if(hc_violation_1 < hc_violation_2){
				hc_ne[hc_location] = num1;
			}else if(hc_violation_1 > hc_violation_2){
				hc_ne[hc_location] = num2;
			}else{
				//
			}	
		}
		hc_challenge++;

		} //一個体分の散策while終了
		
	} //上位NE分のHCのループ終了

}//LANKのループはここまで

//最終世代最大値
max_fitness_result = max_fitness;

///////////////////////結果判定//////////////////////////
if(ok == 1){
	//解が見つかった場合
	printf("探索が終了しました\n");
	printf("lank = %d max_fitness_result = %f\n",ans_lank,max_fitness_result);
	printf("課題5 回数 = %d\n",count_all);
	printf("\n");
	}else{
	//解が見つからなかった場合
	printf("解を発見できませんでした\n");
	printf("max_fitness_result = %f\n",max_fitness_result);
	printf("課題5 回数 = %d\n",count_all);
}

//fclose(fp);
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

////////////////////////ソートする///////////////////////////////////////////////////

int vio_sort(const void *a, const void *b){

	chrom vio1 = *(chrom *)a;
	chrom vio2 = *(chrom *)b;
	int tmp1 = vio1.violation; 
	int tmp2 = vio2.violation;
	return tmp1 - tmp2;
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
