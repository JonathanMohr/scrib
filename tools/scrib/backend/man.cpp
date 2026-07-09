#include "man.hpp"
#include "ast.hpp"

#include <ctime>
#include <ostream>
#include <sstream>
#include <iomanip>

void Escape(std::ostream& out, const TextParagraph& line)
{
    bool regular = true;

    bool lineStart = true;
    for (std::size_t idx = 0; idx < line.content.size(); idx++)
    {
        const Text& text = line.content[idx];

        bool newRegular = false;
        if (!text.bold && text.italic)
            out << "\\fI";
        else if (text.bold && !text.italic)
            out << "\\fB";
        else if (text.bold && text.italic)
            out << "\\f(BI";
        else
        {
            newRegular = true;
            if (!regular) out << "\\fR";
        }

        regular = newRegular;

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
    }

    if (!regular) out << "\\fR";
}

void EscapeQuoted(std::ostream& out, const std::string& text)
{
    for (char c : text)
    {
        if (c == '\\')
            out << "\\\\";
        else if (c == '"')
            out << "\"\"";
        else
            out << c;
    }
}

void GenerateManTroff(std::ostream& out, const Document& document, const Constants& constants, bool verySafe)
{
    (void)verySafe;

    const std::string* manTitle = constants.get("man-title");
    const std::string* manSection = constants.get("man-section");
    const std::string* manSource = constants.get("man-source");
    const std::string* manManual = constants.get("man-manual");

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
    out << ".TH \"";

    if (manTitle)
        EscapeQuoted(out, *manTitle);
    else
        out << "scrib";
    out << "\" ";

    if (manSection)
        EscapeQuoted(out, *manSection);
    else
        out << "1";
    out << " \"";

    out << oss.str() << '"';

    if (manSource)
    {
        out << " \"";
        EscapeQuoted(out, *manSource);
        out << '"';
    }
    else if (manManual)
    {
        out << " \"";
        EscapeQuoted(out, "source");
        out << '"';
    }
    
    if (manManual)
    {
        out << " \"";
        EscapeQuoted(out, *manManual);
        out << '"';
    }

    out << "\n\n";

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
                out << "\n\n"; 
            }
            else if constexpr (std::is_same_v<T, TextParagraph>)
            {
                Escape(out, n);
                out << "\n\n";
            }
        }, node.data);
    }
}
