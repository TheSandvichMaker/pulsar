internal EntireFile read_entire_file(char* file_name, Allocator allocator, u32 flags = 0) {
    EntireFile result = {};

    FILE* in = fopen(file_name, "rb");
    if (in) {
        fseek(in, 0, SEEK_END);
        result.size = ftell(in);
        fseek(in, 0, SEEK_SET);

        size_t alloc_size = result.size;
        if (flags & ReadFileEntireFile_NullTerminate) {
            alloc_size += 1;
        }

        result.data = allocate(allocator, alloc_size, no_clear());

        if (result.data) {
            fread(result.data, result.size, 1, in);

            if (flags & ReadFileEntireFile_NullTerminate) {
                (cast(u8*) result.data)[result.size] = 0;
            }
        } else {
            result.size = 0;
        }

        fclose(in);
    }

    return result;
}

internal String read_text_file(char* file_name, Allocator allocator) {
    EntireFile file = read_entire_file(file_name, allocator, ReadFileEntireFile_NullTerminate);
    String result = wrap_string(file.size, cast(char*) file.data);
    return result;
}
