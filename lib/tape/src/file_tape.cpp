#include <stdexcept>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>

#include <iostream>

#include "file_tape.hpp"
#include "itape.hpp"

constexpr static const int PARAMETERS_COUNT = 4;
constexpr static const char *CONFIG_PARAMETERS[PARAMETERS_COUNT] = {"read_delay", "write_delay", "move_delay", "rewind_delay"};

static int buffer;

void read_parameters(const char *config_path, const char * const*expected_parameters, int values[PARAMETERS_COUNT]) 
{
    std::ifstream input(config_path);
    std::string name;
    if (!input.is_open())
        throw std::invalid_argument("cannot open config file");

    for (int i = 0; i < PARAMETERS_COUNT; ++i) {
        input >> name;
        if (name != expected_parameters[i])
            throw std::invalid_argument("unexpected parameter " + name);
        input >> values[i];
    }
}

FileTape::FileTape(const char *path, const char *config_path)
    : _storage(path, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate)
{
    if (!_storage.is_open())
        throw std::invalid_argument("cannot open storage file");

    _size = _storage.tellg() / 4;
    _storage.seekg(0, std::ios::beg);
    int values[PARAMETERS_COUNT];
    read_parameters(config_path, CONFIG_PARAMETERS, values);
    _read_delay = values[0];
    _write_delay = values[1];
    _move_delay = values[2];
    _rewind_delay = values[3];
}

FileTape::FileTape(FileTape &&other)
    : _storage(std::move(other._storage))
    , _read_delay(std::move(other._read_delay))
    , _write_delay(std::move(other._write_delay))
    , _move_delay(std::move(other._move_delay))
    , _rewind_delay(std::move(other._read_delay)) 
{}

FileTape &FileTape::operator=(FileTape &&other)
{
    if (this != &other) {
        _storage = std::move(other._storage);
        _read_delay = std::move(other._read_delay);
        _write_delay = std::move(other._write_delay);
        _move_delay = std::move(other._move_delay);
        _rewind_delay = std::move(other._read_delay);
    }
    return *this;
}

int FileTape::read()
{
    if (current_pos() == size())
        throw std::out_of_range("cannot from unwritten cell");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(_read_delay));
    _storage.read(reinterpret_cast<char*>(&buffer), 4);
    move(-1);
    return buffer;   
}

void FileTape::write(int value)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(_write_delay));
    if (current_pos() == _size)
        ++_size;
    _storage.write(reinterpret_cast<char*>(&value), 4);
    move(-1);
}

void FileTape::move_forward() noexcept
{
    std::this_thread::sleep_for(std::chrono::milliseconds(_move_delay));

    if (current_pos() == size()) {
        buffer = 0;
        _storage.write(reinterpret_cast<char*>(&buffer), 4);
        ++_size;
    } else {
       move(1);
    }
}

void FileTape::move_back() noexcept
{
    std::this_thread::sleep_for(std::chrono::milliseconds(_move_delay));

    if (current_pos() == 0)
        return;
    
    move(-1);
}

void FileTape::rewind(int delta)
{
    if (delta > 0 && current_pos() + delta >= size())
        throw std::invalid_argument("cannot rewind more than tape size");
    if (delta < 0 && current_pos() + delta < 0)
        throw std::invalid_argument("cannot rewind more than beginning of tape");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(_rewind_delay));
    move(delta);
}

int FileTape::current_pos() noexcept
{
    return _storage.tellg() / 4;
}

int FileTape::size() noexcept
{
    return _size;
}

FileTape::~FileTape() noexcept
{}

void FileTape::move(int delta) noexcept
{
    _storage.seekg(delta * 4, std::ios::cur);
}
