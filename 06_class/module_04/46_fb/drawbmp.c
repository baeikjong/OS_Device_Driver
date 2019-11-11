#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>  /* for O_RDONLY */
#include  <sys/mman.h> /* for mmap */
#include <linux/fb.h>  /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include "bmp.h"

#define FBDEVFILE "/dev/fb0"
//#define COLS 160
//#define ROWS 120
#define COLS 320
#define ROWS 240

//#define COLS 480
//#define ROWS 272

/* ���α׷��� �߸� ���� ��ų�� ������ ���α׷� ������ */
void usage()
{
	printf("\n=====================================\n");
	printf("\nUsage: ./drawbmp  xxx.bmp \n");
	printf("\n=====================================\n");
}

/* bmp���Ͽ��� �׸� ������ �����͸� �̾� ���� �Լ� */
/* ������ ���Ǽ��� ���� 24bit Ʈ�� Į�������� ����� �� ���� */
 void read_bmp(char *filename, char **pDib, char **data, int *cols, int *rows)
{
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER *bmpInfoHeader;
	unsigned int size;
	unsigned char ID[2];
	int nread;
	FILE *fp;

	fp = fopen(filename,"rb");
	if(fp == NULL) {
		printf("ERROR\n");
		return;
	}
	
	ID[0] = fgetc(fp);
	ID[1] = fgetc(fp);
	
/*  BMP ��ũ�� Ȯ�� */
	if(ID[0] != 'B' && ID[1] != 'M') {
		fclose(fp);
		return;
	}

 /* bmp ���Ͽ��� BITMAPFILEHEADER ��ŭ ���� */
	nread = fread(&bmpHeader.bfSize,1,sizeof(BITMAPFILEHEADER),fp);
	size = bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);	// bmp header를 제외한 사이즈를 구함.

/* ������ ���� size��ū �޸𸮸� ��� �Ͼ pDib�� ����*/
	*pDib = (unsigned char *)malloc(size);	
	fread(*pDib,1,size,fp);
	
/* ������ ���� pDib���� BITMAPINFOHEADER�� ������ */
	bmpInfoHeader = (BITMAPINFOHEADER *)*pDib;

/* 24��Ʈ true Į�� �ֶ��� ��� �� �� �ֵ��� �� */
	if(24 != bmpInfoHeader->biBitCount)
	{
		printf("It supports only 24bit bmp!\n");
		fclose(fp);
		return;
	}
	
/* BITMAPINFOHEADER���� ���ο� ���� �������� ���� */
	*cols = bmpInfoHeader->biWidth;
	*rows = bmpInfoHeader->biHeight;
	
/* ���� �������� �ּҸ� ������ */
/* ó�� ��ġ���� �����Ͱ� ��ġ�� �� ������ offset�̹Ƿ� bmpHeader ��ŭ ���ְ�, �ռ� signature�� �����鼭 �Һ��� 2����Ʈ�� ���־�� ��. */
	*data = (char *)(*pDib + bmpHeader.bfOffBits - sizeof(bmpHeader)-2);
	fclose(fp);
}

/* �Ҵ��� �޸� ���� */	
void close_bmp(char **pDib)
{
	free(*pDib);
}

