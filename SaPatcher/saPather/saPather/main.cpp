#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <Windows.h>


#define ADDR_OFFSET_PORT	0xC1004
#define ADDR_OFFSET_ENCR	0xC1006

#define ENCR_BUFFER_MAX_LENGTH	30

#pragma pack(push, 1)
#include "str_obfuscator_NoTemplate.hpp"
#pragma pack(pop)

static_assert(sizeof(unsigned short) == 2);
static_assert(sizeof(cryptor::string_encryptor) == 37);

/* 
	-l C://libsamp.so
	-i 127.127.127.127
	-p 7777
*/

class InputParser {

public:
	InputParser(int &argc, char **argv) {
		for (int i = 1; i < argc; ++i) {
			std::string s_arg = argv[i];
			this->tokens.push_back(s_arg);
		}
	}

	const std::string& getCmdOption(const std::string &option) const {
		std::vector<std::string>::const_iterator itr;
		itr = std::find(this->tokens.begin(), this->tokens.end(), option);
		if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
			return *itr;
		}
		static const std::string empty_string("");
		return empty_string;
	}

	bool cmdOptionExists(const std::string &option) const {
		return std::find(this->tokens.begin(), this->tokens.end(), option)
			!= this->tokens.end();
	}
private:
	std::vector <std::string> tokens;
};

inline void print_msg(const std::string &msg) {
	std::cout << msg << std::endl;
}

void print_as_hex(const std::string &msg) 
{
	for (size_t i = 0; i < msg.length(); ++i) 
	{
		std::cout << std::hex << (unsigned int)msg[i] << ' ';
	}

	std::cout << std::endl;
}

int main(int argc, char **argv)
{
	InputParser input(argc, argv);

	const std::string &libpath = input.getCmdOption("-l");
	if (libpath.empty()) {
		print_msg("error: use -l <path_to_libsamp>");
		return 1;
	}

	const std::string &address = input.getCmdOption("-i");
	if (!address.empty()) {
		if (address.length() > ENCR_BUFFER_MAX_LENGTH) {
			print_msg("error: length of address must not exceed 30 characters");
			return 1;
		}
	}
	else {
		print_msg("error: use -i <ip address>");
		return 1;
	}

	static unsigned short port = 0;
	if (input.cmdOptionExists("-p")) {
		port = atoi(input.getCmdOption("-p").c_str());
	}
	else {
		print_msg("error: use -p <port>");
		return 1;
	}

#ifdef _DEBUG
	print_msg("");
	print_msg("path: " + libpath);
	print_msg("address: " + address + " | length: " + std::to_string(address.length()));
	print_msg("port: " + std::to_string(port));
#endif

	const auto addressEncrypted = cryptor::create(address.c_str(), address.length() + 1);
	print_msg("encrBuffer: ");
	print_as_hex( std::string( addressEncrypted.getBuffer() ) );
	print_msg("encrKey: " + std::to_string(static_cast<int>(addressEncrypted.getKey())));

	std::fstream fs(libpath, std::fstream::binary | std::fstream::in | std::fstream::out);

	if (!fs.is_open()) {
		print_msg("error: can't open " + libpath);
		return 1;
	}

#ifdef _DEBUG
	unsigned short r_port;
	fs.seekp(ADDR_OFFSET_PORT);
	fs.read((char*)&r_port, sizeof(port));
	print_msg("");
	print_msg("=================== library info =====================");
	print_msg("library port: " + std::to_string(r_port));
#endif

#ifdef _DEBUG
	auto r_addressEncrypted = cryptor::create("test", 5);
	fs.seekp(ADDR_OFFSET_ENCR);
	fs.read((char*)&r_addressEncrypted, sizeof(cryptor::string_encryptor));
	print_msg("library encrBuffer: ");
	print_as_hex(r_addressEncrypted.getBuffer());
	print_msg("library encrKey: " + std::to_string(static_cast<int>(r_addressEncrypted.getKey())));
	print_msg("library decrBuffer: ");
	print_as_hex(std::string(r_addressEncrypted.decrypt()));
	print_msg(">>> " + std::string(r_addressEncrypted.decrypt()));
	print_msg("======================================================");
	print_msg("");
#endif

	fs.seekp(ADDR_OFFSET_PORT);
	fs.write((const char*)&port, sizeof(port));
	fs.seekp(ADDR_OFFSET_ENCR);
	fs.write((const char*)&addressEncrypted, sizeof(cryptor::string_encryptor));
	fs.close();

#ifdef _DEBUG
	print_msg("decrBuffer: ");
	print_as_hex(std::string(addressEncrypted.decrypt()));
	print_msg(">>> " + std::string(addressEncrypted.decrypt()));
#endif

	print_msg("Done!");
	return 0;
}