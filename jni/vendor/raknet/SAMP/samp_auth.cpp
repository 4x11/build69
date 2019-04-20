// https://github.com/P3ti/RakSAMP/blob/master/raknet/SAMP/samp_auth.cpp
#include <main.h>

#define endian_swap8(x) (x)
#define endian_swap16(x) ((x>>8) | (x<<8))
#define endian_swap32(x) ((x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24))
#define endian_swap64(x) ((x>>56) | ((x<<40) & 0x00FF000000000000) | \
		((x<<24) & 0x0000FF0000000000) | ((x<<8)  & 0x000000FF00000000) | \
		((x>>8)  & 0x00000000FF000000) | ((x>>24) & 0x0000000000FF0000) | \
        ((x>>40) & 0x000000000000FF00) | (x<<56))

#define ROTL(value, shift) ((value << shift) | (value >> (sizeof(value)*8 - shift)))
#define ROTR(value, shift) ((value >> shift) | (value << (sizeof(value)*8 - shift)))
#define swap(x,y,T) {T tmp = x; x = y; y = tmp;}

void SHA1(char *message, unsigned long *out)
{	
	unsigned long h0 = 0x67452301;
	unsigned long h1 = 0xEFCDAB89;
	unsigned long h2 = 0x98BADCFE;
	unsigned long h3 = 0x10325476;
	unsigned long h4 = 0xC3D2E1F0;

	unsigned long len = 0;
	unsigned long long bitlen = 0;
	
	while (message[len])
	{
		len++;
		bitlen += 8;
	}
	
	unsigned long complement = (55 - (len%56)) + 8*(((len+8)/64));
	unsigned long newlen = len + complement + 8 + 1;
	char *pMessage = new char[newlen];
	if (!pMessage)
		return;

	memcpy(pMessage, message, len);
	pMessage[len] = -128;
	memset(pMessage+len+1, 0, complement);

	*(unsigned long long*)&pMessage[len + 1 + complement] = endian_swap64(bitlen);

	unsigned long chunks = newlen/64;
	unsigned long w[80];

	for (unsigned long x = 0; x < chunks; x++)
	{
		for (unsigned long i = 0; i < 16; i++)
			w[i] = endian_swap32(*(unsigned long*)(&pMessage[x*64 + i*4]));

		memset(&w[16], 0, 64*4);

		for (unsigned long i = 16; i <= 79; i++)
			w[i] = ROTL((w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]), 1);

		unsigned long a = h0;
		unsigned long b = h1;
		unsigned long c = h2;
		unsigned long d = h3;
		unsigned long e = h4;

		for (unsigned long i = 0; i <= 79; i++)
		{
			unsigned long f;
			unsigned long k;

			if (0 <= i && i <= 19)
			{
				f = (b & c) | ((~b) & d);
				k = 0x5A827999;
			}
			else if (20 <= i && i <= 39)
			{
				f = b ^ c ^ d;
				k = 0x6ED9EBA1;
			}
			else if (40 <= i && i <= 59)
			{
				f = (b & c) | (b & d) | (c & d);
				k = 0x8F1BBCDC;
			}
			else if (60 <= i && i <= 79)
			{
				f = b ^ c ^ d;
				k = 0xCA62C1D6;
			}

			unsigned long temp = (ROTL(a, 5) + f + e + k + w[i])&0xFFFFFFFF;
			e = d;
			d = c;
			c = ROTL(b, 30);
			b = a;
			a = temp;
		}

		h0 = (h0 + a)&0xFFFFFFFF;
		h1 = (h1 + b)&0xFFFFFFFF;
		h2 = (h2 + c)&0xFFFFFFFF;
		h3 = (h3 + d)&0xFFFFFFFF;
		h4 = (h4 + e)&0xFFFFFFFF;
	}

	delete [] pMessage;

	out[0] = h0;
	out[1] = h1;
	out[2] = h2;
	out[3] = h3;
	out[4] = h4;
}

