#include "utils.hpp"
#include <core/os/file.hpp>
#include <string>

String ReadEntireFile(const char* filename) {
    std::string fallback = "../../../" + std::string(filename);

    if(!File::Exist(filename))
        filename = fallback.c_str();
    
    SX_ASSERT(File::Exist(filename));

    File file;
    file.Open(filename, File::Mode::Read);

    size_t file_size = file.Size();
    String content(file_size);

    file.Read(content.Data(), file_size);

    file.Close();
    
    return content;
}