int main(int argc, char *argv[])
{
	int cols, rows;
	char *pData,*data;
	char r,g,b;
	//unsigned short bmpdata1[COLS*ROWS];
	unsigned short bmpdata1[ROWS][COLS];
	unsigned short pixel;
	unsigned short *pfbmap;
	struct fb_var_screeninfo fbvar;
	int fbfd;
	int i,j,k,t;
	

	if(argc != 2){
		usage();
		return 0;
	}
	
/* ������ ���۸� ���� �� */
	fbfd = open(FBDEVFILE, O_RDWR);
	
	if(fbfd < 0){
		perror("fbdev open");
		exit(1);
	}

/* mmap �Լ��� �̿��Ͽ� flame buffer�� �޸��� ���� �ּҸ� ����� */
/* 1 pixel�� 2byte�� �ʿ��ϹǷ� *2 */
	pfbmap = (unsigned short *)
		mmap(0, COLS*ROWS*2,PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

	if((unsigned)pfbmap == (unsigned)-1)
	{
		perror("fbdev mmap");
		exit(1);
	}

	/*  bmp������ �о� �帲 */	
	read_bmp(argv[1], &pData, &data, &cols,&rows);


/**************************** Image 1 *********************************/
	 
	for(j=0;j<rows;j++){
	//for(j=120;j<(rows+120);j++){
		k = j*cols*3; /* �� �ȼ��� 3byte�ΰ��� 2byte�� �ٲ� */
		printf("cols = %d, rows = %d, k = %d\n", cols, rows, k);
		/* bmp image�� ������ ����ǹǷ� �ؿ������� �о�;� ��. */
		//t = (rows -1 - j)*320; //cols;	// 320 * 240
		//t = (rows -1 - j)*480;		// 480 * 272
		for(i=0;i<cols;i++)
		//for(i=160;i<(cols+160);i++)
		{ 
			b = *(data + (k + i*3));
			g = *(data + (k + i*3+1));
			r = *(data + (k + i*3+2));
			pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
			//bmpdata1[t+i+160] = pixel;
			//bmpdata1[j+120][i+160] = pixel;
			bmpdata1[(rows-1-j)+120*0][i+160*0] = pixel;
		}
	} 	
#if 0
/**************************** Image 2 *********************************/
	 
	for(j=0;j<rows;j++){
	//for(j=120;j<(rows+120);j++){
		k = j*cols*3; /* �� �ȼ��� 3byte�ΰ��� 2byte�� �ٲ� */
		printf("cols = %d, rows = %d, k = %d\n", cols, rows, k);
		/* bmp image�� ������ ����ǹǷ� �ؿ������� �о�;� ��. */
		//t = (rows -1 - j)*320; //cols;	// 320 * 240
		//t = (rows -1 - j)*480;		// 480 * 272
		for(i=0;i<cols;i++)
		//for(i=160;i<(cols+160);i++)
		{ 
			b = *(data + (k + i*3));
			g = *(data + (k + i*3+1));
			r = *(data + (k + i*3+2));
			pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
			//bmpdata1[t+i+160] = pixel;
			//bmpdata1[j+120][i+160] = pixel;
			bmpdata1[(rows-1-j)+120*0][i+160*1] = pixel;
		}
	} 

/**************************** Image 3 *********************************/
	 
	for(j=0;j<rows;j++){
	//for(j=120;j<(rows+120);j++){
		k = j*cols*3; /* �� �ȼ��� 3byte�ΰ��� 2byte�� �ٲ� */
		printf("cols = %d, rows = %d, k = %d\n", cols, rows, k);
		/* bmp image�� ������ ����ǹǷ� �ؿ������� �о�;� ��. */
		//t = (rows -1 - j)*320; //cols;	// 320 * 240
		//t = (rows -1 - j)*480;		// 480 * 272
		for(i=0;i<cols;i++)
		//for(i=160;i<(cols+160);i++)
		{ 
			b = *(data + (k + i*3));
			g = *(data + (k + i*3+1));
			r = *(data + (k + i*3+2));
			pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
			//bmpdata1[t+i+160] = pixel;
			//bmpdata1[j+120][i+160] = pixel;
			bmpdata1[(rows-1-j)+120*1][i+160*0] = pixel;
		}
	} 
/*********************************** Image 4 **************************/

	for(j=0;j<rows;j++){
	//for(j=120;j<(rows+120);j++){
		k = j*cols*3;
		printf("cols = %d, rows = %d, k = %d\n", cols, rows, k);
		/* bmp image�� ������ ����ǹǷ� �ؿ������� �о�;� ��. */
		//t = (rows -1 - j)*320; //cols;	// 320 * 240
		//t = (rows -1 - j)*480;		// 480 * 272
		for(i=0;i<cols;i++)
		//for(i=160;i<(cols+160);i++)
		{ 
			b = *(data + (k + i*3));
			g = *(data + (k + i*3+1));
			r = *(data + (k + i*3+2));
			pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
			//bmpdata1[t+i+160] = pixel;
			//bmpdata1[j+120][i+160] = pixel;
			bmpdata1[(rows-1-j)+120*1][i+160*1] = pixel;
		}
	}
#endif 
/*  bmp ���Ͽ��� ���� data�� ������ ������ �޸𸮷� ī�� �� */
	memcpy(pfbmap,bmpdata1, COLS*ROWS*2);
	
/*      Ȱ�� ���� �޷θ����� ������ �� ������ ���۷� close �� */
	munmap(pfbmap,ROWS*COLS*2);
	close_bmp(&pData);
	close(fbfd);
	return 0;
}