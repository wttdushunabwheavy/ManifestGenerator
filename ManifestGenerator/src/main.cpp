#include "main.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")


std::vector<FFileData> Files;

std::unordered_set<std::string> CfgPaths;
std::unordered_set<std::string> NonCfgPaths;
std::unordered_set<std::string> ExcludedPaths;

std::unordered_set<std::string> CfgExtensions =
{
    ".ini"
};

ENonCfgMode NonCfgMode = ENonCfgMode::Automatic;


// =========================================================
// MAIN
// =========================================================

int main()
{
    const std::filesystem::path currentPath =
        std::filesystem::current_path();
    
    
    LoadCfgPaths(
        currentPath / "CfgFiles.cfg");

    LoadNonCfgPaths(
        currentPath / "NonCfgFiles.cfg");

    LoadExcludedPaths(
        currentPath / "ExcludedPaths.cfg");

    LoadCfgExtensions(
        currentPath / "CfgExtensions.cfg");

    Interact();

    std::cout
        << "End. Press Enter to close the program.\n";

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n');

    std::cin.get();

    return 0;
}


// =========================================================
// INTERACTION
// =========================================================

void Interact()
{
    const std::filesystem::path currentPath =
        std::filesystem::current_path();

    const std::filesystem::path filesPath =
        currentPath / "files";

    std::error_code ec;

    std::filesystem::create_directories(
        filesPath,
        ec
    );

    if (ec)
    {
        std::cerr
            << "Failed to create files directory: "
            << filesPath
            << " | "
            << ec.message()
            << '\n';

        return;
    }

    std::cout << "\n!";

    std::string command;

    if (!(std::cin >> command))
        return;

    std::cout << "\n";


    // -----------------------------------------------------
    // HELP
    // -----------------------------------------------------

    if (command == "help")
{
    std::cout
        << "Configuration:\n\n"

        << "  !setupcfg\n"
        << "      Create/reset CfgFiles.cfg.\n"
        << "      Add relative paths of CFG files.\n\n"

        << "  !setupcfgext\n"
        << "      Create/reset CfgExtensions.cfg.\n"
        << "      Add extensions treated as CFG.\n\n"

        << "  !setupnoncfg\n"
        << "      Create/reset NonCfgFiles.cfg.\n"
        << "      Add relative paths of non-CFG files.\n\n"

        << "  !setupexcluded\n"
        << "      Create/reset ExcludedPaths.cfg.\n"
        << "      Add files or folders that should never be scanned.\n\n"

        << "  !noncfgauto\n"
        << "      Enable automatic non-CFG mode.\n"
        << "      Every file that is not CFG is included.\n\n"

        << "  !noncfgmanual\n"
        << "      Enable manual non-CFG mode.\n"
        << "      Only files from NonCfgFiles.cfg are included.\n\n"


        << "Scanning:\n\n"

        << "  !scancfg\n"
        << "      Scan CFG files and print them.\n\n"

        << "  !scannoncfg\n"
        << "      Scan non-CFG files and print them.\n\n"


        << "Manifest generation:\n\n"

        << "  !generatecfgmanifest\n"
        << "      Generate CfgManifest.txt.\n\n"

        << "  !generatenoncfgmanifest\n"
        << "      Generate Manifest.txt.\n\n"

        << "  !generateboth(gb)\n"
        << "      Generate both manifests.\n\n"


        << "Quick start:\n\n"

        << "  First setup:\n"
        << "      !setupcfg -> !setupcfgext -> !setupexcluded\n\n"

        << "  CFG manifest:\n"
        << "      Configure CfgFiles.cfg/CfgExtensions.cfg\n"
        << "      -> !generatecfgmanifest\n\n"

        << "  Non-CFG automatic:\n"
        << "      !noncfgauto -> !generatenoncfgmanifest\n\n"

        << "  Non-CFG manual:\n"
        << "      Configure NonCfgFiles.cfg\n"
        << "      -> !noncfgmanual -> !generatenoncfgmanifest\n\n"

        << "  Both manifests:\n"
        << "      Configure files\n"
        << "      -> !generateboth\n\n"

        << "  Check before generation:\n"
        << "      !scancfg / !scannoncfg\n\n";

    Interact();
}


    // -----------------------------------------------------
    // CONFIG SETUP
    // -----------------------------------------------------

    else if (command == "setupcfg")
    {
        SetupCfgPaths(
            currentPath / "CfgFiles.cfg");

        Interact();
    }

    else if (command == "setupcfgext")
    {
        SetupCfgExtensions(
            currentPath / "CfgExtensions.cfg");

        Interact();
    }

    else if (command == "setupnoncfg")
    {
        SetupNonCfgPaths(
            currentPath / "NonCfgFiles.cfg");

        Interact();
    }

    else if (command == "setupexcluded")
    {
        SetupExcludedPaths(
            currentPath / "ExcludedPaths.cfg");

        Interact();
    }


    // -----------------------------------------------------
    // NON-CFG MODE
    // -----------------------------------------------------

    else if (command == "noncfgauto")
    {
        NonCfgMode =
            ENonCfgMode::Automatic;

        std::cout
            << "Non-CFG automatic mode enabled.\n";

        Interact();
    }

    else if (command == "noncfgmanual")
    {
        NonCfgMode =
            ENonCfgMode::Manual;

        std::cout
            << "Non-CFG manual mode enabled.\n";

        Interact();
    }


    // -----------------------------------------------------
    // SCAN CFG
    // -----------------------------------------------------

    else if (command == "scancfg")
    {
        Files.clear();

        ScanDirectory(
            filesPath,
            filesPath,
            EScanMode::Cfg);
        

        std::cout
            << "Found "
            << Files.size()
            << " CFG files.\n\n";

        for (const FFileData& file : Files)
        {
            std::cout
                << file.path.lexically_relative(currentPath)
                << '\n';
        }

        Interact();
    }


    // -----------------------------------------------------
    // SCAN NON-CFG
    // -----------------------------------------------------

    else if (command == "scannoncfg")
    {
        Files.clear();

        ScanDirectory(
            filesPath,
            filesPath,
            EScanMode::NonCfg);

        std::cout
            << "Found "
            << Files.size()
            << " non-CFG files.\n\n";

        for (const FFileData& file : Files)
        {
            std::cout
                << file.path.lexically_relative(currentPath)
                << '\n';
        }

        Interact();
    }


    // -----------------------------------------------------
    // GENERATE CFG MANIFEST
    // -----------------------------------------------------

    else if (command == "generatecfgmanifest")
    {
        Files.clear();

        ScanDirectory(
            filesPath,
            filesPath,
            EScanMode::Cfg);

        std::cout
            << "Found "
            << Files.size()
            << " CFG files.\n";

        HashFiles();

        GenerateManifest(
            currentPath / "CfgManifest.txt",
            currentPath);

        Interact();
    }


    // -----------------------------------------------------
    // GENERATE NON-CFG MANIFEST
    // -----------------------------------------------------

    else if (command == "generatenoncfgmanifest")
    {
        Files.clear();

        ScanDirectory(
            filesPath,
            filesPath,
            EScanMode::NonCfg);

        std::cout
            << "Found "
            << Files.size()
            << " non-CFG files.\n";

        HashFiles();

        GenerateManifest(
            currentPath / "Manifest.txt",
            currentPath);

        Interact();
    }


    // -----------------------------------------------------
    // GENERATE BOTH
    // -----------------------------------------------------

    else if (command == "generateboth" || command == "gb")
    {
        // CFG
        Files.clear();

        ScanDirectory(
            filesPath,
            filesPath,
            EScanMode::Cfg);

        std::cout
            << "Found "
            << Files.size()
            << " CFG files.\n";

        HashFiles();

        GenerateManifest(
            currentPath / "CfgManifest.txt",
            currentPath);


        // NON-CFG
        Files.clear();

        ScanDirectory(
            filesPath,
            filesPath,
            EScanMode::NonCfg);

        std::cout
            << "Found "
            << Files.size()
            << " non-CFG files.\n";

        HashFiles();

        GenerateManifest(
            currentPath / "Manifest.txt",
            currentPath);

        Interact();
    }


    // -----------------------------------------------------
    // UNKNOWN COMMAND
    // -----------------------------------------------------

    else
    {
        std::cout
            << "Invalid command.\n";

        Interact();
    }
}


