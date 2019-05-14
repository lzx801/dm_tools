# dm_tools
## Tools
- clean_non_ascii.cpp
remove non ascii characters, extract id, content, title from JSON, prepare for segmentation
- underscore_text.cpp
remove phrase tags, translate multi-word phrases into underscore separated ones, like hello_world. This is used for word2vec.
- generate_NER_json.cpp
generate JSON with title, content and id, extract phrases and their positions into the NER field of JSON
## Usage
- `g++ -std=c++17 *.cpp exec`
- `./exec [file_location]`