void SHA1(char *message, char buf[64])
{
	if (!buf) return;
	unsigned long out[5];
	SHA1(message, out);
	sprintf(buf, "%.8X%.8X%.8X%.8X%.8X", out[0], out[1], out[2], out[3], out[4]);
}

/*
v4 = (*(int (__stdcall **)(unsigned int))(*(_DWORD *)pFileSystem + 28))(0xBADDEA6Bu);
if ( v4 != -1 )
{
  ptr_of_auth_hash_transform_table = (*(int (__stdcall **)(int))(*(_DWORD *)pFileSystem + 20))(v4);
}
*/

// after version change, may need to dump this again, but wasn't changed yet afaik
const static uint8_t auth_hash_transform_table[100] = {
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80,
	0x08, 0x06, 0x00, 0x00, 0x00, 0xE4, 0xB5, 0xB7, 0x0A, 0x00, 0x00, 0x00,
	0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0B, 0x13, 0x00, 0x00, 0x0B,
	0x13, 0x01, 0x00, 0x9A, 0x9C, 0x18, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41,
	0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8E, 0x7C, 0xFB, 0x51, 0x93, 0x00, 0x00,
	0x00, 0x20, 0x63, 0x48, 0x52, 0x4D, 0x00, 0x00, 0x7A, 0x25, 0x00, 0x00,
	0x80, 0x83, 0x00, 0x00, 0xF9, 0xFF, 0x00, 0x00, 0x80, 0xE9, 0x00, 0x00,
	0x75, 0x30, 0x00, 0x00
};

uint8_t transform_auth_sha1(uint8_t value, uint8_t _xor)
{
	uint8_t result = value;

	for(uint8_t i = 0; i < 100; i++)
	{
		result = result ^ auth_hash_transform_table[i] ^ _xor;
	}

	return result;
}

// CAnimManager::AddAnimation has been hooked by kye, but resolved jmp address isn't in samp.dll
const static uint8_t code_from_CAnimManager_AddAnimation[20] = {
	0xFF, 0x25, 0x34, 0x39, // gta_sa.exe + 0x4D3AA0
	0x4D, 0x00, 0x90, 0x90, // gta_sa.exe + 0x4D3AA4
	0x90, 0x90, 0x56, 0x57, // gta_sa.exe + 0x4D3AAC
	0x50, 0x8B, 0x44, 0x24, // gta_sa.exe + 0x4D3AA8
	0x14, 0x8D, 0x0C, 0x80  // gta_sa.exe + 0x4D3AB0
};

char samp_sub_100517E0(uint8_t a1)
{
	char result = a1 + '0';

	if(a1 + '0' > '9')
	{
		result = a1 + '7';
	}

	return result;
}

void auth_stringify(char *out, uint8_t* hash)
{
	uint8_t i = 0;
	uint8_t* j = hash;

	do
	{
		out[i] = samp_sub_100517E0(*j >> 4); i++;
		out[i] = samp_sub_100517E0(*j & 0xF); i++;

		j++;
	}
	while(i < 40);

	out[i] = '\0';
}

void gen_auth_key(char buf[260], char* auth_in)
{
	char message[260];
	if(!auth_in) return;
	sprintf(message, "%s", auth_in);

	unsigned long out[5];
	uint8_t *pb_out = (uint8_t*)&out;

	SHA1(message, out);

	for(uint8_t i = 0; i < 5; i++) { pb_out[i] = transform_auth_sha1(pb_out[i], 0x2F); }
	for(uint8_t i = 5; i < 10; i++) { pb_out[i] = transform_auth_sha1(pb_out[i], 0x45); }
	for(uint8_t i = 10; i < 15; i++) { pb_out[i] = transform_auth_sha1(pb_out[i], 0x6F); }
	for(uint8_t i = 15; i < 20; i++) { pb_out[i] = transform_auth_sha1(pb_out[i], 0xDB); }
	for(uint8_t i = 0; i < 20; i++) { pb_out[i] ^= code_from_CAnimManager_AddAnimation[i]; }

	auth_stringify(buf, pb_out);
}