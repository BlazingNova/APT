//============================================================================
// Name        : AES.cpp
// Author      : Mathew Prabakar
// Version     : 1.0
// Copyright   : Keep your hands off my code
// Description : AES Encryption in C++
//============================================================================

#include <iostream>
#include <stdio.h>
#include <bitset>
#include <vector>
#include <string>
using namespace std;

class AES{

//private:
public:
	uint8_t sbox [256]= {
		    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
		    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
		    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
		    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
		    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
		    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
		    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
		    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
		    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
		    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
		    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
		    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
		    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
		    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
		    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
		    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
		    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
		    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
		    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
		    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
		    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
		    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
		    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
		    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
		    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
		    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
		    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
		    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
		    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
		    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
		    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
		    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
	};
private:
	uint8_t KEY[16];
public:
	vector<uint32_t> Text;
	vector<uint32_t> W;
	vector<uint32_t> Result;

int peak(uint32_t a)
{
	int j = 0;
	for(j=31;j>=0;j--){
		if(a&(1<<j)){
			//cout<<"\n"<<j<<"\n";
			break;
		}
	}
	return j;
}

uint32_t modulo(uint32_t a)
{
	uint32_t q=0x11b;
	uint32_t temp=a;

	while(peak(temp)>=8)
	{
//		cout<<"Peak is "<<peak(temp)<<" "<<bitset<32>(temp)<<"\n";
//		cout<<"MOD is     "<<bitset<32>(q<<abs(8-peak(temp)))<<"\n          ";
		temp ^=(q<<abs(8-peak(temp)));
	}
	cout<<"BYE\n";

	return temp;
}

void SubBytes(uint32_t state[4])
{
	uint8_t t[4];
	for(int i=0;i<4;i++){
	t[0] = sbox[ ((state[i]&0x000000f0)>>4 ) * 16 +  (state[i]&0x0000000f )     ];
	t[1] = sbox[ ((state[i]&0x0000f000)>>12) * 16 + ((state[i]&0x00000f00)>>8)  ];
	t[2] = sbox[ ((state[i]&0x00f00000)>>20) * 16 + ((state[i]&0x000f0000)>>16) ];
	t[3] = sbox[ ((state[i]&0xf0000000)>>28) * 16 + ((state[i]&0x0f000000)>>24) ];
	state[i] = t[3]<<24 | t[2]<<16 | t[1]<<8 | t[0];
	}
}

uint32_t ShiftBytes(uint32_t a, int b)
{

	for(int i=0;i<b;i++)
	{
	bool t=a&(1<<31);
	a<<=1;
	if(t)
	a|=1;
	}

	return a;
}

uint32_t ShiftBytes(uint32_t state[4], int b, int c)
{

	uint32_t t[4];
	for(int i=0;i<c;i++)
	{

		t[0]=state[0]&(0xff<<(24-8*b));
		t[1]=state[1]&(0xff<<(24-8*b));
		t[2]=state[2]&(0xff<<(24-8*b));
		t[3]=state[3]&(0xff<<(24-8*b));

		state[0]&=~(0xff<<(24-8*b));
		state[1]&=~(0xff<<(24-8*b));
		state[2]&=~(0xff<<(24-8*b));
		state[3]&=~(0xff<<(24-8*b));

		state[0]|=t[1];
		state[1]|=t[2];
		state[2]|=t[3];
		state[3]|=t[0];
	}

}

void MixColumns(uint32_t &a)
{
	uint8_t temp[4];
	uint8_t r[4];
	uint8_t b[4];
	uint8_t h;

for(int i=3;i>=0;i--)
	{
	temp[i]=a&(0xff);
	a>>=8;
	//cout<<hex<<(int)temp[i]<<" ";

	 h = (temp[i]>>7) ? 0xff:0;
	 b[i]=temp[i]<<1;
	 b[i] ^= 0x1B & h;
	}
r[0] = b[0] ^ temp[3] ^ temp[2] ^ b[1] ^ temp[1];
r[1] = b[1] ^ temp[0] ^ temp[3] ^ b[2] ^ temp[2];
r[2] = b[2] ^ temp[1] ^ temp[0] ^ b[3] ^ temp[3];
r[3] = b[3] ^ temp[2] ^ temp[1] ^ b[0] ^ temp[0];
a = r[0]<<24 | r[1]<<16 | r[2]<<8 | r[3];
}

uint32_t mult (uint32_t a, uint32_t b)
{
	uint32_t temp=0;
	for(int i=0; i<32;i++)
	{
		if(b&1){
		temp ^= (a<<i);
		}
		b=b>>1;
	}

	temp = modulo(temp);
return temp;
}

void AddRoundKey(uint32_t in[4], int index)
{

	//cout<<dec<<index<<" ADD\n\n";
    //
	//cout<<hex<<in[0]<<"\n";
	//cout<<hex<<in[1]<<"\n";
	//cout<<hex<<in[2]<<"\n";
	//cout<<hex<<in[3]<<"\n\n";
    //
	//cout<<"PLUS \n";
	//cout<<hex<<W[index+0]<<"\n";
	//cout<<hex<<W[index+1]<<"\n";
	//cout<<hex<<W[index+2]<<"\n";
	//cout<<hex<<W[index+3]<<"\n\n";

	for(int i=0;i<4;i++)
			in[i] = in[i]^W[index+i];

	//cout<<"EQUALS \n";
	//cout<<hex<<in[0]<<"\n";
	//cout<<hex<<in[1]<<"\n";
	//cout<<hex<<in[2]<<"\n";
	//cout<<hex<<in[3]<<"\n\n";
}

uint8_t mulx(uint8_t ip)
{
	uint16_t a=ip;
	a=a<<1;
	if(a &0x100)
		a^=0x1b;
	return a;
}

uint8_t genRC(int i)
{
	if(i<=1)
		return 0x01;
	else
		return mulx(genRC(i-1));
}

uint32_t g(uint32_t a, int i)
{
	uint32_t temp;
	uint32_t res;
	temp = ShiftBytes(a,8);
	//cout<<"After ROT \n";
	//cout<<hex<<temp;

	uint8_t b[4];
	//cout<<"\n"<<hex<<((temp &0xf0000000)>>28)<<" "<<((temp &0x0f000000)>>24)<<"\n";

	b[0] = sbox[ ((temp&0x000000f0)>>4 ) * 16 + (temp&0x0000000f )       ];
	b[1] = sbox[ ((temp&0x0000f000)>>12) * 16 + ((temp&0x00000f00)>>8)   ];
	b[2] = sbox[ ((temp&0x00f00000)>>20) * 16 + ((temp&0x000f0000)>>16)  ];
	b[3] = sbox[ ((temp&0xf0000000)>>28) * 16 + ((temp &0x0f000000)>>24) ];

	//cout<<"After Sbox\n";
	//cout<<hex<<(int)b[3];
	res =b[3]<<24 | b[2]<<16 | b[1]<<8 | b[0];
	//cout<<hex<<(int)res<<"\n";

	uint32_t RC=(genRC(i/4)<<24);
	//cout<<hex<<(int)RC<<"\n";
	return res ^ RC;
}


void SetKey(string str)
{
	char myarr[16]={NULL};
	str.copy(myarr,16);
	for(int i=0;i<16;i++)
		KEY[i] = (int)myarr[i];

	cout<<"\nKEY\n";
	for(int i=0;i<16;i++)
		cout<<hex<<(int)KEY[i]<<" ";
	cout<<"\n";
}

void SetIP(string str)
{
	Text.clear();

	char myarr[str.length()+1]={NULL};
	str.copy(myarr,str.length());
	uint8_t t[4];

	//cout<<char_traits<char>::length(myarr);
	for(int i=0;i<str.length();i+=4){
//		cout<<i<<" ";
		for(int j=0; j<4;j++){
			if(i+j<str.length())
			t[j] = (int)myarr[i+j];
			else
			t[j] = 0x00;
//		cout<<t[j]<<" ";
		}
		Text.push_back(t[0]<<24 | t[1]<<16 | t[2]<<8 | t[3]);
//	cout<<"\n";
	}

	for(int i=0;i<Text.size()%4;i++)
		Text.push_back(0x00000000);
}


void keyexpansion()
{

	uint32_t wt [4];
	for(int i=0;i<4;i++)
			wt[i] = (KEY[i*4]<<24)|(KEY[i*4+1]<<16)|(KEY[i*4+2]<<8)|(KEY[i*4+3]);

	//cout<<hex<<(int)wt[0]<<" "<<(int)wt[1]<<" "<<(int)wt[2]<<" "<<(int)wt[3]<<"\n";
	W.clear();

	W.push_back(wt[0]);
	W.push_back(wt[1]);
	W.push_back(wt[2]);
	W.push_back(wt[3]);

	for(int i=3;i<43;i+=4)
	{
	uint32_t gt=g(wt[3],i+1);
	//cout<<hex<<gt<<"\n";
	wt[0] ^= gt;
	wt[1] ^= wt[0];
	wt[2] ^= wt[1];
	wt[3] ^= wt[2];
	//cout<<hex<<(int)wt[0]<<" "<<(int)wt[1]<<" "<<(int)wt[2]<<" "<<(int)wt[3]<<"\n";
	W.push_back(wt[0]);
	W.push_back(wt[1]);
	W.push_back(wt[2]);
	W.push_back(wt[3]);

	}
}

//public:

void cypher(uint32_t in[4])
{

	AddRoundKey(in,0);

	for(int i=1;i<=9;i++)
	{
		SubBytes(in);

		//cout<<"SUB BYTES EQUALS \n";
		//cout<<hex<<in[0]<<"\n";
		//cout<<hex<<in[1]<<"\n";
		//cout<<hex<<in[2]<<"\n";
		//cout<<hex<<in[3]<<"\n\n";

		ShiftBytes(in,1,1);
		ShiftBytes(in,2,2);
		ShiftBytes(in,3,3);

		//cout<<"ShiftBytes EQUALS \n";
		//cout<<hex<<in[0]<<"\n";
		//cout<<hex<<in[1]<<"\n";
		//cout<<hex<<in[2]<<"\n";
		//cout<<hex<<in[3]<<"\n\n";

		MixColumns(in[0]);
		MixColumns(in[1]);
		MixColumns(in[2]);
		MixColumns(in[3]);

		//cout<<"MixColumns EQUALS \n";
		//cout<<hex<<in[0]<<"\n";
		//cout<<hex<<in[1]<<"\n";
		//cout<<hex<<in[2]<<"\n";
		//cout<<hex<<in[3]<<"\n\n";

		AddRoundKey(in,i*4);
	}

	SubBytes(in);
	ShiftBytes(in,1,1);
	ShiftBytes(in,2,2);
	ShiftBytes(in,3,3);
	AddRoundKey(in,40);
}

vector<uint32_t> encrypt(string str)
{
	SetIP(str);
	Result.clear();
	cout<<"\nText\n";

	for(int i=0;i<Text.size();i++){
		cout<<hex<<Text[i]<<"\n";
	if(i%4==3)
		cout<<"-------------\n";
	}


	uint32_t in[4];
	keyexpansion();
//	cout<<Text.size()/4<<"\n";
	for(int i=0;i<Text.size()/4;i++)
	{
		in[0]=Text[i*4+0];
		in[1]=Text[i*4+1];
		in[2]=Text[i*4+2];
		in[3]=Text[i*4+3];

	cypher(in);

	cout<<hex<<in[0]<<"\n";
	cout<<hex<<in[1]<<"\n";
	cout<<hex<<in[2]<<"\n";
	cout<<hex<<in[3]<<"\n";

	Result.push_back(in[0]);
	Result.push_back(in[1]);
	Result.push_back(in[2]);
	Result.push_back(in[3]);
	}

	return Result;
}

const vector<uint32_t> getResult()
		{
			return Result;
		}

const uint8_t* getKey()
		{
			return KEY;
		}

void PrintResult()
	{
		cout<<"\nRESULT "<<"\n";
		for(int i=0;i<Result.size();i++)
			cout<<hex<<Result[i]<<"\n";
	}

void PrintKey()
	{
		cout<<"KEY\n";
		for(int i=0;i<16;i++)
			cout<<hex<<(int)KEY[i]<<" ";
	}
};

int main() {

	vector<uint32_t> Enc;
	AES obj;

	obj.SetKey("Thats my Kung Fu");
	obj.PrintKey();

	Enc=obj.encrypt("Two One Nine Two");
	obj.PrintResult();

	return 0;
}
