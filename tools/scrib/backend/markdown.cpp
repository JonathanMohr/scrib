#include "markdown.hpp"

#include <iostream>

void EscapeMarkdown(std::ostream& out, const TextLine& line)
{
    bool skipMarker = false;

    bool lineStart = true;
    for (std::size_t idx = 0; idx < line.content.size(); idx++)
    {
        const Text& text = line.content[idx];

        const char* marker = NULL;
        if (!text.bold && text.italic)
            marker = "*";
        else if (text.bold && !text.italic)
            marker = "**";
        else if (text.bold && text.italic)
            marker = "***";

        if (marker && !skipMarker) out << marker;
        skipMarker = false;

        for (std::size_t i = 0; i < text.text.size(); i++)
        {
            const char c = text.text[i];

            if (lineStart && std::isspace(static_cast<unsigned char>(c)))
            {
                out << c;
                continue;
            }

            if (lineStart)
            {
                lineStart = false;

                if (c == '-' || c == '+' || c == '#')
                {
                    out << '\\' << c;
                    continue;
                }

                if (std::isdigit(static_cast<unsigned char>(c)))
                {
                    std::size_t j = i;
                    while (j < text.text.size() && std::isdigit(static_cast<unsigned char>(text.text[j])))
                        j++;
                    if (j < text.text.size() && (text.text[j] == '.' || text.text[j] == ')'))
                    {
                        out << text.text.substr(i, j - i);
                        out << '\\' << text.text[j];
                        i = j;
                        continue;
                    }
                }
            }

            switch (c)
            {
                case '\\': case '*': case '_': case '`':
                case '[': case ']': case '(': case ')':
                case '<': case '>': case '!': case '&': case '~':
                    out << '\\' << c;
                    break;
                default:
                    out << c;
                    break;
            }
        }

        if (idx + 1 < line.content.size() &&
            line.content[idx + 1].bold == text.bold &&
            line.content[idx + 1].italic == text.italic &&
            (text.bold || text.italic))
            skipMarker = true;
        else if (marker) out << marker;
    }
}

void GenerateMarkdown(std::ostream& out, const Document& document)
{
    for (const Node& node : document.nodes)
    {
        std::visit([&out](const auto& n)
        {
            using T = std::decay_t<decltype(n)>;

            if constexpr (std::is_same_v<T, Heading>)
            {
                if (n.subheading) out << '#';
                out << "# ";
                EscapeMarkdown(out, n.text);
                out << '\n'; 
            }
            else if constexpr (std::is_same_v<T, TextLine>)
            {
                EscapeMarkdown(out, n);
                out << '\n';
            }
            else if constexpr (std::is_same_v<T, EmptyLine>)
            {
                out << '\n';
            }
        }, node.data);
    }
}