// =========================================================
// PATH NORMALIZATION
// =========================================================

std::string NormalizePath(
    const std::filesystem::path& path)
{
    std::string result =
        path.lexically_normal().generic_string();

    while (!result.empty() &&
           result.back() == '/')
    {
        result.pop_back();
    }

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(
                std::tolower(c));
        });

    return result;
}


// =========================================================
// CFG PATHS
// =========================================================

void LoadCfgPaths(
    const std::filesystem::path& configFile)
{
    CfgPaths.clear();

    std::ifstream file(configFile);

    if (!file)
    {
        return;
    }

    std::string path;

    while (std::getline(file, path))
    {
        if (path.empty() ||
            path[0] == '#')
        {
            continue;
        }

        CfgPaths.insert(
            NormalizePath(path));
    }
}


void SetupCfgPaths(
    const std::filesystem::path& configFile)
{
    std::ofstream file(configFile);

    if (!file)
    {
        std::cout
            << "Failed to create CFG file.\n";

        return;
    }

    file
        << "# Explicit CFG file paths\n"
        << "# One path per line\n"
        << "#\n"
        << "# Example:\n"
        << "# Config/Game.ini\n"
        << "# Config/Graphics.ini\n"
        << "# SpecialConfig.dat\n";

    std::cout
        << "Created: "
        << configFile
        << '\n';
}


