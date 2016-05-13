//巡回セールスマン問題//

//実験6

//初期定義
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

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

//変数定義
	char temp1[100],temp2[100]; //一時保存のための文字列変数
	char name[20]; //問題名
	int dimension; //地点数
	int i,j,s,t,k; //forループ用の変数

	fscanf(fp,"%s %s %s\n",temp1,temp2,name);
	fgets(temp1,200,fp); //COMMENT文を飛ばす
	fgets(temp1,200,fp); //TYPE文を飛ばす
	fscanf(fp,"%s %s %d\n",temp1,temp2,&dimension);
	fgets(temp1,200,fp); //Edge文を飛ばす
	fgets(temp1,200,fp); //Node文を飛ばす

	printf("NAME:%s\n",name);
	printf("DIMENSION:%d\n",dimension);

//距離データを読み込む

	point P[dimension]; //地点数だけ定義する
	for(i = 0; i < dimension; i++){
		fscanf(fp,"%d %lf %lf\n",&P[i].number,&P[i].x_point,&P[i].y_point); //各都市地点データを保存する
	}
	
//各都市i,j間の距離を計算する

	int distans[dimension][dimension]; //各都市[i][j]間の距離を保存する
	double x_distans,y_distans;
	for(i = 0; i < dimension; i++){
		for(j = 0; j < dimension; j++){
			x_distans = (P[i].x_point - P[j].x_point ) * (P[i].x_point - P[j].x_point );
			y_distans = (P[i].y_point - P[j].y_point ) * (P[i].y_point - P[j].y_point );
			distans[i][j] = (int)(sqrt(x_distans + y_distans) + 0.5); //ここで保存する
		}
	}

//順回路を計算する

	int first_point; //最初の地点番号
	int now_point; //現在の地点番号
	int next_point; //次の地点番号
	int last_point; //最終地点番号 (=最初の地点番号)
	int count = 0; //解のためカウンター
	int root[dimension][dimension+1]; //順路解の保存[最初の地点番号][解の順]
	int temp_distans = 10000000; //距離の一時保存
	
	int check[dimension][dimension]; //解として使われているかチェックする 0:済 1:未済

	//実験計測のためのデータを作っておく
	int max_root_score = 0; //閉回路長最大値
	int min_root_score = 10000000; //閉回路長最小値
	int avg_root_score = 0; //閉回路長平均値
	double imp_late[dimension]; //改善率 何%距離を少なくできたかの指標

	int test1=0,test2,test3; //デバッグ用変数

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
		root[first_point][count] = now_point; //出発点の保存
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
			root[first_point][count] = next_point; //解の保存
			//その地点を今後使わないようにする
			for(t = 0; t < dimension; t++){
				check[now_point][t] = 0;
				check[t][now_point] = 0;
			}
			now_point = next_point; //次の地点を現在地点として繰り返す

		} //ここで地点決定のループ終了


		//閉回路にする
		
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

		root[first_point][dimension] = last_point; //閉回路にする
		P[first_point].length += distans[now_point][last_point]; //距離の保存
		before_score = P[first_point].length; //保存
		
		for(s = 0; s < dimension; s++){
				change_score_after += distans[root[i][s]][root[i][s+1]];
			}
		
		//2-0ptによる解の改善をする
		//改善されるまで繰り返す
		//change_score_after = P[first_point].length; //最初の一回のため

		do { //前回よりも改善してるうちは繰り返し
			//各値の初期化
			P[first_point].length = 0;
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
						root1 = distans[root[i][s]][root[i][s+1]];
						root2 = distans[root[i][t]][root[i][t+1]];
						root3 = distans[root[i][s]][root[i][t]];
						root4 = distans[root[i][s+1]][root[i][t+1]];
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
				reverse_temp[s] = root[i][i_change_point + s];
			}
			//これを逆順に挿入していく
			for(s = 0; s < j_change_point - i_change_point + 1; s++){
				root[i][j_change_point - s] = reverse_temp[s];
			} 
			change_score_after = 0;
			//改善後の距離の計算
			for(s = 0; s < dimension; s++){
				change_score_after += distans[root[i][s]][root[i][s+1]];
			}

			//////////////////////////////////////////////////////////////

			after_score = change_score_before; //終了ならばこれが改善後なのでこれになる

		} while(change_score_before > change_score_after); //dowhile分の終了

		P[first_point].length = after_score;

		//計測結果のための処理をする

		if(max_root_score < after_score) max_root_score = after_score; //最大値判定
		if(min_root_score > after_score) min_root_score = after_score; //最小値判定
		avg_root_score += after_score; //のちに平均値に使う
		imp_late[i] = (1.00000 - ((double)after_score / (double)before_score) ) * 100 ; //改善率の計算

	} //解のループ終了

	avg_root_score = avg_root_score / dimension; //平均値
	
	//標準偏差を求める
	double V = 0; //分散
	double SD = 0; //標準偏差
	for(i = 0; i < dimension; i++){
		V += (P[i].length - avg_root_score) * (P[i].length - avg_root_score);
	}
	V = V / dimension;
	SD = sqrt(V);

	//改善率の算出
	double imp_late_avg = 0;
	double imp_late_max = 0;
	for(i = 0; i < dimension; i++){
		imp_late_avg += imp_late[i];
		if(imp_late_max < imp_late[i]) imp_late_max = imp_late[i];
	}
	imp_late_avg = imp_late_avg / (double)dimension; //平均改善率の計算

	//長さ表示
	for(i = 0; i < dimension; i++){
			printf("[%3d] %d ",i+1,P[i].length);
			if( ((i+1)%5 == 0) ) printf("\n");
	}
	printf("\n");

	printf("計測結果\n");
	printf("最大値 = %d 最小値 = %d 平均値 = %d 標準偏差 = %f\n", max_root_score,min_root_score,avg_root_score,SD);
	printf("改善率 最大値 = %f 平均値 = %f\n",imp_late_max,imp_late_avg);

	fclose(fp); //ファイルのクローズ

	/////////////////結果の出力////////////////////
	FILE *fp1;
	char *csv = ".csv";
	strcat(name,csv);
	fp1 = fopen(name,"w");
	if(fp1==NULL){
		printf("can't read file\n");
		return -1;
		}
	printf("save_file_name: %s.txt \n",argv[1]);
	for(s = 0; s < dimension; s++){
		fprintf(fp1,"%d\n",P[s].length);
	}
	fclose(fp1);

	return 0;
}