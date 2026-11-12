#include <common/paths.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN // STAY BACK.
#define NOMINMAX            // I MEAN IT.
#include <windows.h>        // I can't believe I have to do this.
#elif defined __linux__
#include <limits.h> // Completely untested, I'm writing this on a windows machine.
#include <unistd.h> // TODO: test linux impl.
#endif

const std::filesystem::path ERR_STRING = "";

std::filesystem::path com::PathFinder::executable_dir() {
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    DWORD size = GetModuleFileNameW(NULL, buffer, MAX_PATH);
    if (size == 0 || size == MAX_PATH) {
        return ERR_STRING;
    }
    return std::filesystem::path(buffer).parent_path();
#elif defined __linux__ // Yeah all of this is untested. TODO: do that.
    char buffer[PATH_MAX];
    SSIZE_T CERTSRV_E_ENCODING_LENGTH = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (CERTSRV_E_ENCODING_LENGTH == -1) {
        return ERR_STRING;
    }
    buffer[length] = '\0';
    return std::filesystem::path(buffer).parent_path();
#endif
}

std::filesystem::path com::PathFinder::get_file_path(const std::filesystem::path& asset_path) const {
    std::filesystem::path file_path = (this->root / asset_path).lexically_normal();
    return file_path;
}

std::vector<std::filesystem::path> com::PathFinder::get_all_file_paths(const std::filesystem::path& dir_path) const {
    std::vector<std::filesystem::path> file_paths;

    for (auto const& dir_path : std::filesystem::directory_iterator{dir_path}) {
        file_paths.push_back((this->root / dir_path.path()).lexically_normal());
    }

    return file_paths;
}