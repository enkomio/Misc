#define NOMINMAX
#define UMDF_USING_NTSTATUS
#include <windows.h>
#include <bcrypt.h>
#include <ntstatus.h>

#include <cstdint>
#include <cstdio>
#include <new>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <system_error>
#include <stdexcept>
#include <atomic>
#include <io.h>
#include <fcntl.h>
#ifdef _OPENMP
#include <omp.h>
#endif

std::error_code last_win32_error()
{
	return std::error_code(static_cast<int>(GetLastError()), std::system_category());
}

using key = std::uint64_t;
static_assert(sizeof(key) == 8, "Invalid key type");

using block = std::uint64_t;
static_assert(sizeof(block) == 8, "Invalid block type");

struct key_schedule
{
	key key0;
	key key1;
};

void throw_if_failed(NTSTATUS status)
{
	if (status < 0) {
		throw std::system_error(static_cast<int>(status), std::system_category());
	}
}

[[noreturn]] void throw_last_error()
{
	throw std::system_error(last_win32_error());
}

template<class T>
std::string raw_to_hex(T const& obj)
{
	auto const raw_bytes = reinterpret_cast<unsigned char const*>(std::addressof(obj));
	constexpr auto raw_size = sizeof(obj);
	constexpr auto hex_byte_len = 2;
	char raw_hex[raw_size * hex_byte_len + 1];

	for (std::size_t i = 0; i < raw_size; ++i) {
		std::snprintf(&raw_hex[i * hex_byte_len], hex_byte_len + 1, "%02X", raw_bytes[i]);
	}

	return raw_hex;
}

static constexpr int keys_count = 1 << 24;

key key_at_index(int i)
{
	if (i >= keys_count)
		throw std::out_of_range("key index out of range");

	return (i & 0x000007) << 5 | (i & 0x0003F8) << 6 | (i & 0x01FC00) << 7 | (i & 0xFE0000) << 8;
}

void decrypt_message(BCRYPT_ALG_HANDLE des_ebc, const key_schedule& keys, const void *encrypted, std::size_t encrypted_size, void *plaintext, std::size_t& plaintext_size, DWORD flags = BCRYPT_BLOCK_PADDING);

