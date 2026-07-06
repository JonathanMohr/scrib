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

Document ParseDocument(std::istream& input, Constants& constants)
{
    (void)constants;

    Document doc;

    std::string raw;

    TextLine textLine;
    Text current;

    while (std::getline(input, raw))
    {
        uint64_t spaceCount = 0;
        while (spaceCount < raw.size() && std::isspace(static_cast<unsigned char>(raw[spaceCount])))
            spaceCount++;

        std::string line = raw.substr(spaceCount);
        if (line.empty())
        {
            doc.nodes.push_back(Node{EmptyLine{}});
            continue;
        }

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

        textLine.content.clear();
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

                if (c == '\\')
                {
                    i++;
                    if (i < line.size())
                    {
                        if (line[i] == '#' || line[i] == '*' ||
                            line[i] == '_' || line[i] == '-' ||
                            line[i] == '\\' || line[i] == '%')
                        {
                            current.text += line[i];
                            continue;
                        }
                    }

                    current.text += '\\';
                }
                else if (c == '*')
                {
                    if (current.text.size() > 0)
                        textLine.content.push_back(current);
                    current.text.clear();

                    current.bold = !current.bold;
                }
                else if (c == '_')
                {
                    if (current.text.size() > 0)
                        textLine.content.push_back(current);
                    current.text.clear();

                    current.italic = !current.italic;
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
        }
        else
        {
            for (std::size_t i = 0; i < line.size(); i++)
            {
                if (line[i] == '\\')
                {
                    i++;
                    if (i < line.size())
                    {
                        if (line[i] == '#' || line[i] == '*' ||
                            line[i] == '_' || line[i] == '-' ||
                            line[i] == '\\' || line[i] == '%')
                        {
                            current.text += line[i];
                            continue;
                        }
                    }
                    
                    current.text += '\\';
                    continue;
                }

                const char c = line[i];
                
                if (c == '*')
                {
                    if (current.text.size() > 0)
                        textLine.content.push_back(current);
                    current.text.clear();

                    current.bold = !current.bold;
                }
                else if (c == '_')
                {
                    if (current.text.size() > 0)
                        textLine.content.push_back(current);
                    current.text.clear();

                    current.italic = !current.italic;
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
            else
                doc.nodes.push_back(Node{EmptyLine{}});
        }
    }

    return doc;
}
