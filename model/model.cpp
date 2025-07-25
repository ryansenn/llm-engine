#include "model.h"
#include <fstream>

void Model::load(std::string path){
    std::ifstream f(path, std::ios::binary);
    f.seekg(0);
    
    char buffer[1024];
    uint8_t entry_type;
    std::string key;

    uint8_t value_type;


    while (f.peek() != EOF){
        f.read(reinterpret_cast<char*>(&entry_type), sizeof(entry_type));

        std::cout << entry_type << std::endl;

        // Metadata entry
        if (entry_type == 0) {
            f.read(buffer, 50);
            key.assign(buffer, 50);
            key.erase(key.find('\0'));

            f.read(reinterpret_cast<char*>(&value_type), sizeof(uint8_t));

            switch(value_type){
                // int
                case 0:
                    int32_t value;

                    f.read(reinterpret_cast<char*>(&value), sizeof(int32_t));

                    if (key == "vocab_size") {
                        config.vocab_size = value;
                    } else if (key == "hidden_size") {
                        config.hidden_size = value;
                    }
                    break;
                // float
                case 1:
                    break;
                //string
                case 2:
                    break;
            }
        }

        // Tensor entry
        else if (entry_type == 1) {
            Tensor* tensor;

            f.read(buffer, 50);
            key.assign(buffer, 50);
            key.erase(key.find('\0'));

            f.read(reinterpret_cast<char*>(&value_type), sizeof(value_type));

            switch(value_type){
                // bfloat16
                case 0:
                    break;
                // float16
                case 1:
                    break;
                // float32
                case 2:
                    int64_t size;
                    f.read(reinterpret_cast<char*>(&size), sizeof(int64_t));

                    void* data = std::malloc(size);
                    f.read(reinterpret_cast<char*>(data), size);

                    tensor = new Tensor(key, data);
                    break;
            }

            if (key == "model.embed_tokens.weight"){
                token_embedding_table = tensor;
            }

        }
        else {
            std::cerr << "FATAL: Unknown entry_type " << entry_type << " in model file\n";
            std::exit(EXIT_FAILURE);
        }
    }
}