// =========================================================
// CFG EXTENSIONS
// =========================================================

void LoadCfgExtensions(
    const std::filesystem::path& configFile)
{
    CfgExtensions.clear();

    std::ifstream file(configFile);

    if (!file)
    {
        CfgExtensions.insert(".ini");
        return;
    }

    std::string extension;

    while (std::getline(file, extension))
    {
        if (extension.empty() ||
            extension[0] == '#')
        {
            continue;
        }

        if (extension[0] != '.')
        {
            extension = "." + extension;
        }

        std::transform(
            extension.begin(),
            extension.end(),
            extension.begin(),
            [](unsigned char c)
            {
                return static_cast<char>(
                    std::tolower(c));
            });

        CfgExtensions.insert(extension);
    }
}


void SetupCfgExtensions(
    const std::filesystem::path& configFile)
{
    std::ofstream file(configFile);

    if (!file)
    {
        std::cout
            << "Failed to create CFG extensions file.\n";

        return;
    }

    file
        << "# File extensions treated as CFG\n"
        << "# One extension per line\n"
        << "#\n"
        << "# Examples:\n"
        << "# .ini\n"
        << "# .cfg\n"
        << "# .json\n";

    std::cout
        << "Created: "
        << configFile
        << '\n';
}


// =========================================================
// NON-CFG PATHS
// =========================================================

void LoadNonCfgPaths(
    const std::filesystem::path& configFile)
{
    NonCfgPaths.clear();

    std::ifstream file(configFile);

    if (!file)
    {
        return;
    }

    std::string path;

    while (std::getline(file, path))
    {
        if (path.empty() ||
            path[0] == '#')
        {
            continue;
        }

        NonCfgPaths.insert(
            NormalizePath(path));
    }
}


void SetupNonCfgPaths(
    const std::filesystem::path& configFile)
{
    std::ofstream file(configFile);

    if (!file)
    {
        std::cout
            << "Failed to create NonCfgFiles.cfg.\n";

        return;
    }

    file
        << "# Manual non-CFG file paths\n"
        << "# One path per line\n"
        << "#\n"
        << "# Example:\n"
        << "# Content/Textures/a.png\n"
        << "# Content/Meshes/house.uasset\n"
        << "# SomeFile.dat\n";

    std::cout
        << "Created: "
        << configFile
        << '\n';
}


// =========================================================
// EXCLUDED PATHS
// =========================================================

void LoadExcludedPaths(
    const std::filesystem::path& configFile)
{
    ExcludedPaths.clear();

    std::ifstream file(configFile);

    if (!file)
    {
        return;
    }

    std::string path;

    while (std::getline(file, path))
    {
        if (path.empty() ||
            path[0] == '#')
        {
            continue;
        }

        ExcludedPaths.insert(
            NormalizePath(path));
    }
}


