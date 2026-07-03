#include "ast.hpp"

typedef enum Status
{
    STATUS_NORMAL,
    STATUS_ITALIC,
    STATUS_BOLD,
    STATUS_ITALIC_BOLD
} Status;

static void convertStatus(Status status, bool& bold, bool& italic)
{
    switch (status)
    {
        case STATUS_ITALIC:
            bold = false;
            italic = true;
            break;

        case STATUS_BOLD:
            bold = true;
            italic = false;
            break;

        case STATUS_ITALIC_BOLD:
            bold = true;
            italic = true;
            break;

        case STATUS_NORMAL: default:
            bold = false;
            italic = false;
            break;
    }
}

Document ParseDocument(std::istream& input)
{
    Document doc;
    std::string line;

    TextLine textLine;
    Text current;
    Status status;
    while (std::getline(input, line))
    {
        textLine.content.clear();
        current.text.clear();
        status = STATUS_NORMAL;

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
                        if (line[i] == '*' || line[i] == '_' || line[i] == '\\')
                        {
                            current.text += line[i];
                            continue;
                        }
                    }

                }
                else if (c == '*')
                {
                    convertStatus(status, current.bold, current.italic);
                    if (current.text.size() > 0)
                        textLine.content.push_back(current);
                    current.text.clear();

                    if (status == STATUS_NORMAL)
                        status = STATUS_BOLD;
                    else if (status == STATUS_BOLD)
                        status = STATUS_NORMAL;
                }
                else
                {
                    current.text += line[i];
                }
            }

            if (!current.text.empty())
            {
                convertStatus(status, current.bold, current.italic);
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
                            line[i] == '_' || line[i] == '$' ||
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
                    convertStatus(status, current.bold, current.italic);
                    if (current.text.size() > 0)
                        textLine.content.push_back(current);
                    current.text.clear();

                    if (status == STATUS_NORMAL)
                        status = STATUS_BOLD;
                    else if (status == STATUS_BOLD)
                        status = STATUS_NORMAL;
                    else
                        current.text += "*";
                }
                else if (c == '_')
                {
                    convertStatus(status, current.bold, current.italic);
                    if (current.text.size() > 0)
                        textLine.content.push_back(current);
                    current.text.clear();

                    if (status == STATUS_NORMAL)
                        status = STATUS_ITALIC;
                    else if (status == STATUS_ITALIC)
                        status = STATUS_NORMAL;
                    else
                        current.text += "_";
                }
                else if (c == '$')
                {
                    convertStatus(status, current.bold, current.italic);
                    if (current.text.size() > 0)
                        textLine.content.push_back(current);
                    current.text.clear();

                    if (status == STATUS_NORMAL)
                        status = STATUS_ITALIC_BOLD;
                    else if (status == STATUS_ITALIC_BOLD)
                        status = STATUS_NORMAL;
                    else
                        current.text += "$";
                }
                else
                {
                    current.text += line[i];
                }
            }

            if (!current.text.empty())
            {
                convertStatus(status, current.bold, current.italic);
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
