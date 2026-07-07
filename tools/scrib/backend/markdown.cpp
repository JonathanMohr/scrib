#include "markdown.hpp"

void EscapeMarkdown(std::ostream& out, const TextLine& line, bool verySafe)
{
    bool space = true;
    bool lastSpecial = false;

    bool lastEndStar = false;

    bool lineStart = true;
    for (std::size_t idx = 0; idx < line.content.size(); idx++)
    {
        const Text& text = line.content[idx];

        uint64_t startSpaceCount = 0;
        while (startSpaceCount < text.text.size() && std::isspace(static_cast<unsigned char>(text.text[startSpaceCount])))
            startSpaceCount++;

        uint64_t endSpaceCount = 0;
        while (endSpaceCount < text.text.size() && std::isspace(static_cast<unsigned char>(text.text[text.text.size() - 1 - endSpaceCount])))
            endSpaceCount++;

        std::string content = text.text.substr(startSpaceCount, text.text.size() -startSpaceCount - endSpaceCount);

        for (uint64_t i = 0; i < startSpaceCount; i++) out << ' ';
        if (startSpaceCount > 0) space = true;

        if (!space && lastSpecial && verySafe)
        {
            if (text.bold || text.italic)
            {
                out << "&#x200B;";
            }
        }

        bool boldStar = false;
        bool italicStar = false;
        if (text.bold)
        {
            if (lastSpecial && lastEndStar)
            {
                out << "__";
                lastEndStar = false;
            }
            else
            {
                out << "**";
                boldStar = true;
                lastEndStar = true;
            }
        }
        if (text.italic)
        {
            if ((lastSpecial || text.bold) && lastEndStar)
            {
                out << "_";
                lastEndStar = false;
            }
            else
            {
                out << "*";
                italicStar = true;
                lastEndStar = true;
            }
        }

        lastSpecial = false;
        space = false;

        for (std::size_t i = 0; i < content.size(); i++)
        {
            const char c = content[i];

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
                    while (j < content.size() && std::isdigit(static_cast<unsigned char>(content[j])))
                        j++;
                    if (j < content.size() && (content[j] == '.' || content[j] == ')'))
                    {
                        out << content.substr(i, j - i);
                        out << '\\' << content[j];
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

        if (text.italic)
        {
            lastEndStar = italicStar;
            if (!italicStar)
                out << "_";
            else
                out << "*";
            lastSpecial = true;
        }
        if (text.bold)
        {
            lastEndStar = boldStar;
            if (!boldStar)
                out << "__";
            else
                out << "**";
            lastSpecial = true;
        }

        for (uint64_t i = 0; i < endSpaceCount; i++) out << ' ';
        if (endSpaceCount > 0) space = true;
    }
}

void GenerateMarkdown(std::ostream& out, const Document& document, const Constants& constants, bool verySafe)
{
    (void)constants;
    
    for (const Node& node : document.nodes)
    {
        std::visit([&out, &verySafe](const auto& n)
        {
            using T = std::decay_t<decltype(n)>;

            if constexpr (std::is_same_v<T, Heading>)
            {
                if (n.subheading) out << '#';
                out << "# ";
                EscapeMarkdown(out, n.text, verySafe);
                out << '\n'; 
            }
            else if constexpr (std::is_same_v<T, TextLine>)
            {
                EscapeMarkdown(out, n, verySafe);
                out << '\n';
            }
            else if constexpr (std::is_same_v<T, EmptyLine>)
            {
                out << '\n';
            }
        }, node.data);
    }
}
