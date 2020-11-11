// DSPE_2016706018_박건희_9
#define _CRT_SECURE_NO_WARNINGS

//헤더파일
#include<stdio.h>
#include<math.h>
#include<stdlib.h>

//이미지사이즈
#define WIDTH 352
#define HEIGHT 288 

//clipping
#define CLIP(x) (x < 0 ? 0 : ( x > 255 ? 255 : x))    

typedef unsigned char BYTE;

BYTE** MemAlloc_2D(int width, int height);  // 2D memory allocation
void MemFree_2D(BYTE** arr, int height);    // 2D memory free

int Read_Frame(FILE* fp_in, BYTE** img_in, int width, int height);         // 파일 읽어오기
void Write_Frame(FILE* fp_out, BYTE** img_in, int width, int height);       // 파일 쓰기
void RGB_to_YUV(BYTE** img_in, BYTE** img_out, int width, int height);       // RGB를 YUV로
void YUV_to_RGB(BYTE** img_in, BYTE** img_out, int width, int height);       // YUV를 RGB로

void YUV444_to_420(BYTE** img_in, BYTE** img_Y, BYTE** img_U420, BYTE** img_V420, int width, int height);
void YUV420_to_444(BYTE** img_Y, BYTE** img_U420, BYTE** img_V420, BYTE** img_out, int width, int height);
void YUV444_to_422(BYTE** img_in, BYTE** img_Y, BYTE** img_U422, BYTE** img_V422, int width, int height);
void YUV422_to_444(BYTE** img_Y, BYTE** img_U422, BYTE** img_V422, BYTE** img_out, int width, int height);
int main()
{
	FILE* fp_in = fopen("Suzie_CIF_150_30.rgb", "rb");
	FILE* fp_out0 = fopen("[YUV444]Suzie_CIF_150_30.yuv", "wb");
	FILE* fp_out1 = fopen("[YUV420]Suzie_CIF_150_30.yuv", "wb");
	FILE* fp_out2 = fopen("[YUV422]Suzie_CIF_150_30.yuv", "wb");
	FILE* fp_out3 = fopen("[Recon_420]Suzie_CIF_150_30.rgb", "wb");
	FILE* fp_out4 = fopen("[Recon_422]Suzie_CIF_150_30.rgb", "wb");

	BYTE** img_YUV444, ** img_RGB;
	BYTE** img_Y, ** img_U420, ** img_V420;
	BYTE** img_U422, ** img_V422;

	int size = 1;	//loop용
	//메모리 할당
	img_YUV444 = MemAlloc_2D(WIDTH, HEIGHT * 3);
	img_RGB = MemAlloc_2D(WIDTH, HEIGHT * 3);

	img_Y = MemAlloc_2D(WIDTH, HEIGHT);
	img_U420 = MemAlloc_2D(WIDTH>>1, HEIGHT>>1);
	img_V420 = MemAlloc_2D(WIDTH >> 1, HEIGHT >> 1);

	img_U422 = MemAlloc_2D(WIDTH >> 1, HEIGHT);
	img_V422 = MemAlloc_2D(WIDTH >> 1, HEIGHT);


	while (size = Read_Frame(fp_in, img_RGB, WIDTH, HEIGHT * 3)) {
		RGB_to_YUV(img_RGB, img_YUV444, WIDTH, HEIGHT);
		Write_Frame(fp_out0, img_YUV444, WIDTH, HEIGHT * 3);
	
		YUV444_to_420(img_YUV444, img_Y, img_U420, img_V420, WIDTH, HEIGHT);
		YUV444_to_422(img_YUV444, img_Y, img_U422, img_V422, WIDTH, HEIGHT);
		
		Write_Frame(fp_out1, img_Y, WIDTH, HEIGHT);
		Write_Frame(fp_out1, img_U420, WIDTH>>1, HEIGHT>>1);
		Write_Frame(fp_out1, img_V420, WIDTH>>1, HEIGHT>>1);
		
		Write_Frame(fp_out2, img_Y, WIDTH, HEIGHT);
		Write_Frame(fp_out2, img_U422, WIDTH >> 1, HEIGHT);
		Write_Frame(fp_out2, img_V422, WIDTH >> 1, HEIGHT);
		
		YUV420_to_444(img_Y, img_U420, img_V420, img_YUV444, WIDTH, HEIGHT);
		YUV_to_RGB(img_YUV444, img_RGB, WIDTH, HEIGHT);
		Write_Frame(fp_out3, img_RGB, WIDTH, HEIGHT * 3);
		
		YUV422_to_444(img_Y, img_U422, img_V422, img_YUV444, WIDTH, HEIGHT);
		YUV_to_RGB(img_YUV444, img_RGB, WIDTH, HEIGHT);
		Write_Frame(fp_out4, img_RGB, WIDTH, HEIGHT * 3);

	}


	//메모리 반환
	MemFree_2D(img_YUV444, HEIGHT * 3);
	MemFree_2D(img_RGB, HEIGHT * 3);

	MemFree_2D(img_Y, HEIGHT);
	MemFree_2D(img_U420, HEIGHT>>1);
	MemFree_2D(img_V420, HEIGHT>>1);

	MemFree_2D(img_U422, HEIGHT);
	MemFree_2D(img_V422, HEIGHT);

	_fcloseall();
	return 0;
}

