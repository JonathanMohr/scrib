#include "ast.hpp"
#include <cctype>
#include <iostream>
#include <stdexcept>

void Constants::addForce(const char* name, std::string value)
{
    std::deque<std::string>::size_type index = values.size();
    values.push_back(std::move(value));
    constants[name] = index;
}

bool Constants::add(const char* name, std::string value)
{
    if (constants.find(name) != constants.end())
        return false;

    addForce(name, std::move(value));
    return true;
}

const std::string* Constants::get(const char* name) const
{
    auto it = constants.find(name);
    if (it == constants.end()) return nullptr;

    std::deque<std::string>::size_type index = it->second;
    return &values[index];
}

static bool parseEscaped(std::size_t& i, std::string& line, Text& current)
{
    if (line[i] == '\\')
    {
        i++;
        if (i < line.size())
        {
            switch (line[i])
            {
                case '%': case '#':
                case '*': case '_':
                case '-': case '\\':
                    current.text += line[i];
                    return true;

                default:
                    current.text += '\\';
                    current.text += line[i];
            }
        }
        else
            current.text += '\\';

        return true;
    }

    return false;
}

Document ParseDocument(std::istream& input, Constants& constants)
{
    (void)constants;

    Document doc;

    std::string raw;

    TextParagraph textLine;
    Text current;

    while (std::getline(input, raw))
    {
        uint64_t spaceCount = 0;
        while (spaceCount < raw.size() && std::isspace(static_cast<unsigned char>(raw[spaceCount])))
            spaceCount++;

        std::string line = raw.substr(spaceCount);
        if (line.empty())
        {
            if (textLine.content.size() > 0)
            {
                bool valid = false;
                for (const Text& text : textLine.content)
                {
                    if (text.text.size() > 0) valid = true;
                }

                if (valid)
                    doc.nodes.push_back(Node{textLine});
            }

            textLine.content.clear();
        }
        else if (!textLine.content.empty())
            textLine.content.push_back(Text{false, false, " "});

        // Constant
        if (line[0] == '%')
        {
            std::string::size_type i = 1;
            while (i < line.size())
            {
                if (std::isspace(static_cast<unsigned char>(line[i]))) break;
                i++;
            }

            const std::string name = line.substr(1, i - 1);

            while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) i++;

            const std::string value = line.substr(i);

            if (!constants.add(name, value))
                throw std::runtime_error("'" + name + "' defined multiple times.");

            continue;
        }

        current.text.clear();
        current.bold = false;
        current.italic = false;

        if (!line.empty() && line[0] == '#')
        {
            std::size_t textStart = 1;
            if (line.size() > 1 && line[1] == '#')
                textStart = 2;
            if (line.size() > textStart && line[textStart] == ' ')
                textStart++;

            for (std::size_t i = textStart; i < line.size(); i++)
            {
                const char c = line[i];

                if (parseEscaped(i, line, current))
                    continue;
                else if (c == '*')
                {
                    if (i + 1 < line.size() && line[i + 1] == '*')
                    {
                        if (current.text.size() > 0)
                            textLine.content.push_back(current);
                        current.text.clear();

                        current.bold = !current.bold;

                        i++;
                    }
                    else current.text += '*';
                }
                else if (c == '_')
                {
                    if (i + 1 < line.size() && line[i + 1] == '_')
                    {
                        if (current.text.size() > 0)
                            textLine.content.push_back(current);
                        current.text.clear();

                        current.italic = !current.italic;

                        i++;
                    }
                    else current.text += '_';
                }
                else
                {
                    if (std::isspace(static_cast<unsigned char>(line[i])))
                    {
                        while (i + 1 < line.size() && std::isspace(static_cast<unsigned char>(line[i + 1])))
                            i++;
                    }
                    current.text += line[i];
                }
            }

            if (!current.text.empty())
            {
                if (current.text.size() > 0)
                    textLine.content.push_back(current);
            }

            if (line.size() > 1 && line[1] == '#')
            {
                Heading heading{textLine, true};
                doc.nodes.push_back(Node{std::move(heading)});
            }
            else
            {
                Heading heading{textLine, false};
                doc.nodes.push_back(Node{std::move(heading)});
            }
            textLine.content.clear();
        }
        else
        {
            for (std::size_t i = 0; i < line.size(); i++)
            {
                if (parseEscaped(i, line, current))
                    continue;

                const char c = line[i];
                
                if (c == '*')
                {
                    if (i + 1 < line.size() && line[i + 1] == '*')
                    {
                        if (current.text.size() > 0)
                            textLine.content.push_back(current);
                        current.text.clear();

                        current.bold = !current.bold;

                        i++;
                    }
                    else current.text += '*';
                }
                else if (c == '_')
                {
                    if (i + 1 < line.size() && line[i + 1] == '_')
                    {
                        if (current.text.size() > 0)
                            textLine.content.push_back(current);
                        current.text.clear();

                        current.italic = !current.italic;

                        i++;
                    }
                    else current.text += '_';
                }
                else
                {
                    if (std::isspace(static_cast<unsigned char>(line[i])))
                    {
                        while (i + 1 < line.size() && std::isspace(static_cast<unsigned char>(line[i + 1])))
                            i++;
                    }
                    current.text += line[i];
                }
            }

            if (!current.text.empty())
            {
                if (current.text.size() > 0)
                    textLine.content.push_back(current);
            }
        }
    }

    if (textLine.content.size() > 0)
    {
        bool valid = false;
        for (const Text& text : textLine.content)
        {
            if (text.text.size() > 0) valid = true;
        }

        if (valid)
            doc.nodes.push_back(Node{textLine});
    }

    textLine.content.clear();

    return doc;
}
