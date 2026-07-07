# CLI

## Usage

```sh
scrib <input.scb> (-o <output>) [options]
```

## Options

### `-h`/`--help`

Print a help message and exit.

### `-v`/`--version`

Print a version message and exit.

### `-o <file>`

Specify the output file.

### `-f <format>`/`--format <format>`

Specify the output format.

### `-C<name> <value>` (not `-C<name>=<value>`!)

Define a constants and its value.

### `--strict`

Use a strict mode. The output might be longer, but more consistent with variants of the output.

## Formats

### markdown

This will output Markdown following the CommonMark and GFM specification.

#### Constants

This does not use any constants

### man-troff

This will output TROFF which will use macros defined by man for man-pages.

#### Constants

- `man-title` (default: 'scrib') - Title for the man-page

- `man-section` (default: '1') - Section for the man-page

- `man-source` (default: none, unless `man-manual` is set, then 'source') - Source for the man-page

- `man-manual` (default: none) - Manual for the man-page