BYTE** MemAlloc_2D(int width, int height) // 2D memory allocation
{
	BYTE** arr;
	arr = (BYTE**)malloc(sizeof(BYTE*) * height);
	for (int i = 0; i < height; i++)
	{
		arr[i] = (BYTE*)malloc(sizeof(BYTE) * width);
	}

	return arr;
}

void MemFree_2D(BYTE** arr, int height)  // 2D memory free
{
	for (int i = 0; i < height; i++)
	{
		free(arr[i]);
	}
	free(arr);
}

int Read_Frame(FILE* fp_in, BYTE** img_in, int width, int height) 
{
	int i, size = 0;

	for (i = 0; i < height; i++)
		size += fread(img_in[i], sizeof(BYTE), width, fp_in);
	return size;
}

void Write_Frame(FILE* fp_out, BYTE** img_in, int width, int height) 
{
	int i;
	for (i = 0; i < height; i++)
		fwrite(img_in[i], sizeof(BYTE), width, fp_out);
}

void FileRead(char* filename, BYTE** img_in, int width, int height)  //파일로부터 데이터 읽기
{
	FILE* fp_in;

	fp_in = fopen(filename, "rb");
	for (int i = 0; i < height; i++)
	{
		fread(img_in[i], sizeof(BYTE), width, fp_in);
	}
	fclose(fp_in);
}

void FileWrite(char* filename, BYTE** img_out, int width, int height)  //파일로부터 데이터 쓰기
{
	FILE* fp_out;

	fp_out = fopen(filename, "wb");
	for (int i = 0; i < height; i++)
	{
		fwrite(img_out[i], sizeof(BYTE), width, fp_out);
	}
	fclose(fp_out);
}

void RGB_to_YUV(BYTE** img_in, BYTE** img_out, int width, int height)  // RGB -> YUV
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			img_out[i][j] = ((66 * img_in[i][j] + 129 * img_in[height + i][j] + 25 * img_in[2 * height + i][j] + 128) >> 8) + 16;			//Y
			img_out[height + i][j] = ((-38 * img_in[i][j] - 74 * img_in[height + i][j] + 112 * img_in[2 * height + i][j]) >> 8) + 128;		//U
			img_out[2 * height + i][j] = ((112 * img_in[i][j] - 94 * img_in[height + i][j] - 18 * img_in[2 * height + i][j]) >> 8) + 128;	//V
		}
	}
}

void YUV_to_RGB(BYTE** img_in, BYTE** img_out, int width, int height)  // YUV -> RGB
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			img_out[i][j] = CLIP((298 * (img_in[i][j] - 16) + 409 * (img_in[2 * height + i][j] - 128) + 128) >> 8);
			img_out[height + i][j] = CLIP((298 * (img_in[i][j] - 16) - 100 * (img_in[height + i][j] - 128) - 208 * (img_in[2 * height + i][j] - 128)) >> 8);
			img_out[2 * height + i][j] = CLIP((298 * (img_in[i][j] - 16) + 516 * (img_in[height + i][j] - 128) + 128) >> 8);
		}
	}
}


void YUV444_to_420(BYTE** img_in, BYTE** img_Y, BYTE** img_U420, BYTE** img_V420, int width, int height){	//444->420
	for(int i=0; i<height; i++)
	{ 
		for (int j = 0; j < width; j++)
		{
			img_Y[i][j] = img_in[i][j];	//Y는 유지
			//높이, 너비가 짝수일때 마다 시행
			//U,V는 height부분에서 평균
			if (i % 2 == 0 && j % 2 == 0) {
				img_U420[i / 2][j / 2] = (img_in[height + i][j] + img_in[height + i + 1][j]) / 2;	
				img_V420[i / 2][j / 2] = (img_in[2 * height + i][j] + img_in[2 * height + i + 1][j]) / 2;
			}
		}
	}
}
void YUV420_to_444(BYTE** img_Y, BYTE** img_U420, BYTE** img_V420, BYTE** img_out, int width, int height) {	//420->444
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			img_out[i][j] = img_Y[i][j];	//Y는 유지
			//U,V는 복사
			img_out[height + i][j] = img_U420[i/2][j/2];
			img_out[2 * height + i][j] = img_V420[i/2][j/2];
		}
	}
}
void YUV444_to_422(BYTE** img_in, BYTE** img_Y, BYTE** img_U422, BYTE** img_V422, int width, int height) {	//444->422
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			img_Y[i][j] = img_in[i][j];	//Y는 유지
			//U,V에서 너비가 짝수일때만 복사
			if (j % 2 == 0) {
				img_U422[i][j/2] = img_in[height + i][j];
				img_V422[i][j/2] = img_in[2 * height + i][j];
			}
		}
	}
}
void YUV422_to_444(BYTE** img_Y, BYTE** img_U422, BYTE** img_V422, BYTE** img_out, int width, int height) {	//422->444
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			img_out[i][j] = img_Y[i][j];	//Y는 유지
			//U,V복사
			img_out[height + i][j] = img_U422[i][j / 2];
			img_out[2*height + i][j] = img_V422[i][j / 2];
		}
	}
}