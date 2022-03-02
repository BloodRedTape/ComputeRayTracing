#include "utils.hpp"
#include <core/os/file.hpp>

String ReadEntireFile(const char* filename) {
    SX_ASSERT(File::Exist(filename));

    File file;
    file.Open(filename, File::Mode::Read);

    size_t file_size = file.Size();
    String content(file_size);

    file.Read(content.Data(), file_size);

    file.Close();
    
    return content;
}
