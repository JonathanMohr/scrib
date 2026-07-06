# Syntax

If you write text, it will be put out exactly the way you wrote it, with a few exceptions:

# Constants

Constants are case-sensitive.

## Definition

You can define a constant by using the -C argument.

```sh
scrib test.scb -o out.md -CexampleName exampleValue
```

You can define a constant in the the code, using '%'.
Everything after the first space until the end of the line will be used as value.

```scb
%exampleName exampleValue
%otherConstant other value with multiple words

# Header

[...]
```

## Usage

For now, constants are just used by the backend.

# Formatting

Leading spaces will be ignored

## Header

- Start a line with '\#' for a header
- Start a line with '\#\#' for a subheader

## Style

The styles are fully independet of eachother.

- Bold: Put text between '*'
- Italic: Put text between '_'

# Escaping

There is a list of symbols that can be escaped by using a backslash. If a backslash is used without any of the recognized symbols following, it will be interpreted as normal text.

- "\\\#" will result in '\#'
- "\\%" will result in '%'
- "\\\*" will result in '\*'
- "\\\_" will result in '\_'
- "\\-" will result in '-'
- "\\\\" will result in '\\'
