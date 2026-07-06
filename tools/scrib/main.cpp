#include <cstring>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <optional>

#include "backend/man.hpp"
#include "version.h"
#include "ast.hpp"

#include "backend/markdown.hpp"

typedef enum Format
{
    FORMAT_MARKDOWN,
    FORMAT_MAN_TROFF
} Format;

static void printHelp(const char* name, std::ostream& out)
{
    out << "Usage: " << name << " [-h] [-v] <file> (-o <output)\n";
    out << "\nFlags:\n";
    out << "-h/--help               Show this message\n";
    out << "-v/--version            Show the version of this executable\n";
    out << "-o <file>               Specify the output file\n";
    out << "-f/--format <format>    Specify the format for the output\n";
    out << "                        Options: markdown, man-troff\n";

    out << "-C<name> <value>        Define a constant with a value";

    out << "\nFormats:\n";
    out << "- markdown:\n";
    out << "    Constants: None\n";
    out << "- man-troff:\n";
    out << "    Constants:\n";
    out << "    - man-title:    Title for the output (default: 'scrib')\n";
    out << "    - man-section:  Section for the man output (default: '1')\n";
    out << "    - man-source:   Source for the man output (default: 'source' if man-manual else <none>)\n";
    out << "    - man-manual:   Manual for the man output (default: <none>)\n";

    out.flush();
}

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        printHelp(argv[0], std::cerr);
        return 1;
    }

    Constants constants;

    int outputFileIndex = 0;
    int inputFileIndex = 0;
    const char* formatStr = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printHelp(argv[0], std::cout);
            return 0;
        }
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
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
            if (formatStr != NULL)
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
            formatStr = argv[i];
        }

        else if (memcmp(argv[i], "-C", 2) == 0)
        {
            const char* arg = argv[i];
            if (strlen(arg) == 2)
            {
                std::cerr << "Error: Missing name after '-C'\n";
                return 1;
            }

            i++;
            if (i >= argc)
            {
                std::cerr << "Error: Missing value after '" << arg << "'\n";
                return 1;
            }

            const char* name = arg + 2;
            const char* value = argv[i];

            if (!constants.add(name, value))
            {
                std::cerr << "Error: Constant '" << name << "' already defined\n";
                return 1;
            }
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

    Format format;
    if (formatStr == NULL || strcmp(formatStr, "markdown") == 0)
    {
        format = FORMAT_MARKDOWN;
    }
    else if (strcmp(formatStr, "man-troff") == 0)
    {
        format = FORMAT_MAN_TROFF;
    }
    else
    {
        std::cerr << "Error: Invalid format: " << formatStr << '\n';
        return 1;
    }

    const char* expectedExtension = ".txt";
    std::filesystem::path input = std::filesystem::path(argv[inputFileIndex]);

    std::ifstream inputStream(input.string());
    if (!inputStream.is_open())
    {
        std::cerr << "Error: Could not open " << input << "\n";
        return 1;
    }

    Document document = ParseDocument(inputStream, constants);
    inputStream.close();

    switch (format)
    {
        case FORMAT_MARKDOWN:
            expectedExtension = ".md";
            break;

        case FORMAT_MAN_TROFF:
            expectedExtension = ".man-troff";
            break;

        default:
            std::cerr << "Internal Error: Unknown format" << '\n';
            return 1;
    }

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
    switch (format)
    {
        case FORMAT_MARKDOWN:
            GenerateMarkdown(outputStream, document, constants);
            break;

        case FORMAT_MAN_TROFF:
            GenerateManTroff(outputStream, document, constants);
            break;

        default:
            std::cerr << "Internal Error: Unknown format" << '\n';
            return 1;
    }

    outputStream.close();

    return 0;
}
