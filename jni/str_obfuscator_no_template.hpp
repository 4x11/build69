#ifndef STR_OBFUSCATOR_HPP_
#define STR_OBFUSCATOR_HPP_

namespace detail {
	struct encryptor {
		static constexpr void encrypt(char *dest, const char *str, size_t len, char key) {
			for (size_t index = 0; index < len; index++) {
				dest[index] = str[index] ^ key;
			}
		}
	};
}

class cryptor
{
public:
	class string_encryptor
	{
	public: 
		constexpr string_encryptor(const char *str, size_t len) :
			_buffer{}, _decrypted{ false }, _key{ static_cast<const char>(len % 255) }, _length{len}
		{
			detail::encryptor::encrypt(_buffer, str, _length, _key);
		}
#ifdef _WIN32
		__declspec(noinline)
#elif __GNUC__
		__attribute__((noinline))
#endif
		const char *decrypt() const {
			if (_decrypted) {
				return _buffer;
			}

			for (size_t i = 0; i < _length; i++) {
				_buffer[i] ^= _key;
			}

			_decrypted = true;

			return _buffer;
		}

		char getKey() const {
			return _key;
		}

		const char* getBuffer() const {
			return _buffer;
		}

	private:
		mutable char _buffer[31];
		mutable bool _decrypted;
		const char _key;
		size_t _length;
	};

	static constexpr auto create(const char* str, size_t len) {
		return string_encryptor(str, len);
	}
};

#endif // STR_OBFUSCATOR_HPP_