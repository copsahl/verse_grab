# Verse Grab

Small C program that uses https://bible-api.com.

## Libraries
- libcurl4
- [json-parser](https://github.com/json-parser/json-parser)

## Build
- `make`
  
## Run
- `./verse_grab <book> <chapter> [<verse>, <verse>-<verse>]`
- `./verse_grab romans 5 20`
- `./verse_grab genesis 1 1-10`
