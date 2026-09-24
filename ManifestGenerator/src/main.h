#pragma once

#include <filesystem>
#include <array>
#include <string>
#include <vector>
#include <unordered_set>

#define NOMINMAX

#include <windows.h>

struct FFileData
{
    std::filesystem::path path;
    uintmax_t size = 0;
    std::array<BYTE, 32> hash{};
};

enum class EScanMode
{
    Cfg,
    NonCfg
};

enum class ENonCfgMode
{
    Automatic,
    Manual
};


extern std::vector<FFileData> Files;

extern std::unordered_set<std::string> CfgPaths;
extern std::unordered_set<std::string> NonCfgPaths;
extern std::unordered_set<std::string> ExcludedPaths;
extern std::unordered_set<std::string> CfgExtensions;

extern ENonCfgMode NonCfgMode;


void Interact();


// Configuration
void LoadCfgPaths(const std::filesystem::path& configFile);
void SetupCfgPaths(const std::filesystem::path& configFile);

void LoadNonCfgPaths(const std::filesystem::path& configFile);
void SetupNonCfgPaths(const std::filesystem::path& configFile);

void LoadExcludedPaths(const std::filesystem::path& configFile);
void SetupExcludedPaths(const std::filesystem::path& configFile);

void LoadCfgExtensions(const std::filesystem::path& configFile);
void SetupCfgExtensions(const std::filesystem::path& configFile);


// Scanning
void ScanDirectory(
    const std::filesystem::path& path,
    const std::filesystem::path& rootPath,
    EScanMode mode);


// Hashing / manifest
void HashFiles();

std::string HashToString(
    const std::array<BYTE, 32>& hash);

void GenerateManifest(
    const std::filesystem::path& manifestPath,
    const std::filesystem::path& rootPath);


// Helpers
std::string NormalizePath(
    const std::filesystem::path& path);

// relativePath must already be NormalizePath()-ed and relative to the
// scan root. ScanDirectory computes it once per entry and passes it to
// both functions below instead of each one recomputing it independently.
bool IsExcluded(
    const std::string& relativePath);

bool IsCfgFile(
    const std::filesystem::path& path,
    const std::string& relativePath);