void SetupExcludedPaths(
    const std::filesystem::path& configFile)
{
    std::ofstream file(configFile);

    if (!file)
    {
        std::cout
            << "Failed to create ExcludedPaths.cfg.\n";

        return;
    }

    file
        << "# Files and folders excluded from scanning\n"
        << "# One path per line\n"
        << "#\n"
        << "# Files:\n"
        << "# ManifestGenerator.exe\n"
        << "# Manifest.txt\n"
        << "# CfgManifest.txt\n"
        << "#\n"
        << "# Folders:\n"
        << "# Saved/\n"
        << "# Intermediate/\n";

    std::cout
        << "Created: "
        << configFile
        << '\n';
}


// =========================================================
// EXCLUSION CHECK
// =========================================================

bool IsExcluded(
    const std::filesystem::path& path,
    const std::filesystem::path& rootPath)
{
    const std::string relative =
        NormalizePath(
            path.lexically_relative(rootPath));

    if (relative.empty())
    {
        return false;
    }

    for (const std::string& excluded :
         ExcludedPaths)
    {
        if (relative == excluded)
        {
            return true;
        }

        if (relative.starts_with(
                excluded + "/"))
        {
            return true;
        }
    }

    return false;
}


// =========================================================
// CFG CHECK
// =========================================================

bool IsCfgFile(
    const std::filesystem::path& path,
    const std::filesystem::path& rootPath)
{
    const std::string relative =
        NormalizePath(
            path.lexically_relative(rootPath));


    // Explicitly listed CFG file
    if (CfgPaths.contains(relative))
    {
        return true;
    }


    // CFG by extension
    std::string extension =
        path.extension().string();

    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(
                std::tolower(c));
        });


    return CfgExtensions.contains(extension);
}


// =========================================================
// DIRECTORY SCANNING
// =========================================================

void ScanDirectory(
    const std::filesystem::path& path,
    const std::filesystem::path& rootPath,
    EScanMode mode)
{
    std::error_code ec;

    std::filesystem::directory_iterator iterator(
        path,
        std::filesystem::directory_options::skip_permission_denied,
        ec);

    if (ec)
    {
        std::cerr
            << "Failed to scan directory: "
            << path
            << " | "
            << ec.message()
            << '\n';

        return;
    }


    for (const auto& entry : iterator)
    {
        if (entry.is_directory(ec))
        {
            if (ec)
            {
                ec.clear();
                continue;
            }


            // Do not recurse into excluded folders.
            if (IsExcluded(
                    entry.path(),
                    rootPath))
            {
                continue;
            }


            ScanDirectory(
                entry.path(),
                rootPath,
                mode);

            continue;
        }


        if (!entry.is_regular_file(ec))
        {
            ec.clear();
            continue;
        }


        if (IsExcluded(
                entry.path(),
                rootPath))
        {
            continue;
        }


        const bool isCfg =
            IsCfgFile(
                entry.path(),
                rootPath);


        bool add = false;


        // -------------------------------------------------
        // CFG MODE
        // -------------------------------------------------

        if (mode == EScanMode::Cfg)
        {
            add = isCfg;
        }


        // -------------------------------------------------
        // NON-CFG MODE
        // -------------------------------------------------

        else
        {
            if (NonCfgMode ==
                ENonCfgMode::Automatic)
            {
                // Everything that isn't CFG.
                add = !isCfg;
            }

            else
            {
                // Only explicitly listed files.
                const std::string relative =
                    NormalizePath(
                        entry.path()
                            .lexically_relative(rootPath));

                add =
                    NonCfgPaths.contains(relative);
            }
        }


        if (!add)
        {
            continue;
        }


        FFileData fileData;

        fileData.path =
            entry.path();

        fileData.size =
            std::filesystem::file_size(
                entry.path(),
                ec);


        if (ec)
        {
            std::cerr
                << "Failed to get file size: "
                << entry.path()
                << " | "
                << ec.message()
                << '\n';

            ec.clear();
            continue;
        }


        Files.push_back(
            std::move(fileData));
    }
}


