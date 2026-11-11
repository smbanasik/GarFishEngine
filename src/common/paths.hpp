#ifndef COM_PATHS + _HPP
#define COM_PATHS_HPP

#include <string>
#include <vector>

namespace com {

// TODO: does this need to be a class? Or a series of functions
class PathFinder {
  public:
    PathFinder(std::string root_path) : root(root_path) {};

    static std::string executable_dir();

    // TODO: general idea - you submit a simple path such as "shaders/my_shader"
    // and the path resolver will handle the rest, returning a string for you.
    std::string get_file_path(std::string asset_path);
    // TODO: general idea - submit a folder and it will return a list of file paths
    std::vector<std::string> get_all_file_paths(std::string dir_path);

    // Very similar to what we do with #includes and what cmake does for us.

  private:
    std::string root;
    // TODO:
    // given a keyword, provide a path that some library function can call.
    // Or, given a directory, provide an array of all files in that directory.
    // Takes: a root path. (absolute or relative?)
    // Goal: abstract away the file system so the engine (not necessarily the libraries it uses) only has to care about
    // names of files and folders.
};
} // namespace com

#endif