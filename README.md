# scrib

scrib is a markup language and compiler that lets you write once and compile to Markdown, man pages (TROFF), and (eventually) more formats.

Initially, this project was started out of the annoyance of maintaining separate docs for Markdown and man pages.

scrib is in early development. Core language features are relatively stable, but breaking changes are still possible as the project evolves.

## Installation

### Homebrew (macOS/Linux)

scrib is distributed via a custom Homebrew tap (third-party formula repository).
Since `scrib` is a short, generic name, using the full path avoids potential future conflicts with other taps or Homebrew Core. As of the time of writing this README, there are no known conflicts with Homebrew Core.
As of Homebrew 6.0.0, third-party taps must be explicitly trusted before their formulae can be loaded.

```sh
# Load the Homebrew tap
brew tap JonathanMohr/tap

# Homebrew 6+:
brew trust JonathanMohr/tap # trust the whole tap
# or
brew trust ---formula JonathanMohr/tap/scrib # Only trust the single formula

# Install scrib
brew install JonathanMohr/tap/scrib
# Or (if there are no conflicts)
brew install scrib
```

### Scoop (Windows)

scrib is distributed via a custom Scoop bucket (third-party repository).
Since `scrib` is a short, generic name, using the full name avoids potential future conflicts with other buckets or Scoop named buckets. As of the time of writing this README, there are no known conflicts with Scoop core.

```sh
# Load the Scoop bucket (You can choose a different name)
scoop bucket add JonathanMohr https://github.com/JonathanMohr/scoop-bucket

# Install scrib
scoop install JonathanMohr/scrib # Or the name you chose
# Or (if there are no conflicts)
scoop install scrib
```

### Build from source

If you do not use Homebrew or Scoop, or just prefer to build manually, you will need the following tools available in your PATH:

- Python 3.10+
- clang, clang++
- lld
- llvm-ar
- llvm-dsymutil or dsymutil (only required when targeting macOS)

The `ci` module will build scrib and run a small set of tests.

```sh
# Clone and enter the repository
git clone https://github.com/JonathanMohr/scrib
cd scrib

# Build
python3 -m ci # use 'python' instead of 'python3' on Windows
```

After building, `./dist` will contain the install prefix (`/bin`, etc.). Copy its contents to a location of your choice (e.g. `~/.local/scrib`), then either add its directories to your `PATH` (and potentially `MANPATH`, etc.), or create symlinks into a directory that's already on it.

## Usage

After you have installed scrib, you can start using it.

You can create `input.scb` (Or any other name):

```scb
# Interesting header

## Very important category

This content is very *important*.

## Less important category

This content is much _less important_ than before.

## Confidential category

This content is extremely *important* and *_confidential_*.
```

Then you can compile it to Markdown using the following command:

```sh
scrib input.scb -o output.md -f markdown
```

Then you will get the output that will look similar to this:

```md
# Interesting header

## Very important category

This content is very **important**.

## Less important category

This content is much *less important* than before.

## Confidential category

This content is extremely **important** and __*confidential*__.
```

- [CLI reference](docs/cli.md) - all available flags and options.
- [Syntax reference](docs/syntax.md) - the full `.scb` language reference.

## Features

- Compiles to Markdown and man pages (TROFF), with more output formats planned

- Define constants via CLI flag or inline in the source

- Basic formatting: headers, bold, italic

- Escaping for reserved symbols

## License

This project is licensed under [Apache 2.0](LICENSE).
