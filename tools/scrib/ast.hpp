#pragma once

#include <deque>
#include <istream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <cstdint>

struct Text
{
    bool italic;
    bool bold;
    std::string text;
};

struct TextParagraph
{
    std::vector<Text> content;
};

struct Heading
{
    TextParagraph text;
    bool subheading;
};

struct Node
{
    std::variant<Heading, TextParagraph> data;
};

struct Document
{
    std::vector<Node> nodes;
};

class Constants
{
public:
    Constants() {}
    ~Constants() = default;

    void addForce(const char* name, std::string value);

    bool add(const char* name, std::string value);
    inline bool add(const std::string& name, std::string value)
    { return add(name.c_str(), std::move(value)); }

    const std::string* get(const char* name) const;
    inline const std::string* get(const std::string& name) const
    { return get(name.c_str()); }

private:
    std::deque<std::string> values;
    std::unordered_map<std::string, std::deque<std::string>::size_type> constants;
};

Document ParseDocument(std::istream& input, Constants& constants);