// =========================================================
// SHA-256
// =========================================================

void HashFiles()
{
    BCRYPT_ALG_HANDLE hAlg = nullptr;


    if (!BCRYPT_SUCCESS(
            BCryptOpenAlgorithmProvider(
                &hAlg,
                BCRYPT_SHA256_ALGORITHM,
                nullptr,
                0)))
    {
        throw std::runtime_error(
            "BCryptOpenAlgorithmProvider failed");
    }


    DWORD cbHashObject = 0;
    DWORD cbData = 0;


    if (!BCRYPT_SUCCESS(
            BCryptGetProperty(
                hAlg,
                BCRYPT_OBJECT_LENGTH,
                reinterpret_cast<PUCHAR>(
                    &cbHashObject),
                sizeof(cbHashObject),
                &cbData,
                0)))
    {
        BCryptCloseAlgorithmProvider(
            hAlg,
            0);

        throw std::runtime_error(
            "BCryptGetProperty failed");
    }


    std::vector<BYTE> hashObject(
        cbHashObject);

    std::vector<BYTE> buffer(
        1024 * 1024);


    for (FFileData& fileData : Files)
    {
        BCRYPT_HASH_HANDLE hHash = nullptr;


        if (!BCRYPT_SUCCESS(
                BCryptCreateHash(
                    hAlg,
                    &hHash,
                    hashObject.data(),
                    cbHashObject,
                    nullptr,
                    0,
                    0)))
        {
            fileData.hash.fill(0);
            continue;
        }


        std::ifstream file(
            fileData.path,
            std::ios::binary);


        if (!file)
        {
            fileData.hash.fill(0);

            BCryptDestroyHash(
                hHash);

            continue;
        }


        bool ok = true;


        while (file)
        {
            file.read(
                reinterpret_cast<char*>(
                    buffer.data()),
                buffer.size());


            const std::streamsize bytesRead =
                file.gcount();


            if (bytesRead > 0)
            {
                if (!BCRYPT_SUCCESS(
                        BCryptHashData(
                            hHash,
                            buffer.data(),
                            static_cast<ULONG>(
                                bytesRead),
                            0)))
                {
                    ok = false;
                    break;
                }
            }
        }


        if (file.bad())
        {
            ok = false;
        }


        if (ok)
        {
            ok =
                BCRYPT_SUCCESS(
                    BCryptFinishHash(
                        hHash,
                        fileData.hash.data(),
                        static_cast<ULONG>(
                            fileData.hash.size()),
                        0));
        }


        if (!ok)
        {
            fileData.hash.fill(0);
        }


        BCryptDestroyHash(
            hHash);
    }


    BCryptCloseAlgorithmProvider(
        hAlg,
        0);
}


// =========================================================
// HASH -> HEX STRING
// =========================================================

std::string HashToString(
    const std::array<BYTE, 32>& hash)
{
    static constexpr char Hex[] =
        "0123456789ABCDEF";

    std::string result;

    result.reserve(64);


    for (BYTE byte : hash)
    {
        result +=
            Hex[(byte >> 4) & 0x0F];

        result +=
            Hex[byte & 0x0F];
    }


    return result;
}


// =========================================================
// MANIFEST
// =========================================================

void GenerateManifest(
    const std::filesystem::path& manifestPath,
    const std::filesystem::path& rootPath)
{
    std::ofstream file(
        manifestPath,
        std::ios::trunc);


    if (!file)
    {
        std::cout
            << "Failed to create manifest: "
            << manifestPath
            << '\n';

        return;
    }


    uintmax_t totalSize = 0;


    for (const FFileData& fileData :
         Files)
    {
        const std::string relativePath =
            fileData.path
                .lexically_relative(rootPath)
                .generic_string();


        file
            << relativePath
            << '|'
            << fileData.size
            << '|'
            << HashToString(
                fileData.hash)
            << '\n';


        totalSize +=
            fileData.size;
    }


    std::cout
        << "Manifest generated.\n"
        << "Path: "
        << manifestPath
        << '\n'
        << "Files: "
        << Files.size()
        << '\n'
        << "Total size: "
        << totalSize
        << " bytes\n";
}