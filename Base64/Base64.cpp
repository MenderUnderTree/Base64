// Base64.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include <iostream>

typedef char Byte;
typedef char bit;

const char Base64Alphabet[64] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
								  'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
								  '0','1','2','3','4','5','6','7','8','9','+','/' };
const char PAD = '=';
/*
*将字节转换为位数组，通过移位操作实现
*/
void BytetoBit(Byte var,bit re[8]) {
	for (int i = 0; i < 8; i++) {
		re[7-i] = (var >> i) & 1;
	}
	return;
}
/*
*将位数组转换为字节，每次循环时字节左移移位用低位接收数组中的一个数
*/
void BittoByte(bit var[8],Byte* re) {
	for (int i = 0; i < 8; i++) {
		*re = (*re << 1) + var[i];
	}
	return;
}
/*
*生成24位块作为一次运算的基本单位
*/
void MakeBlock(bit re[24],bit var[3][8]) {
	for (int i = 0; i < 3; i++) {
		memcpy(re + i * 8, var[i], 8);
	}
	return;
}
/*
*生成32位块作为一次运算的基本单位
*/
void MakeBlockD(bit re[32], bit var[4][8]) {
	for (int i = 0; i < 4; i++) {
		memcpy(re + i * 8, var[i], 8);
	}
	return;
}
/*
*实现加密过程
*/
void Encipher(bit block[24],bit re[4][8]) {
	bit pad[2] = { 0,0 };
	for (int i = 0; i < 4; i++) {
		memcpy(re[i], pad, 2);
	}
	for (int i = 0; i < 4; i++) {
		memcpy(re[i] + 2, block + i * 6, 6);
	}
	for (int i = 0; i < 4; i++) {
		Byte temp;
		BittoByte(re[i], &temp);
		Byte value = Base64Alphabet[temp];
		memset(re[i], 0, 8);
		BytetoBit(value, re[i]);
	}
	return;
}
/*解密过程查表*/
Byte Index(Byte var) {
	if (var >= 65 && var <= 90) {
		var = var - 65;
	}
	else {
		if (var >= 97 && var <= 122) {
			var = var - 97 + 26;
		}
		else {
			if (var >= 48 && var <= 57) {
				var = var - 48 + 52;
			}
			else {
				if (var == '+') {
					var = 62;
				}
				else {
					if (var == '/') {
						var = 63;
					}
				}
			}
		}
	}
	return var;
}
/*
*实现解密过程
*/
void Decipher(bit EnBlock[32],bit DeBlock[24]) {
	for (int i = 0; i < 4; i++) {
		bit temp[8];
		memcpy(temp, EnBlock+i*8, 8);
		Byte tempB = 0;
		BittoByte(temp, &tempB);
		tempB = Index(tempB);
		memset(temp, 0, 8);
		BytetoBit(tempB, temp);
		memcpy(EnBlock + i * 8, temp, 8);
	}
	for (int i = 0; i < 4; i++) {
		memcpy(DeBlock + i * 6, EnBlock + 2 + i * 8, 6);
	}
	return;
}

