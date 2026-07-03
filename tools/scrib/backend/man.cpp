#include "man.hpp"

#include <ctime>
#include <sstream>
#include <iomanip>

void Escape(std::ostream& out, const TextLine& line)
{
    bool skipMarker = false;

    bool lineStart = true;
    for (std::size_t idx = 0; idx < line.content.size(); idx++)
    {
        const Text& text = line.content[idx];

        if (!skipMarker)
        {
            if (!text.bold && text.italic)
                out << "\\fI";
            else if (text.bold && !text.italic)
                out << "\\fB";
            else if (text.bold && text.italic)
                out << "\\f(BI";
            else if(!lineStart)
                out << "\\fR";
        }
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

                if (c == '.')
                {
                    out << "\\&.";
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
                case '\\': case '-':
                    out << '\\' << c;
                    break;

                case '\"':
                    out << "\\(dq";

                case '\'':
                    out << "\\(aq";

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
        else
        {
            
        }
    }

    out << "\\fR";
}

void GenerateManTroff(
    std::ostream& out,
    const Document& document,
    const char* title,
    const char* section,
    const char* source,
    const char* manual
)
{
    std::time_t t = std::time(nullptr);
    std::tm utc_tm{};

#ifdef _WIN32
    gmtime_s(&utc_tm, &t);
#else
    gmtime_r(&t, &utc_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&utc_tm, "%Y-%m-%d");

    // TODO: Actually escape the symbols
    out << ".TH \""
        << title << "\" "
        << section << " \""
        << oss.str() << "\"";

    if (source)
        out << " \"" << source << "\"";
    
    if (manual)
        out << " \"" << manual << "\"";

    out << '\n';

    for (const Node& node : document.nodes)
    {
        std::visit([&out](const auto& n)
        {
            using T = std::decay_t<decltype(n)>;

            if constexpr (std::is_same_v<T, Heading>)
            {
                if (n.subheading) out << ".SS ";
                else              out << ".SH ";
                Escape(out, n.text);
                out << '\n'; 
            }
            else if constexpr (std::is_same_v<T, TextLine>)
            {
                Escape(out, n);
                out << '\n';
            }
            else if constexpr (std::is_same_v<T, EmptyLine>)
            {
                out << '\n';
            }
        }, node.data);
    }
}
