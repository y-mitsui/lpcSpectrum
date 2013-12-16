#include "wave.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Sound *Read_Wave(char *filename)
{
	unsigned int i;
	unsigned char header_buf[20];			        //フォーマットチャンクのサイズまでのヘッダ情報を取り込む
	FILE *fp;
	Sound *snd;
	unsigned long datasize;										//波形データのバイト数
	unsigned short fmtid;											//fmtのIDを格納する
	unsigned short channelnum;								//チャンネル数
	unsigned long samplingrate;								//サンプリング周波数
	unsigned short bit_per_sample;						//量子化ビット数
	unsigned char *buf;												//フォーマットチャンクIDから拡張部分までのデータを取り込む
	unsigned long fmtsize;

	if((fp = fopen(filename, "rb")) == NULL){
		fprintf(stderr, "Error: %s could not read.", filename);
		return NULL;
	}

	fread(header_buf, sizeof(unsigned char), 20, fp);		//フォーマットチャンクサイズまでのヘッダ部分を取り込む

	//ファイルがRIFF形式であるか
	if(strncmp(header_buf, "RIFF", 4)){
		fprintf(stderr, "Error: %s is not RIFF.", filename);
		fclose(fp);
		return NULL;
	}

	//ファイルがWAVEファイルであるか
	if(strncmp(header_buf + 8, "WAVE", 4)){
		fprintf(stderr, "Error: %s is not WAVE.", filename);
		fclose(fp);
		return NULL;
	}

	//fmt のチェック
	if(strncmp(header_buf + 12, "fmt ", 4)){
		fprintf(stderr, "Error: %s fmt not found.", filename);
		fclose(fp);
		return NULL;
	}

	memcpy(&fmtsize, header_buf + 16, sizeof(fmtsize));

	if((buf = (unsigned char *)malloc(sizeof(unsigned char)*fmtsize)) == NULL){
		fprintf(stderr, "Allocation error\n");
		fclose(fp);
		return NULL;
	}

	fread(buf, sizeof(unsigned char), fmtsize, fp);		  //フォーマットIDから拡張部分までのヘッダ部分を取り込む

	memcpy(&fmtid, buf, sizeof(fmtid));			//LinearPCMファイルならば1が入る

	if(fmtid!=1){
		fprintf(stderr, "Error: %s is not LinearPCM.", filename);
		fclose(fp);
		return NULL;
	}

	memcpy(&channelnum, buf + 2, sizeof(channelnum));	//チャンネル数を取得
	memcpy(&samplingrate, buf + 4, sizeof(samplingrate)); //サンプリング周波数を取得
	memcpy(&bit_per_sample, buf + 14, sizeof(bit_per_sample)); //量子化ビット数を取得

	fread(buf, sizeof(unsigned char), 8, fp);		  //factもしくはdataのIDとサイズを取得8バイト
	
	if(!strncmp(buf, "fact", 4)){
		fread(buf, sizeof(unsigned char), 4, fp);
		fread(buf, sizeof(unsigned char), 8, fp);
	}

	if(strncmp(buf, "data", 4)){
		fprintf(stderr, "Error: %s data part not found.", filename);
		fclose(fp);
		return NULL;
	}

	memcpy(&datasize, buf + 4, sizeof(datasize)); //波形データのサイズの取得

	if((snd = Create_Sound(channelnum, samplingrate, bit_per_sample, datasize)) == NULL){
		fclose(fp);
		return NULL;
	}

	if(channelnum==1 && bit_per_sample==8){
		fread(snd->monaural8, sizeof(unsigned char), snd->datanum, fp);		//データ部分を全て取り込む
	}else if(channelnum==1 && bit_per_sample==16){
		fread(snd->monaural16, sizeof(signed short), snd->datanum, fp);
	}else if(channelnum==2 && bit_per_sample==8){
		for(i=0; i<snd->datanum; i++){
			fread(&(snd->stereo8[i].l), sizeof(unsigned char), 1, fp);
			fread(&(snd->stereo8[i].r), sizeof(unsigned char), 1, fp);
		}
	}else if(channelnum==2 && bit_per_sample==16){
		for(i=0; i<snd->datanum; i++){
			fread(&(snd->stereo16[i].l), sizeof(signed short), 1, fp);
			fread(&(snd->stereo16[i].r), sizeof(signed short), 1, fp);
		}
	}else{
		fprintf(stderr, "Header is destroyed.");
		fclose(fp);
		Free_Sound(snd);
	}

	return snd;
}

