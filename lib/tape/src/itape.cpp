#include "file_tape.hpp"
#include "itape.hpp"

std::unique_ptr<ITape> ITape::new_file_tape(const char *path, const char *config_path)
{
    return std::unique_ptr<ITape>(new FileTape(path, config_path));
}