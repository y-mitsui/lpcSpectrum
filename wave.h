#ifndef __WAVE_H_INCLUDED__
#define __WAVE_H_INCLUDED__

#define HEADERSIZE 44

typedef struct{
	signed short l;
	signed short r;
}Soundsample16;

typedef struct{
	unsigned char l;
	unsigned char r;
}Soundsample8;

typedef struct{
	unsigned short channelnum;				//モノラルなら1、ステレオなら2
	unsigned long samplingrate;				//Hz単位
	unsigned short bit_per_sample;		//1サンプルあたりのbit数
	unsigned long datanum;						//モノラルならサンプル数を、ステレオなら左右１サンプルずつの組の数

	unsigned char *monaural8;					//8ビットモノラルのデータならこれを使う
	signed short *monaural16;						//16ビットモノラルならばこれを使う
	Soundsample8 *stereo8;						//8ビットステレオならばこれを使う
	Soundsample16 *stereo16;					//16ビットステレオならばこれを使う
}Sound;

//取得に成功すればポインタを失敗すればNULLを返す
Sound *Read_Wave(char *filename);

//書き込みに成功すれば0を失敗すれば1を返す
int Write_Wave(char *filename, Sound *snd);

//Soundを作成し、引数の情報に合わせて領域の確保をする。使われる形式以外の領域のポインタはNULL
//成功すればポインタを、失敗すればNULLを返す
Sound *Create_Sound(unsigned short channelnum, unsigned long samplingrate, unsigned short bit_per_sample, unsigned long datasize);

//Soundを開放する
void Free_Sound(Sound *snd);

#endif /*__WAVE_H_INCLUDED__*/


