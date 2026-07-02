#include "ast.hpp"

Document ParseDocument(std::istream& input)
{
    Document doc;
    std::string line;

    while (std::getline(input, line))
    {
        if (!line.empty() && line[0] == '#')
        {
            std::size_t level = 0;
            while (level < line.size() && line[level] == '#')
                level++;

            std::size_t textStart = level;
            while (textStart < line.size() && line[textStart] == ' ')
                textStart++;

            Heading heading;
            heading.level = static_cast<uint32_t>(level);
            heading.text = line.substr(textStart);

            doc.nodes.push_back(Node{heading});
        }
        else
        {
            Text text;
            text.content = line;

            doc.nodes.push_back(Node{text});
        }
    }

    return doc;
}
