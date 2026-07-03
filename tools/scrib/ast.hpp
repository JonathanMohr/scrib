#pragma once

#include <istream>
#include <string>
#include <variant>
#include <vector>

struct Text
{
    bool italic;
    bool bold;
    std::string text;
};

struct TextLine
{
    std::vector<Text> content;
};

struct EmptyLine
{
    char buffer;
};

struct Heading
{
    TextLine text;
    bool subheading;
};

struct Node
{
    std::variant<Heading, TextLine, EmptyLine> data;
};

struct Document
{
    std::vector<Node> nodes;
};

Document ParseDocument(std::istream& input);
