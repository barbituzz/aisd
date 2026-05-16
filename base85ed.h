#pragma once
#pragma once
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <stdexcept>

namespace base85 {

    inline const char ALPHABET[] =
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-:+=^!/*?&<>()[]{}@%$#";

    inline unsigned int pow_uint(unsigned int base, unsigned int exp) {
        unsigned int res = 1;
        for (unsigned int i = 0; i < exp; ++i) {
            res *= base;
        }
        return res;
    }

    // функция создания карты декодирования
    inline std::array<int, 256> make_decode_map() {
        std::array<int, 256> map{};
        map.fill(-1);
        for (int i = 0; i < 85; ++i) {
            map[static_cast<uint8_t>(ALPHABET[i])] = i;
        }
        return map;
    }

    // статическая карта для быстрого декодирования 
    inline const std::array<int, 256>& get_decode_map() {
        static const std::array<int, 256> decode_map = make_decode_map();
        return decode_map;
    }

    // функция кодирования вектора байт в Base85 строку 
    inline std::vector<uint8_t> encode(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> out;
        out.reserve((data.size() + 3) / 4 * 5);

        size_t i = 0;
        for (; i + 4 <= data.size(); i += 4) {
            uint32_t value = (static_cast<uint32_t>(data[i]) << 24) |
                (static_cast<uint32_t>(data[i + 1]) << 16) |
                (static_cast<uint32_t>(data[i + 2]) << 8) |
                static_cast<uint32_t>(data[i + 3]);

            uint8_t chunk[5];
            for (int j = 4; j >= 0; --j) {
                chunk[j] = ALPHABET[value % 85];
                value /= 85;
            }
            out.insert(out.end(), chunk, chunk + 5);
        }

        
        if (i < data.size()) {
            size_t remaining = data.size() - i;
            uint32_t value = 0;
            for (size_t j = 0; j < 4; ++j) {
                value <<= 8;
                if (j < remaining) {
                    value |= data[i + j];
                }
            }

            uint8_t chunk[5];
            for (int j = 4; j >= 0; --j) {
                chunk[j] = ALPHABET[value % 85];
                value /= 85;
            }
            out.insert(out.end(), chunk, chunk + remaining + 1);
        }
        return out;
    }

    // декодирование Base85 данных обратно в бинарный вид
    inline std::vector<uint8_t> decode(const std::vector<uint8_t>& data) {
        if (data.empty()) return {};

        const auto& decode_map = get_decode_map();
        std::vector<uint8_t> out;
        out.reserve(data.size() * 4 / 5);

        size_t i = 0;
        for (; i + 5 <= data.size(); i += 5) {
            uint32_t value = 0;
            for (size_t j = 0; j < 5; ++j) {
                int code = decode_map[data[i + j]];
                if (code < 0) {
                    throw std::invalid_argument("Invalid character in Base85 stream");
                }
                value = value * 85 + code;
            }

            out.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
            out.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
            out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
            out.push_back(static_cast<uint8_t>(value & 0xFF));
        }

        // обработка неполного концевого блока символов
        if (i < data.size()) {
            size_t remaining_chars = data.size() - i;
            if (remaining_chars < 2) {
                throw std::invalid_argument("Truncated Base85 stream");
            }

            uint32_t value = 0;
            for (size_t j = 0; j < 5; ++j) {
                value *= 85;
                if (j < remaining_chars) {
                    int code = decode_map[data[i + j]];
                    if (code < 0) throw std::invalid_argument("Invalid character");
                    value += code;
                }
                else {
                    value += 84; // заполнение недостающих символов
                }
            }

            size_t bytes_to_write = remaining_chars - 1;
            for (size_t j = 0; j < bytes_to_write; ++j) {
                out.push_back(static_cast<uint8_t>((value >> (24 - j * 8)) & 0xFF));
            }
        }
        return out;
    }

}
