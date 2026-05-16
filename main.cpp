#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>
#include <string>
#include <cassert>

#include "base85ed.h"

// --- тесты
void run_unit_tests() {
    std::cout << "Running Base85 System Tests...\n";

    // пустые данные
    std::vector<uint8_t> empty;
    assert(base85::encode(empty).empty());
    assert(base85::decode(empty).empty());
    std::cout << "[OK] test_empty passed.\n";

    //  базовые значения
    assert(base85::pow_uint(85, 0) == 1);
    assert(base85::pow_uint(2, 3) == 8);

    std::vector<uint8_t> zeros = { 0, 0, 0, 0 };
    auto encoded_zeros = base85::encode(zeros);
    assert(encoded_zeros == std::vector<uint8_t>({ '0', '0', '0', '0', '0' }));
    assert(base85::decode(encoded_zeros) == zeros);
    std::cout << "[OK] test_basic_values passed.\n";

    //  Набивка 
    std::string text = "Hello World!";
    std::vector<uint8_t> data(text.begin(), text.end());
    auto enc = base85::encode(data);
    assert(base85::decode(enc) == data);
    std::cout << "[OK] test_padding passed.\n";

    std::cout << "All tests completed successfully!\n\n";
}

std::vector<uint8_t> read_stdin_to_vector_iostream()
{
    constexpr std::streamsize BUF_SIZE = 64 * 1024;
    std::vector<uint8_t> out;
    out.reserve(1024);
    std::vector<char> buf(BUF_SIZE);

    std::ios::sync_with_stdio(false);

    while (std::cin)
    {
        std::cin.read(buf.data(), BUF_SIZE);
        std::streamsize n = std::cin.gcount();
        if (n > 0)
        {
            out.insert(out.end(), reinterpret_cast<uint8_t*>(buf.data()),
                reinterpret_cast<uint8_t*>(buf.data() + n));
        }
        if (n < BUF_SIZE)
        {
            break;
        }
    }
    return out;
}

void write_vector_to_stdout(const std::vector<uint8_t>& data)
{
    std::ios::sync_with_stdio(false);
    std::cout.setf(std::ios::fmtflags(0), std::ios::basefield);

    if (!data.empty())
    {
        std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
    std::cout.flush();
}

int main(int argc, const char* argv[])
{
    // если программа запущена без аргументов, то автоматом прогоняем тесты
    if (argc == 1)
    {
        run_unit_tests();
        std::cout << "To use encoder, provide arguments: -e or -d\n";
        return 0;
    }

    if (argc != 2)
    {
        std::cerr << "Use -e or -d argument\n";
        return 1;
    }
    else
    {
        std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> func = nullptr;
        std::string a = argv[1];
        if (a == "--encode" || a == "-e")
        {
            func = base85::encode;
        }
        else if (a == "--decode" || a == "-d")
        {
            func = base85::decode;
        }
        else
        {
            std::cerr << "Don't know how to deal with <" << a << ">, use -e or -d\n";
            return 1;
        }

        auto data = read_stdin_to_vector_iostream();
        auto result = func(data);
        write_vector_to_stdout(result);
    }

    return 0;
}
