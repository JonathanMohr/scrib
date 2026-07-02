#include <cstring>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <optional>

#include "version.h"
#include "ast.hpp"

#include "backend/markdown.hpp"

static void printHelp(const char* name, std::ostream& out)
{
    out << "Usage: " << name << " [-h] [-v] <file> (-o <output)\n";
    out << "\nFlags:\n";
    out << "-h/--help               Show this message\n";
    out << "-v/--version            Show the version of this executable\n";
    out << "-o <file>               Specify the output file\n";
    out << "-f/--format <format>    Specify the format for the output\n";
    out << "                        Options: markdown";

    out.flush();
}

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        printHelp(argv[0], std::cerr);
        return 1;
    }

    int outputFileIndex = 0;
    int inputFileIndex = 0;
    const char* format = NULL; // Default

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 && strcmp(argv[i], "--help"))
        {
            printHelp(argv[0], std::cout);
            return 0;
        }
        if (strcmp(argv[i], "-v") == 0 && strcmp(argv[i], "--version"))
        {
            printVersion();
            return 0;
        }

        if (strcmp(argv[i], "-o") == 0)
        {
            if (outputFileIndex != 0)
            {
                std::cerr << "Error: Multiple output filenames specified\n";
                return 1;
            }
            i++;
            if (i >= argc)
            {
                std::cerr << "Error: Missing output filename after '-o'\n";
                return 1;
            }
            outputFileIndex = i;
        }

        else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0)
        {
            if (format != NULL)
            {
                std::cerr << "Error: Multiple formats specified\n";
                return 1;
            }
            i++;
            if (i >= argc)
            {
                std::cerr << "Error: Missing format after '-f'/'--format'\n";
                return 1;
            }
            format = argv[i];
        }

        else
        {
            if (inputFileIndex != 0)
            {
                std::cerr << "Error: Multiple inputs specified\n";
                return 1;
            }
            inputFileIndex = i;
        }
    }

    if (inputFileIndex == 0)
    {
        std::cerr << "Error: No input specified\n";
        return 1;
    }

    const char* expectedExtension = ".txt"; // TODO: Actually set it
    std::filesystem::path input = std::filesystem::path(argv[inputFileIndex]);

    std::ifstream inputStream(input.string());
    if (!inputStream.is_open())
    {
        std::cerr << "Error: Could not open " << input << "\n";
        return 1;
    }

    Document document = ParseDocument(inputStream);
    inputStream.close();

    expectedExtension = ".md";

    std::optional<std::filesystem::path> outputOptional = std::nullopt;
    if (outputFileIndex == 0)
        outputOptional = input.replace_extension(expectedExtension);
    else
        outputOptional = std::filesystem::path(argv[outputFileIndex]);
    std::filesystem::path& output = outputOptional.value();

    std::ofstream outputStream(output.string());
    if (!outputStream.is_open())
    {
        std::cerr << "Error: Could not open " << output << '\n';
        return 1;
    }

    // Default
    if (format == NULL | strcmp(format, "markdown") == 0)
    {
        GenerateMarkdown(outputStream, document);
    }
    else
    {
        std::cerr << "Error: Invalid format: " << format << '\n';
        return 1;
    }

    outputStream.close();

    return 0;
}
