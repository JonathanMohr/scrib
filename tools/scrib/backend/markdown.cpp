#include "markdown.hpp"

void EscapeMarkdown(std::ostream& out, const std::string& text)
{
    bool lineStart = true;
    for (std::size_t i = 0; i < text.size(); i++)
    {
        const char c = text[i];

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
                while (j < text.size() && std::isdigit(static_cast<unsigned char>(text[j])))
                    j++;
                if (j < text.size() && (text[j] == '.' || text[j] == ')'))
                {
                    out << text.substr(i, j - i);
                    out << '\\' << text[j];
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
                out << "# ";
                EscapeMarkdown(out, n.text);
                out << '\n'; 
            }
            else if constexpr (std::is_same_v<T, Text>)
            {
                EscapeMarkdown(out, n.content);
                out << '\n';
            }
        }, node.data);
    }
}
