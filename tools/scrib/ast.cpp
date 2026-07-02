#include "ast.hpp"

Document ParseDocument(std::istream& input)
{
    Document doc;
    std::string line;

    while (std::getline(input, line))
    {
        if (!line.empty() && line[0] == '#')
        {
            std::size_t textStart = 1;
            while (textStart < line.size() && line[textStart] == ' ')
                textStart++;

            Heading heading;
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
