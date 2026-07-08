#include "markdown.hpp"
#include <cstring>
#include <ostream>

class SmartOstream
{
public:
    SmartOstream(std::ostream& _os) : os(_os) {}

    SmartOstream& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        os << manip;
        if (manip == static_cast<std::ostream&(*)(std::ostream&)>(std::endl))
            last = '\n';
        return *this;
    }

    SmartOstream& operator<<(char c)
    {
        last = c;
        os << c;
        return *this;
    }

    SmartOstream& operator<<(const char* str)
    {
        std::size_t len = std::strlen(str);
        if (len > 0)
            last = str[len - 1];
        os << str;
        return *this;
    }

    SmartOstream& operator<<(const std::string& str)
    {
        if (!str.empty())
            last = str.back();
        os << str;
        return *this;
    }

    SmartOstream& operator<<(const std::string_view& str)
    {
        if (!str.empty())
            last = str.back();
        os << str;
        return *this;
    }

    char getLast() const
    {
        return last;
    }

private:
    std::ostream& os;
    char last = '\0';
};

void EscapeMarkdown(std::ostream& _out, const TextLine& line, bool verySafe)
{
    SmartOstream out(_out);

    bool lineStart = true;
    for (std::vector<Text>::size_type idx = 0; idx < line.content.size(); idx++)
    {
        const Text& text = line.content[idx];

        unsigned char nextChar = '\0';
        if (idx + 1 < line.content.size() && !line.content[idx + 1].text.empty())
        {
            const Text& next = line.content[idx + 1];
            if (std::isspace(static_cast<unsigned char>(next.text[0])))
                nextChar = ' ';
            else if (next.bold)
                nextChar = '*';
            else if (next.italic)
                nextChar = '*';
            else
                nextChar = static_cast<unsigned char>(next.text[0]);
        }

        uint64_t startSpaceCount = 0;
        while (startSpaceCount < text.text.size() && std::isspace(static_cast<unsigned char>(text.text[startSpaceCount])))
            startSpaceCount++;

        uint64_t endSpaceCount = 0;
        while (endSpaceCount < text.text.size() && std::isspace(static_cast<unsigned char>(text.text[text.text.size() - 1 - endSpaceCount])))
            endSpaceCount++;

        std::string content = text.text.substr(startSpaceCount, text.text.size() -startSpaceCount - endSpaceCount);

        for (uint64_t i = 0; i < startSpaceCount; i++) out << ' ';

        const bool special = (text.bold || text.italic);
        const bool multiSpecial = (text.bold && text.italic);
        if (
            (out.getLast() == '*' && special && std::isalnum(nextChar)) ||
            (multiSpecial && std::isalnum(static_cast<unsigned char>(out.getLast()))) ||
            (out.getLast() == '*' && special && verySafe)
        ) out << "&#x200B;";

        bool boldStar = false;
        if (text.bold)
        {
            boldStar = (out.getLast() != '*');
            if (boldStar)
                out << "**";
            else
                out << "__";
        }

        bool italicStar = false;
        if (text.italic)
        {
            italicStar = (out.getLast() != '*');
            if (italicStar)
                out << '*';
            else
                out << '_';
        }

        for (std::string::size_type i = 0; i < content.size(); i++)
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
            if (italicStar)
                out << '*';
            else
                out << '_';
        }

        if (text.bold)
        {
            if (boldStar)
                out << "**";
            else
                out << "__";
        }

        for (uint64_t i = 0; i < endSpaceCount; i++) out << ' ';
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
