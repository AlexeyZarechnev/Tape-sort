#ifndef I_TAPE
#define I_TAPE

#include <memory>

class ITape {
public:
    // I/O
    virtual int read() = 0;
    virtual void write(int value) = 0;

    // Movement
    virtual void move_forward() = 0;
    virtual void move_back() = 0;
    virtual void rewind(int delta) = 0;
    virtual int current_pos() = 0;
    virtual int size() = 0;

    static std::unique_ptr<ITape> new_file_tape(const char *path, const char *config_path);

    virtual ~ITape() noexcept = default;
protected:
    ITape() = default;
private:
};

#endif