key_schedule find_keys(BCRYPT_ALG_HANDLE des_ecb, PUCHAR known_plaintext, PUCHAR known_ciphertext) {
	struct key0_candidate
	{
		block key1_plaintext;
		key key0;

		bool operator<(const key0_candidate& other) const { return key1_plaintext < other.key1_plaintext; }
	};

	std::unique_ptr<key0_candidate[]> const key0_candidates{ new key0_candidate[keys_count] };

	auto const key0_candidates_begin = key0_candidates.get();
	auto const key0_candidates_end = key0_candidates_begin + keys_count;

	static constexpr auto progress_modulo = keys_count / 50;

	std::fprintf(stderr, "computing key 0 candidates");

	#pragma omp parallel for
	for (auto i = 0; i < keys_count; i++) {
		if ((i % progress_modulo) == 0) {
			std::fputc('.', stderr);
		}

		auto& key0_candidate = key0_candidates[i];

		key0_candidate.key0 = key_at_index(i);

		BCRYPT_KEY_HANDLE hkey;

		throw_if_failed(BCryptGenerateSymmetricKey(
			des_ecb,
			&hkey,
			nullptr,
			0,
			reinterpret_cast<PUCHAR>(&key0_candidate.key0),
			sizeof(key0_candidate.key0),
			0
		));

		ULONG written;

		throw_if_failed(BCryptEncrypt(
			hkey,
			known_plaintext,
			sizeof(block),
			nullptr,
			nullptr,
			0,
			reinterpret_cast<PUCHAR>(&key0_candidate.key1_plaintext),
			sizeof(key0_candidate.key1_plaintext),
			&written,
			0
		));

		throw_if_failed(BCryptDestroyKey(hkey));
	}

	std::fprintf(stderr, " done\n");

	std::fprintf(stderr, "indexing key 0 candidates by ciphertext...");
	std::sort(key0_candidates_begin, key0_candidates_end);
	std::fprintf(stderr, " done\n");

	std::fprintf(stderr, "meeting key 0 candidates in the middle from key 1 candidates");

    std::optional<key_schedule> found_keys;
    std::atomic<bool> found;

#ifndef _OPENMP
    {
        constexpr auto start = 0;
        constexpr auto stride = 1;
#else
    #pragma omp parallel
    {
        auto const start = omp_get_thread_num();
        auto const stride = omp_get_num_threads();
#endif
        for (int i = start, j = 0; i < keys_count; i += stride, ++j) {
            if (start == 0 && (i % progress_modulo) == 0)
                std::fputc('.', stderr);

            if ((j % progress_modulo) == 1 && found)
                break;

            key const key1 = key_at_index(i);
            BCRYPT_KEY_HANDLE hkey;

            throw_if_failed(BCryptGenerateSymmetricKey(
                des_ecb,
                &hkey,
                nullptr,
                0,
                reinterpret_cast<PUCHAR>(const_cast<key *>(&key1)),
                sizeof(key1),
                0
            ));

            key0_candidate candidate;
            ULONG written;

            throw_if_failed(BCryptEncrypt(
                hkey,
                known_ciphertext,
                sizeof(block),
                nullptr,
                nullptr,
                0,
                reinterpret_cast<PUCHAR>(&candidate.key1_plaintext),
                sizeof(candidate.key1_plaintext),
                &written,
                0
            ));

            throw_if_failed(BCryptDestroyKey(hkey));

            auto const p_candidate = std::lower_bound(key0_candidates_begin, key0_candidates_end, candidate);

            if (p_candidate != key0_candidates_end && p_candidate->key1_plaintext == candidate.key1_plaintext) {
                found_keys.emplace(key_schedule{ p_candidate->key0, key1 });
                found = true;
                break;
            }
        }
    }

    if (found) {
        std::fprintf(stderr, " done\n");
        return *found_keys;
    }

	throw std::runtime_error("no valid key pair found");
}

void decrypt_message(BCRYPT_ALG_HANDLE des_ebc, const key_schedule& keys, const void *encrypted, std::size_t encrypted_size, void *plaintext, std::size_t& plaintext_size, DWORD flags)
{
    BCRYPT_KEY_HANDLE hkey0;

    throw_if_failed(BCryptGenerateSymmetricKey(
        des_ebc,
        &hkey0,
        nullptr,
        0,
        reinterpret_cast<PUCHAR>(const_cast<key *>(&keys.key0)),
        sizeof(keys.key0),
        0
    ));

    BCRYPT_KEY_HANDLE hkey1;

    throw_if_failed(BCryptGenerateSymmetricKey(
        des_ebc,
        &hkey1,
        nullptr,
        0,
        reinterpret_cast<PUCHAR>(const_cast<key *>(&keys.key1)),
        sizeof(keys.key1),
        0
    ));

    ULONG decrypted_size;

    throw_if_failed(BCryptEncrypt(
        hkey1,
        static_cast<PUCHAR>(const_cast<void *>(encrypted)),
        static_cast<ULONG>(encrypted_size),
        nullptr,
        nullptr,
        0,
        reinterpret_cast<PUCHAR>(plaintext),
        static_cast<ULONG>(encrypted_size),
        &decrypted_size,
        0
    ));

    throw_if_failed(BCryptDecrypt(
        hkey0,
        reinterpret_cast<PUCHAR>(plaintext),
        decrypted_size,
        nullptr,
        nullptr,
        0,
        reinterpret_cast<PUCHAR>(plaintext),
        static_cast<ULONG>(encrypted_size),
        &decrypted_size,
        flags
    ));

    BCryptDestroyKey(hkey1);
    BCryptDestroyKey(hkey0);

    plaintext_size = decrypted_size;
}

std::vector<char> decrypt_message(BCRYPT_ALG_HANDLE des_ebc, const key_schedule& keys, const void *encrypted, std::size_t encrypted_size)
{
	std::vector<char> buffer(encrypted_size);
    std::size_t decrypted_size;
    decrypt_message(des_ebc, keys, encrypted, encrypted_size, buffer.data(), decrypted_size);
	buffer.resize(decrypted_size);
	return buffer;
}

std::wstring get_full_path(const wchar_t *path)
{
	if (auto const buffer_len = GetFullPathNameW(path, 0, nullptr, nullptr)) {
		std::unique_ptr<wchar_t[]> const buffer{ new wchar_t[buffer_len] };

		if (auto const full_path_len = GetFullPathNameW(path, buffer_len, buffer.get(), nullptr)) {
			if (full_path_len < buffer_len) {
				return { buffer.get(), full_path_len };
			}
			else {
				throw std::runtime_error("unexpected error getting full path name");
			}
		}
		else
			throw_last_error();
	}
	else
		throw_last_error();
}

int wmain(int argc, const wchar_t *const *argv)
{
	int ret = EXIT_FAILURE;

	try {
		if (argc < 2) {
			throw std::runtime_error("no file specified");
		}

		auto const input_arg = argv[1];

		BCRYPT_ALG_HANDLE des_ecb;
		throw_if_failed(BCryptOpenAlgorithmProvider(&des_ecb, BCRYPT_DES_ALGORITHM, nullptr, 0));

		throw_if_failed(BCryptSetProperty(
			des_ecb,
			BCRYPT_CHAINING_MODE,
			const_cast<UCHAR *>(reinterpret_cast<const UCHAR *>(BCRYPT_CHAIN_MODE_ECB)),
			sizeof(BCRYPT_CHAIN_MODE_ECB),
			0
		));

		constexpr std::size_t block_size = 8;

		if (auto hmodule = LoadLibraryExW(get_full_path(input_arg).c_str(), nullptr, LOAD_LIBRARY_AS_DATAFILE)) {
			if (auto hrsrc = FindResourceExW(hmodule, L"4DES", L"SEGRETO", MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL))) {
				auto const resource_size = SizeofResource(hmodule, hrsrc);

				if (resource_size >= block_size) {
					if (auto hres = LoadResource(hmodule, hrsrc)) {
						if (auto const resource_data = LockResource(hres)) {
							static char const known_plaintext[] = "Oggetto:";
							constexpr std::size_t known_plaintext_size = sizeof(known_plaintext) - 1;
							static_assert(known_plaintext_size == sizeof(block), "Invalid known plaintext");

							auto const keys = find_keys(des_ecb, reinterpret_cast<PUCHAR>(const_cast<char *>(known_plaintext)), static_cast<PUCHAR>(resource_data));
							std::fprintf(stderr, "found keys %s and %s\n", raw_to_hex(keys.key0).c_str(), raw_to_hex(keys.key1).c_str());

							auto const decrypted = decrypt_message(des_ecb, keys, resource_data, resource_size);

							if (auto const buffer_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, decrypted.data(), static_cast<int>(decrypted.size()), nullptr, 0)) {
								std::vector<WCHAR> buffer(buffer_size);

								if (auto const string_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, decrypted.data(), static_cast<int>(decrypted.size()), buffer.data(), static_cast<int>(buffer.size()))) {
									std::fflush(stdout);
									_setmode(_fileno(stdout), _O_WTEXT);

									buffer.resize(string_size);

									for (auto c : buffer) {
										_putwc_nolock(c, stdout);
									}

									ret = EXIT_SUCCESS;
								}
							}

							if (ret != EXIT_SUCCESS) {
								std::fprintf(stderr, "could not decode message as UTF-8, writing as raw bytes instead\n");

								for (auto c : decrypted) {
									_putc_nolock(c, stdout);
								}

								ret = EXIT_SUCCESS;
							}
						}
						else 
							std::fprintf(stderr, "fatal error: can't lock encrypted data: %s\n", last_win32_error().message().c_str());
					}
					else
						std::fprintf(stderr, "fatal error: can't load encrypted data: %s\n", last_win32_error().message().c_str());
				}
				else
					std::fprintf(stderr, "fatal error: encrypted data too short (%lu bytes)\n", resource_size);
			}
			else
				std::fprintf(stderr, "fatal error: can't find encrypted data resource: %s\n", last_win32_error().message().c_str());

			FreeLibrary(hmodule);
		}
		else
			std::fprintf(stderr, "fatal error: can't load file: %s\n", last_win32_error().message().c_str());
	}
	catch (const std::exception& e) {
		std::fprintf(stderr, "fatal error: %s\n", e.what());
	}

	return ret;
}