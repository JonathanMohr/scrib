#pragma once

#include <istream>
#include <string>
#include <variant>
#include <vector>

struct Heading
{
    std::string text;
};

struct Text
{
    std::string content;
};

struct Node
{
    std::variant<Heading, Text> data;
};

struct Document
{
    std::vector<Node> nodes;
};

Document ParseDocument(std::istream& input);