int Write_Wave(char *filename, Sound *snd)
{
	int i;
	FILE *fp;
	unsigned char header_buf[HEADERSIZE]; //ヘッダを格納する
	unsigned long fswrh;  //リフヘッダ以外のファイルサイズ
	unsigned long fmtchunksize; //fmtチャンクのサイズ
	unsigned long dataspeed;		//データ速度
	unsigned short blocksize;   //1ブロックあたりのバイト数
	unsigned long datasize;			//周波数データのバイト数
	unsigned short fmtid;				//フォーマットID

	if((fp = fopen(filename, "wb")) == NULL){
		fprintf(stderr, "Error: %s could not open.", filename);
		return 1;
	}

	fmtchunksize = 16;
	blocksize = snd->channelnum * (snd->bit_per_sample/8);
	dataspeed = snd->samplingrate * blocksize;
	datasize = snd->datanum * blocksize;
	fswrh = datasize + HEADERSIZE - 8;
	fmtid = 1;

	header_buf[0] = 'R';
	header_buf[1] = 'I';
	header_buf[2] = 'F';
	header_buf[3] = 'F';
	memcpy(header_buf + 4, &fswrh, sizeof(fswrh));
	header_buf[8] = 'W';
	header_buf[9] = 'A';
	header_buf[10] = 'V';
	header_buf[11] = 'E';
	header_buf[12] = 'f';
	header_buf[13] = 'm';
	header_buf[14] = 't';
	header_buf[15] = ' ';
	memcpy(header_buf + 16, &fmtchunksize, sizeof(fmtchunksize));
	memcpy(header_buf + 20, &fmtid, sizeof(fmtid));
	memcpy(header_buf + 22, &(snd->channelnum), sizeof(snd->channelnum));
	memcpy(header_buf + 24, &(snd->samplingrate), sizeof(snd->samplingrate));
	memcpy(header_buf + 28, &dataspeed, sizeof(dataspeed));
	memcpy(header_buf + 32, &blocksize, sizeof(blocksize));
	memcpy(header_buf + 34, &(snd->bit_per_sample), sizeof(snd->bit_per_sample));
	header_buf[36] = 'd';
	header_buf[37] = 'a';
	header_buf[38] = 't';
	header_buf[39] = 'a';
	memcpy(header_buf + 40, &datasize, sizeof(datasize));

	fwrite(header_buf, sizeof(unsigned char), HEADERSIZE, fp);

	if(snd->channelnum==1 && snd->bit_per_sample==8){
		fwrite(snd->monaural8, sizeof(unsigned char), snd->datanum, fp);		//データ部分を全て書き込む
	}else if(snd->channelnum==1 && snd->bit_per_sample==16){
		fwrite(snd->monaural16, sizeof(signed short), snd->datanum, fp);
	}else if(snd->channelnum==2 && snd->bit_per_sample==8){
		for(i=0; i<snd->datanum; i++){
			fwrite(&(snd->stereo8[i].l), sizeof(unsigned char), 1, fp);
			fwrite(&(snd->stereo8[i].r), sizeof(unsigned char), 1, fp);
		}
	}else{
		for(i=0; i<snd->datanum; i++){
			fwrite(&(snd->stereo16[i].l), sizeof(signed short), 1, fp);
			fwrite(&(snd->stereo16[i].r), sizeof(signed short), 1, fp);
		}
	}

	fclose(fp);

	return 0;
}

Sound *Create_Sound(unsigned short channelnum, unsigned long samplingrate, unsigned short bit_per_sample, unsigned long datasize)
{
	Sound *snd;

	if((snd = (Sound *)malloc(sizeof(Sound))) == NULL){
		fprintf(stderr, "Allocation error\n");
		return NULL;
	}

	snd->channelnum = channelnum;
	snd->samplingrate = samplingrate;
	snd->bit_per_sample = bit_per_sample;
	snd->datanum = datasize / (channelnum*(bit_per_sample/8));

	snd->monaural8 = NULL;
	snd->monaural16 = NULL;
	snd->stereo8 = NULL;
	snd->stereo16 = NULL;

	if(channelnum == 1 && bit_per_sample == 8){
		if((snd->monaural8 = (unsigned char *)malloc(datasize)) == NULL){
			fprintf(stderr, "Allocation error\n");
			free(snd);
			return NULL;
		}
	}else if(channelnum == 1 && bit_per_sample == 16){
		if((snd->monaural16 = (signed short *)malloc(sizeof(signed short)*snd->datanum)) == NULL){
			fprintf(stderr, "Allocation error\n");
			free(snd);
			return NULL;
		}
	}else if(channelnum == 2 && bit_per_sample == 8){
		if((snd->stereo8 = (Soundsample8 *)malloc(sizeof(Soundsample8)*snd->datanum)) == NULL){
			fprintf(stderr, "Allocation error\n");
			free(snd);
			return NULL;
		}
	}else if(channelnum == 2 && bit_per_sample == 16){
		if((snd->stereo16 = (Soundsample16 *)malloc(sizeof(Soundsample16)*snd->datanum)) == NULL){
			fprintf(stderr, "Allocation error\n");
			free(snd);
			return NULL;
		}
	}else{
		fprintf(stderr, "Channelnum or Bit/Sample unknown");
		free(snd);
		return NULL;
	}

	return snd;
}

void Free_Sound(Sound *snd)
{
	if(snd->channelnum == 1 && snd->bit_per_sample == 8){
		free(snd->monaural8); 
	}else if(snd->channelnum == 1 && snd->bit_per_sample == 16){
		free(snd->monaural16);
	}else if(snd->channelnum == 2 && snd->bit_per_sample == 8){
		free(snd->stereo8);
	}else{
		free(snd->stereo16);
	}

	free(snd);
}


