//巡回セールスマン問題//

//実験9

//初期定義
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define ALPHA 1
#define BETA 2
#define RHO 0.1
#define RHOBEST 0.01
#define BEST 426

//構造体で地点データを保存する
typedef struct {
int number; //地点番号
double x_point; //x地点座標
double y_point; //y地点座標
int length; //この都市から始まる総距離
}point;


int main(int argc, char *argv[]){

//コマンドライン上の問題ファイルを読み込む

	FILE *fp;
	fp = fopen(argv[1],"r");
	if(fp==NULL){
		printf("can't read file\n");
		return -1;
		}
	printf("load_file_name: %s \n",argv[1]);

//ファイルの各種データを読み込む

//seedの設定 (現在時刻をseedとして取得してランダムに値を変える)
 srand((unsigned) time(NULL));

//変数定義
	char temp1[80],temp2[80]; //一時保存のための文字列変数
	char name[20]; //問題名
	int dimension; //地点数
	int i,j,s,t,k; //forループ用の変数

	fscanf(fp,"%s %s %s\n",temp1,temp2,name);
	fgets(temp1,80,fp); //COMMENT文を飛ばす
	fgets(temp2,80,fp); //TYPE文を飛ばす
	//printf("%s | %s \n",temp1,temp2);
	fscanf(fp,"%s %s %d\n",temp1,temp2,&dimension);
	fgets(temp1,80,fp); //Edge文を飛ばす
	fgets(temp2,80,fp); //Node文を飛ばす
	//printf("%s | %s \n",temp1,temp2);

	printf("NAME:%s\n",name);
	printf("DIMENSION:%d\n",dimension);

/////////////////結果の出力////////////////////
	FILE *fp1;
	char *csv = "_9.csv";
	strcat(name,csv);
	fp1 = fopen(name,"w");
	if(fp1==NULL){
		printf("can't read file\n");
		return -1;
		}
	printf("save_file_name: %s_9.csv \n",argv[1]);


//距離データを読み込む

	point P[dimension]; //地点数だけ定義する
	for(i = 0; i < dimension; i++){
		fscanf(fp,"%d %lf %lf\n",&P[i].number,&P[i].x_point,&P[i].y_point); //各都市地点データを保存する
	}
	
//各都市i,j間の距離を計算する

	int distans[dimension][dimension]; //各都市[i][j]間の距離を保存する
	double reci_distans[dimension][dimension]; //各都市間の距離の逆数
	double x_distans,y_distans;
	for(i = 0; i < dimension; i++){
		for(j = 0; j < dimension; j++){
			x_distans = (P[i].x_point - P[j].x_point ) * (P[i].x_point - P[j].x_point );
			y_distans = (P[i].y_point - P[j].y_point ) * (P[i].y_point - P[j].y_point );
			distans[i][j] = (int)(sqrt(x_distans + y_distans) + 0.5); //ここで保存する
			reci_distans[i][j] = (double)1.0 / (double)distans[i][j]; //逆数の保存
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////////////

///////////////実験結果を考察するための変数////////////////////////
	int best_score_ant_count; //１試行内の最適値を持ったアリの数
	int best_score_ant_count_all; //２００回未更新時の最適値を持ったアリの数の平均値

	int analys_best_score[10]; //10回試行の最高値を保存
	double analys_best_score_average = 0; //10回試行の最高値の平均値を保存
	double analys_best_score_stde = 0; //10解試行の最高値の標準偏差を保存
	int analys_loop_count[10]; //10回試行の試行回数を保存
	double analys_loop_count_average = 0; //10回試行の試行回数の平均値を保存
	double analys_loop_count_stde = 0; //10回試行の試行回数の平均値を保存
	double analys_apper_best_score_p = 0; //最良解の出現確率



//10回繰り返す

int all_loop = 0;
for(all_loop = 0; all_loop < 10; all_loop++){
	printf("%d回目\n",all_loop+1);	
	int first_point; //最初の地点番号
	int now_point; //現在の地点番号
	int next_point; //次の地点番号
	int last_point; //最終地点番号 (=最初の地点番号)
	int count = 0; //解のためカウンター
	int temp_distans = 10000000; //距離の一時保存
	
	int check[dimension][dimension]; //解として使われているかチェックする 0:済 1:未済

	//実験計測のためのデータを作っておく
	int avg_root_score = 0; //閉回路長平均値

	//読み込んだデータを用いて実行
	for(i = 0; i < dimension; i++){
		
		/////////////////////NN法による解の生成///////////////////////

		//解の初期化	
		for(s = 0; s < dimension; s++){
			for(t = 0; t < dimension; t++){
				check[s][t] = 1;
				if(s == t){check[s][t] = 0;} //同じ地点に戻ることはない
			}
		}
		
		//最初の準備
		first_point = i; //最初の地点番号を保存する
		now_point = first_point; //最初の地点番号を始まりとして保存する
		last_point = first_point; //最後の地点番号として保存する
		count = 0; //カウントリセット
		P[first_point].length = 0; //距離の初期化
		

			for(k = 0; k < dimension-1; k++){

				count++; //次の位置にずらす
				temp_distans = 10000000; //distansの初期化
				//次の地点の決定をする(現在位置から最も近い位置にある都市を選択する)
				for(s = 0 ; s < dimension; s++){
					if(check[now_point][s] == 1){
						if(distans[now_point][s] < temp_distans){
							temp_distans = distans[now_point][s]; //一番距離が短いものを保存する
							next_point = s; //次の地点として保存する
						}
					}
				}

				//次の地点を保存して同様の操作を繰り返す
				P[first_point].length += distans[now_point][next_point]; //距離の保存
				//root[first_point][count] = next_point; //解の保存
				//その地点を今後使わないようにする
				for(t = 0; t < dimension; t++){
					check[now_point][t] = 0;
					check[t][now_point] = 0;
				}
				now_point = next_point; //次の地点を現在地点として繰り返す

			} //ここで地点決定のループ終了


		//閉回路にする
		P[first_point].length += distans[now_point][last_point]; //距離の保存
		
	}//ループ終了


//L^mの値を決定する
double Lm = 0;

for(i = 0; i < dimension; i++){
	avg_root_score += P[i].length;
}
Lm = (double)avg_root_score/(double)dimension;

//ここからアントコロニー最適化法
	
	//各種定義
	int ant = 25; //アリの数 課題9にて25に設定　
	double pheromone[dimension][dimension]; //[i][j]: ij間のフェロモン
	double first_pheromone; //初期化フェロモン
	double temp_pheromone; //一時的フェロモン
	int loop_count = 0; //最良解が更新されなかった回数。1000ならば終了
	int best_score = 1000000; //実行中の最良解
	int best_score_before = 1000000; //全開の最良解
	int city_visit[ant][dimension]; //アリの訪問都市をチェックする.1なら訪問済み
	int city_count; //訪問都市数 最大値はdimensionになる
	double temp_ant_city_p;
	double mother_p; //分母の値
	int total_count = 0; 

	int ant_first_point; //初期出発点
	int ant_now_point; //現在位置
	int ant_next_point; //次の都市
	int ant_root_temp[dimension+1]; //アリがたどった順回路
	int ant_root_distance[ant]; //k番目のアリのたどった順回路長
	double ant_root_reci_distance[ant];//順回路長の逆数
	int best_ant_num; //試行中のアリ番号

	int ant_root[ant][dimension+1]; //アリがたどった順回路

	//動的確保(メモリ足りない)
	double ***ant_city_p; 
	ant_city_p =(double***)malloc(sizeof(double**)*ant);
  	for(i=0;i<ant;i++){
    	ant_city_p[i]=(double**)malloc(sizeof(double*)*dimension);
    		for(j=0;j<dimension;j++){
     	 		ant_city_p[i][j]=(double*)malloc(sizeof(double)*dimension);
      				for(k=0;k<dimension;k++){
        				ant_city_p[i][j][k]=(int)malloc(sizeof(double));
     	 				}
    			}
  		}

	int ***ant_root_check;
  	ant_root_check =(int***)malloc(sizeof(int**)*ant);
  	for(i=0;i<ant;i++){
    	ant_root_check[i]=(int**)malloc(sizeof(int*)*dimension);
    		for(j=0;j<dimension;j++){
     	 		ant_root_check[i][j]=(int*)malloc(sizeof(int)*dimension);
      				for(k=0;k<dimension;k++){
        				ant_root_check[i][j][k]=(int)malloc(sizeof(int));
     	 				}
    			}
  		}

	double temp_p = 0;
	double target; //番号を決める

	///////////////////MMASの変数////////////////////////////////////	
	double max_pheromone; //フェロモン上限値
	double min_pheromone; //フェロモン下限値
	////////////////////////////////////////////////////////////////

	/////////////////局所探索用の変数/////////////////////////////////
	int root1; //i_i+1の距離
	int root2; //j_j+1の距離
	int root3; //i_jの距離
	int root4; //i+1_j+1の距離
	int root5 = 0; //比較する変数
	int root_temp; //一時記憶
	int i_change_point; //変更の座標i+1
	int j_change_point; //変更の座標j
	int before_score=0; //2opt前の長さ
	int after_score=0; //2opt後の長さ
	int change_score_before=0; //変更前の長さ
	int change_score_after=0; //変更後の長さ
	///////////////////////////////////////////////////////////////

	
	/////////////////////////////////////////////////////////////
	Lm = Lm * RHO;
	first_pheromone =(double)1.0/(double)Lm;
	//フェロモンの初期化
	for(i = 0; i < dimension; i++){
		for(j = 0; j < dimension; j++){
				pheromone[i][j] = first_pheromone; //初期化する
		}
	}
	max_pheromone = first_pheromone;

	//フェロモン下限値の設定

	double temp_root = 1.0/(double)dimension;
	double temp_root2 = (double)dimension/2.0;
	min_pheromone = 1.0 - pow(RHOBEST,temp_root);
	temp_pheromone = (temp_root2 - 1.0) * pow(RHOBEST,temp_root);
	min_pheromone = min_pheromone / temp_pheromone * max_pheromone;
	//printf("max_pheromone = %f min_pheromone = %f \n",max_pheromone,min_pheromone);
	
	//最良解が更新されなければ繰り返す	
	while(loop_count != 200){ //課題9にては200回

		//printf("loop_count = %d------------------------\n",loop_count);
		
		total_count++; //総カウント数

		for(k = 0; k < ant; k++){
			for(i = 0; i < dimension; i++){
				city_visit[k][i] = 0; //k番目の訪問都市関係の初期化
			}
		}

		//訪問都市変数の初期化
		for(k = 0; k < ant; k++){
			for(i = 0; i < dimension; i++){
				for(j = 0; j < dimension; j++){
					ant_root_check[k][i][j] = 0;
				}
			}		
		}
		
		//アリのループ
		for(k = 0; k < ant; k++){ 
			
			ant_root_distance[k] = 0; //長さの初期化
			ant_now_point = (int)((double)dimension*rand()/(RAND_MAX+1.0)); //出発点のランダム選択
			ant_first_point = ant_now_point; //出発点の保存
			city_visit[k][ant_now_point] = 1; //訪問都市をチェックする 	
			
			//道を一度初期化しておく
			for(i = 0; i < dimension; i++){
				ant_root_temp[i] = 0;
				ant_root[k][i] = 0;
			}

			//出発点の記録
			//ant_root_temp[0] = ant_now_point;
			ant_root[k][0] = ant_now_point;
			
			//都市のループ (順回路生成)
			
			for(i = 0; i < dimension; i++){						

				//分母の計算 (行ってない都市のフェロモンと距離の逆数の積の総和)
				mother_p = 0;
	
				for(j = 0; j < dimension; j++){
					if(city_visit[k][j] == 0){
							mother_p += (pow(pheromone[ant_now_point][j],ALPHA) * pow(reci_distans[ant_now_point][j],BETA));
						}
				}

				//未訪問都市を用いて確率の決定
				for(j = 0; j < dimension; j++){
					
					//0は未訪問.1は訪問済み
					if(city_visit[k][j] == 0){
						temp_ant_city_p = pow(pheromone[ant_now_point][j],ALPHA) * pow(reci_distans[ant_now_point][j],BETA);
						ant_city_p[k][ant_now_point][j] = temp_ant_city_p / mother_p;
					}else{
						ant_city_p[k][ant_now_point][j] = 0.0; //otherwise
					}

				} //次の都市の確率計算ループ

				//もう一度確率計算のためにmothre_p再定義
				mother_p = 0;
				for(j = 0 ; j < dimension ; j++){
					mother_p += ant_city_p[k][ant_now_point][j];
				}

					//算出した確率を用いて次の地点の決定
					temp_p = 0;
					target = ((double)mother_p*rand()/(RAND_MAX+1.0));
					for(s = 0; s < dimension; s++){
						if( ant_city_p[k][ant_now_point][s] != 0){
							temp_p += ant_city_p[k][ant_now_point][s];
							if(temp_p >= target){
								ant_next_point = s; //次期地点の保存
								s = dimension; //ループ終了
							}
						}
					}
				
				//ルートの保存	
				//ant_root_temp[i+1] = ant_next_point; //位置を保存する
				ant_root[k][i+1] = ant_next_point; //位置を保存する(課題9)
				//距離の保存
				ant_root_check[k][ant_now_point][ant_next_point] = 1; //道順の保存
				//これでant_next_pointが次の地点としてループする
				ant_now_point = ant_next_point;
				city_visit[k][ant_now_point] = 1; //訪問都市をチェックする 

				
			} //順回路のループ

			//閉回路にしておく
			
			ant_root[k][dimension] = ant_first_point; //課題9
			for(j = 0; j < dimension; j++){
				ant_root_distance[k] += distans[ant_root[k][j]][ant_root[k][j+1]];
			}
		
			change_score_after = ant_root_distance[k];

			//printf("k = %2d before = %d ",k+1,change_score_after);
			//局所探索
		
			do { //前回よりも改善してるうちは繰り返し
				total_count++;
				//各値の初期化
				root1 = 0;
				root2 = 0;
				root3 = 0;
				root4 = 0;
				root5 = 0;
				i_change_point = 0;
				j_change_point = 0;
				change_score_before = change_score_after;

				//最も改善される組み合わせを決定する (best-option)
				for(s = 0; s < dimension; s++){
					for(t = 0; t < dimension; t++){
						if(s != t && s != t+1 && s+1 != t && s+1 < t){
							root1 = distans[ant_root[k][s]][ant_root[k][s+1]];
							root2 = distans[ant_root[k][t]][ant_root[k][t+1]];
							root3 = distans[ant_root[k][s]][ant_root[k][t]];
							root4 = distans[ant_root[k][s+1]][ant_root[k][t+1]];
							//距離が短くなるならその間の道を逆順にする
							if(root5 < ((root1+root2) - (root3+root4))){
								//printf("%d < %d - %d \n",root3,root2,root1);
								root5  = ((root1+root2) - (root3+root4));
								i_change_point = s+1; //座標の記録
								j_change_point = t; //座標の記録

							}
						}
					}
				} //2-optのループ

				//i_change_pointからj_change_pointまでの道順を逆にする

				//まず元の道順を一時的に保存する
				int reverse_temp[dimension]; //一時的配列
				for(s = 0; s < dimension; s++){
					reverse_temp[s] = 0; //初期化
				}
				//一時的配列に保存
				for(s = 0; s < j_change_point - i_change_point + 1; s++){
					reverse_temp[s] = ant_root[k][i_change_point + s];
				}
				//これを逆順に挿入していく
				for(s = 0; s < j_change_point - i_change_point + 1; s++){
					ant_root[k][j_change_point - s] = reverse_temp[s];
				} 
				change_score_after = 0;
				//改善後の距離の計算
				for(s = 0; s < dimension; s++){
					change_score_after += distans[ant_root[k][s]][ant_root[k][s+1]];
				}

				//////////////////////////////////////////////////////////////

				after_score = change_score_before; //終了ならばこれが改善後なのでこれになる

			} while(change_score_before > change_score_after); //dowhile分の終了

			//解の保存
			ant_root_distance[k] = after_score;
			ant_root_reci_distance[k] = (double)1.0 / (double)ant_root_distance[k];

			//printf("after = %d \n",ant_root_distance[k]);

		} //アリのループ

		//最良解の保存
		best_score = 10000000;
		best_ant_num = 0;
		for(k = 0; k < ant; k++){
			if(ant_root_distance[k] < best_score){
				best_score = ant_root_distance[k];
				best_ant_num = k;
			}
		}

		/////////////////////パラメータ評価///////////////////////
			if(loop_count == 0){
					best_score_ant_count_all = 0;
				}

			best_score_ant_count = 0;
			for(k = 0; k < ant; k++){
				if(ant_root_distance[k] == BEST){
					best_score_ant_count++; //最適解を何個持てたか
				}
			}
			best_score_ant_count_all += best_score_ant_count;
		///////////////////////////////////////////////////////

		//printf("best_ant_num = %d\n",best_ant_num);
		//前回の最良解との比較し更新するかどうか考える
		if(best_score < best_score_before){
			loop_count = 0;
			printf("%d < %d \n",best_score, best_score_before );
			best_score_before = best_score;

		}else{
			loop_count++;
		}

		//最良解の記録
		fprintf(fp1,"%d\n",best_score_before);
		
		//フェロモンの更新(ij=ji)
		for(i = 0; i < dimension; i++){
			for(j = i; j < dimension; j++){
				//最良回のアリが通ったルートかどうかを見る
				if(ant_root_check[best_ant_num][i][j] == 1 || ant_root_check[best_ant_num][j][i] == 1){
					
					temp_pheromone = (1.0 - RHO)*pheromone[i][j] + (double)1.0/(double)best_score;
					if( temp_pheromone > max_pheromone){
						pheromone[i][j] = max_pheromone;
						pheromone[j][i] = max_pheromone;
					}else if( temp_pheromone < min_pheromone){
						pheromone[i][j] = min_pheromone;
						pheromone[j][i] = min_pheromone;	
					}else{
						pheromone[i][j] = temp_pheromone;
						pheromone[j][i] = temp_pheromone;
					}

					} else {

					temp_pheromone = (1.0 - RHO)*pheromone[i][j];
					if( temp_pheromone > max_pheromone){
						pheromone[i][j] = max_pheromone;
						pheromone[j][i] = max_pheromone;
					}else if( temp_pheromone < min_pheromone){
						pheromone[i][j] = min_pheromone;
						pheromone[j][i] = min_pheromone;	
					}else{
						pheromone[i][j] = temp_pheromone;
						pheromone[j][i] = temp_pheromone;
					}

				}
			}
		} //フェロモンの更新終了
	
	} //whileのループ

	//分析のために値を保存しておく
	analys_best_score[all_loop] = best_score_before;
	analys_loop_count[all_loop] = total_count;

	//メモリの解放
	for(i = 0; i < ant; i++){
		for(j = 0; j < dimension; j++){
			free(ant_city_p[i][j]);
			free(ant_root_check[i][j]);
		}
		free(ant_city_p[i]);
		free(ant_root_check[i]);
	}
	free(ant_city_p);
	free(ant_root_check);


} //all_loopの終了

//分析する
int analys_temp_int = 0;
double analys_temp_double = 0;
for(i = 0 ; i < 10; i++){
	analys_best_score_average += analys_best_score[i];
	analys_loop_count_average += analys_loop_count[i];
}

analys_best_score_average = analys_best_score_average/(double)10.0;
analys_loop_count_average = analys_loop_count_average/(double)10.0;


for(i = 0; i < 10; i++){
	analys_best_score_stde += (analys_best_score[i] - analys_best_score_average) * (analys_best_score[i] - analys_best_score_average);
	analys_loop_count_stde += (analys_loop_count[i] - analys_loop_count_average) * (analys_loop_count[i] - analys_loop_count_average); 
}
analys_best_score_stde = analys_best_score_stde/(double)10;
analys_best_score_stde = sqrt(analys_best_score_stde);
analys_loop_count_stde = analys_loop_count_stde/(double)10;
analys_loop_count_stde = sqrt(analys_loop_count_stde);
printf("----------------------------------------------------\n");
printf("%f/%f\n",RHO,RHOBEST);
printf("10回試行 最良解の平均値:%f\n",analys_best_score_average);
printf("10回試行 最良解の標準偏差:%f\n",analys_best_score_stde);
printf("10回試行 試行回数の平均値:%f\n",analys_loop_count_average);
printf("10回試行 試行回数の標準偏差:%f\n",analys_loop_count_stde);
/*
for(i = 0; i < 10; i++){
	printf("[%3d/%3d]\n",analys_best_score[i],analys_loop_count[i]);
}
*/
	fclose(fp); //ファイルのクローズ
	fclose(fp1);
	//////////////////////////////////////////////
	return 0;
}
