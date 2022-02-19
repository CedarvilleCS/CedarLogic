# Intro

The simulator is the new v2.4 logic core meant to be resilient to future design team introduced memory leaks and other bugs.

## Setup

### On Linux or Mac OS (because they're both Unix variants)

1. Clone the repo.
2. Open a terminal in this directory (`CedarLogic/Simulator`).
3. Run the following bash commands:

```bash
mkdir -p build

cd build/

cmake ../

cd ../

code .  # to open vs code and edit source

cd build

make  # to build the solution
```

Note, I saw something in VS-Code about treating Cmake as a first-class build system and it looked like it wanted to do the cmake configure and build
steps itself. I haven't investigated further but that's to say the steps above aren't the only way to do it.
