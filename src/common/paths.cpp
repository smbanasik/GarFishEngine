#include <common/paths.hpp>

const std::string ERR_STRING = "";

// TODO: oh goodness! we have to do icky code here.
std::string com::PathFinder::executable_dir() {
#ifdef _WIN32
    // TODO: pass NULL to module handle to GetModuleFileName
#elif defined __linux__
    // TODO: pass /proc/self/exe to std::filesystem::canonical
#endif
}

std::string com::PathFinder::get_file_path(std::string asset_path) {

    std::string file_path = this->root + "/" + asset_path;

    // TODO: validate if file exists, complain and log if not, return ERR_STRING otherwise.

    return file_path;
}

std::vector<std::string> com::PathFinder::get_all_file_paths(std::string dir_path) {
    std::vector<std::string> file_paths;

    // TODO: need to see how to get every file in this path.

    return file_paths;
}