void Enfile(char * route, char * routef) {
	FILE * file = fopen(route, "rb");
	FILE * filew = fopen(routef, "wb");
	int count;
	Byte data[3];
	bit datainbit[24];
	bit result[4][8];
	while (!feof(file))
	{
		if ((count = fread(data, 1, 3, file)) == 3) {
			bit var[3][8];
			for (int i = 0; i < 3; i++) {
				BytetoBit(data[i], var[i]);
			}
			MakeBlock(datainbit, var);
			Encipher(datainbit, result);
			for (int i = 0; i < 4; i++) {
				Byte tempB;
				BittoByte(result[i], &tempB);
				fwrite(&tempB, 1, 1, filew);
			}
		}
	}
	if(count==1){
		bit temp[8];
		bit ttemp[8];
		BytetoBit(data[0], temp);
		memset(ttemp, 0, 8);
		memcpy(ttemp + 2, temp + 6, 2);
		memcpy(temp + 2, temp, 6);
		memset(temp, 0, 2);
		Byte tempB;
		BittoByte(temp, &tempB);
		tempB = Base64Alphabet[tempB];
		fwrite(&tempB, 1, 1, filew);
		tempB = 0;
		BittoByte(ttemp, &tempB);
		tempB = Base64Alphabet[tempB];
		fwrite(&tempB, 1, 1, filew);
		fwrite(&PAD, 1, 2, filew);
	}
	if (count == 2) {
		bit temp1[8];
		bit temp2[8];
		bit temp3[8];
		memset(temp3, 0, 8);
		BytetoBit(data[0], temp1);
		BytetoBit(data[1], temp2);
		memcpy(temp3 + 2, temp2 + 4, 4);
		memcpy(temp2 + 4, temp2, 4);
		memcpy(temp2 + 2, temp1 + 6, 2);
		memset(temp2, 0, 2);
		memcpy(temp1 + 2, temp1,6);
		memset(temp1, 0, 2);
		Byte tempB;
		BittoByte(temp1, &tempB);
		tempB = Base64Alphabet[tempB];
		fwrite(&tempB, 1, 1, filew);
		tempB = 0;
		BittoByte(temp2, &tempB);
		tempB = Base64Alphabet[tempB];
		fwrite(&tempB, 1, 1, filew); BittoByte(temp1, &tempB);
		tempB = 0;
		BittoByte(temp3, &tempB);
		tempB = Base64Alphabet[tempB];
		fwrite(&tempB, 1, 1, filew);
		fwrite(&PAD, 1, 1, filew);
	}
}

void Defile(char * route, char * routef) {
	FILE * file = fopen(route, "rb");
	FILE * filew = fopen(routef, "wb");
	int count;
	Byte data[4];
	bit datainbit[32];
	bit result[24];
	while (!feof(file))
	{
		if ((count = fread(data, 1, 4, file)) == 4) {
			if (data[3] != '=') {
				bit var[4][8];
				for (int i = 0; i < 4; i++) {
					BytetoBit(data[i], var[i]);
				}
				MakeBlockD(datainbit, var);
				Decipher(datainbit, result);
				for (int i = 0; i < 3; i++) {
					Byte tempB;
					bit temp[8];
					memcpy(temp, result + i * 8, 8);
					BittoByte(temp, &tempB);
					fwrite(&tempB, 1, 1, filew);
				}
			}
			else {
				if (data[2] == '=') {
					Byte var;;
					bit temp[8];
					bit ttemp[8];
					BytetoBit(data[0], temp);
					Byte tempB = data[0];
					tempB = Index(tempB);
					BytetoBit(tempB, temp);
					memcpy(temp, temp + 2, 6);
					tempB = data[1];
					tempB = Index(tempB);
					BytetoBit(tempB, ttemp);
					memcpy(temp + 6, ttemp + 2, 2);
					BittoByte(temp, &var);
					fwrite(&var, 1, 1, filew);
				}
				else {
					Byte var1;
					Byte var2;
					bit temp1[8];
					bit temp2[8];
					bit ttemp[8];
					BytetoBit(data[0], temp1);
					Byte tempB = data[0];
					tempB = Index(tempB);
					BytetoBit(tempB, temp1);
					memcpy(temp1, temp1 + 2, 6);
					tempB = data[1];
					tempB = Index(tempB);
					BytetoBit(tempB, temp2);
					memcpy(temp1 + 6, temp2 + 2, 2);
					BittoByte(temp1, &var1);
					fwrite(&var1, 1, 1, filew);
					memcpy(ttemp, temp2 + 4, 4);
					tempB = data[2];
					tempB = Index(tempB);
					memset(temp1, 0, 8);
					BytetoBit(tempB, temp1);
					memcpy(ttemp + 4, temp1 + 2, 4);
					BittoByte(ttemp, &var2);
					fwrite(&var2, 1, 1, filew);
				}
			}
		}
	}
	fclose(file);
	fclose(filew);
}

int main() {
	char route[] = "G://confirm.txt";
	char routef[] = "G://SD.txt";
	Defile(route, routef);
	return 0;
}
