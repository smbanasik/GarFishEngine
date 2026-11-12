/**
 * @file
 * @brief Abstraction for relative paths in the engine.
 * @author Spencer Banasik
 */
#ifndef COM_PATHS_HPP
#define COM_PATHS_HPP

#include <filesystem>
#include <string>

namespace com {

/**
 * @class PathFinder
 * @brief Finds paths for engine.
 * @author Spencer Banasik
 * @details Takes a root path and will get file paths and directory contents relative to that root path,
 * as opposed to the CWD. This allows the engine to find its assets even if it's called from a weird spot.
 * @invariant Requires a root path
 */
class PathFinder {
  public:
    PathFinder(const std::filesystem::path& root_path) : root(root_path) {};

    /**
     * @brief Get executable directory
     * @author Spencer Banasik
     * @details Grabs the directory of our executable process.
     * @invariant OS dependent, must be a windows or linux machine. Impl apple if ever demanded.
     */
    static std::filesystem::path executable_dir();

    /**
     * @brief Get a file's path
     * @author Spencer Banasik
     * @details Returns a file path from a shorthand path to an asset, with the root directory.
     * @param [in] asset_path The include-like path to our asset relative to the root dir. e.g. src/my_asset.png
     */
    std::filesystem::path get_file_path(const std::filesystem::path& asset_path) const;

    /**
     * @brief Get a list of file paths in a folder
     * @author Spencer Banasik
     * @details Returns a list of file paths from a shorhand path.
     * @param [in] asset_path The include-like path to our folder. e.g. src/my_assets
     */
    std::vector<std::filesystem::path> get_all_file_paths(const std::filesystem::path& dir_path) const;

  private:
    std::filesystem::path root;
};
} // namespace com

#endif