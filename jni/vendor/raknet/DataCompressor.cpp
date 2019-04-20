#include "DataCompressor.h"
#include "DS_HuffmanEncodingTree.h"
#include <assert.h>
#include <string.h> // Use string.h rather than memory.h for the PS3

void DataCompressor::Compress( unsigned char *userData, unsigned sizeInBytes, RakNet::BitStream * output )
{
	// Don't use this for small files as you will just make them bigger!
	assert(sizeInBytes > 2048);

	unsigned int frequencyTable[ 256 ];
	unsigned int i;
	memset(frequencyTable,0,256*sizeof(unsigned int));
	for (i=0; i < sizeInBytes; i++)
		++frequencyTable[userData[i]];
	HuffmanEncodingTree tree;
	unsigned int writeOffset1, writeOffset2, bitsUsed1, bitsUsed2;
	tree.GenerateFromFrequencyTable(frequencyTable);
	output->WriteCompressed(sizeInBytes);
	for (i=0; i < 256; i++)
		output->WriteCompressed(frequencyTable[i]);
	output->AlignWriteToByteBoundary();
	writeOffset1=output->GetWriteOffset();
	output->Write((unsigned int)0);	// Dummy value
	bitsUsed1=output->GetNumberOfBitsUsed();
	tree.EncodeArray(userData, sizeInBytes, output);
	bitsUsed2=output->GetNumberOfBitsUsed();
	writeOffset2=output->GetWriteOffset();
	output->SetWriteOffset(writeOffset1);
	output->Write(bitsUsed2-bitsUsed1); // Go back and write how many bits were used for the encoding
	output->SetWriteOffset(writeOffset2);
}

unsigned DataCompressor::DecompressAndAllocate( RakNet::BitStream * input, unsigned char **output )
{
	HuffmanEncodingTree tree;
	unsigned int bitsUsed, destinationSizeInBytes, decompressedBytes;
	unsigned int frequencyTable[ 256 ];
	unsigned i;
	
	input->ReadCompressed(destinationSizeInBytes);
	for (i=0; i < 256; i++)
		input->ReadCompressed(frequencyTable[i]);
	input->AlignReadToByteBoundary();
	if (input->Read(bitsUsed)==false)
	{
		// Read error
#ifdef _DEBUG
		assert(0);
#endif
		return 0;
	}
	*output = new unsigned char [destinationSizeInBytes];
	tree.GenerateFromFrequencyTable(frequencyTable);
	decompressedBytes=tree.DecodeArray(input, bitsUsed, destinationSizeInBytes, *output );
	assert(decompressedBytes==destinationSizeInBytes);
	return destinationSizeInBytes;
}
