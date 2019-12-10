// @TODO: Os specific implementations of various file related things

internal EntireFile read_entire_file(char* file_name) {
    EntireFile result = {};

    FILE* in = fopen(file_name, "rb");
    if (in) {
        fseek(in, 0, SEEK_END);
        result.size = ftell(in);
        fseek(in, 0, SEEK_SET);

        result.data = malloc(result.size);
        fread(result.data, result.size, 1, in);
        fclose(in);
    }

    return result;
}
internal EntireFile read_entire_file_and_null_terminate(char* file_name) {
    EntireFile result = {};

    FILE* in = fopen(file_name, "rb");
    if (in) {
        fseek(in, 0, SEEK_END);
        result.size = ftell(in);
        fseek(in, 0, SEEK_SET);

        result.data = malloc(result.size + 1);
        fread(result.data, result.size, 1, in);
        ((u8*)result.data)[result.size] = 0;
        fclose(in);
    }

    return result;
}

internal String read_text_file(char* file_name) {
    EntireFile file = read_entire_file_and_null_terminate(file_name);
    String result = wrap_string(file.size, (char*)file.data);
    return result;
}
