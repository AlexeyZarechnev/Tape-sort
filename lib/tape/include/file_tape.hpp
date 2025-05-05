#ifndef FILE_TAPE
#define FILE_TAPE

#include <fstream>

#include "itape.hpp"

class FileTape : public ITape {
private:

    FileTape(const char *path, const char *config_path);

    // Five rule
    FileTape(const FileTape& other) = delete; // Not allowed because file always open
    FileTape(FileTape&& other);

    FileTape& operator=(const FileTape& other) = delete; // Not allowed because file always open
    FileTape& operator=(FileTape&& other);

    // I/O
    int read();
    void write(int value);

    // Movement
    void move_forward() noexcept;
    void move_back() noexcept;
    void rewind(int delta);
    void move(int delta) noexcept;
    int current_pos() noexcept;
    int size() noexcept;
    

    ~FileTape() noexcept;

    std::fstream _storage;
    size_t _size;
    int _read_delay;
    int _write_delay;
    int _move_delay;
    int _rewind_delay;

    friend class ITape;
};

#endif