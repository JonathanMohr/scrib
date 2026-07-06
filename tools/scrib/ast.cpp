#include "ast.hpp"

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

    std::string line;

    TextLine textLine;
    Text current;

    while (std::getline(input, line))
    {
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
                            line[i] == '\\')
                        {
                            current.text += line[i];
                            continue;
                        }
                    }

                }
                else if (c == '*')
                {
                    if (current.text.size() > 0)
                    {
                        if (std::isspace(current.text[0]))
                            current.text.erase(0, 1);
                        if (!current.text.empty() && std::isspace(current.text[current.text.size() - 1]))
                            current.text.pop_back();
                        textLine.content.push_back(current);
                    }
                    current.text.clear();

                    current.bold = !current.bold;
                }
                else if (c == '_')
                {
                    if (current.text.size() > 0)
                    {
                        if (std::isspace(current.text[0]))
                            current.text.erase(0, 1);
                        if (!current.text.empty() && std::isspace(current.text[current.text.size() - 1]))
                            current.text.pop_back();
                        textLine.content.push_back(current);
                    }
                    current.text.clear();

                    current.italic = !current.italic;
                }
                else
                {
                    if (std::isspace(line[i]))
                    {
                        while (i + 1 < line.size() && std::isspace(line[i + 1]))
                            i++;
                    }
                    current.text += line[i];
                }
            }

            if (!current.text.empty())
            {
                if (current.text.size() > 0)
                {
                    if (std::isspace(current.text[0]))
                        current.text.erase(0, 1);
                    if (!current.text.empty() && std::isspace(current.text[current.text.size() - 1]))
                        current.text.pop_back();
                    textLine.content.push_back(current);
                }
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
                            line[i] == '\\')
                        {
                            current.text += line[i];
                            continue;
                        }
                    }
                    
                    current.text += '\\';
                }

                const char c = line[i];
                
                if (c == '*')
                {
                    if (current.text.size() > 0)
                    {
                        if (std::isspace(current.text[0]))
                            current.text.erase(0, 1);
                        if (!current.text.empty() && std::isspace(current.text[current.text.size() - 1]))
                            current.text.pop_back();
                        textLine.content.push_back(current);
                    }
                    current.text.clear();

                    current.bold = !current.bold;
                }
                else if (c == '_')
                {
                    if (current.text.size() > 0)
                    {
                        if (std::isspace(current.text[0]))
                            current.text.erase(0, 1);
                        if (!current.text.empty() && std::isspace(current.text[current.text.size() - 1]))
                            current.text.pop_back();
                        textLine.content.push_back(current);
                    }
                    current.text.clear();

                    current.italic = !current.italic;
                }
                else
                {
                    if (std::isspace(line[i]))
                    {
                        while (i + 1 < line.size() && std::isspace(line[i + 1]))
                            i++;
                    }
                    current.text += line[i];
                }
            }

            if (!current.text.empty())
            {
                if (current.text.size() > 0)
                {
                    if (std::isspace(current.text[0]))
                        current.text.erase(0, 1);
                    if (!current.text.empty() && std::isspace(current.text[current.text.size() - 1]))
                        current.text.pop_back();
                    textLine.content.push_back(current);
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
            else
                doc.nodes.push_back(Node{EmptyLine{}});
        }
    }

    return doc;
}
