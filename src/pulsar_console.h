#ifndef PULSAR_CONSOLE_H
#define PULSAR_CONSOLE_H

#define CONSOLE_COMMAND(name) void name(GameState* game_state, EditorState* editor, GameInput* input, String arguments)
typedef CONSOLE_COMMAND(ConsoleCommandFunction);

struct ConsoleCommand {
    ConsoleCommandFunction* f;
    String name;
    String help;
};

struct ConsoleState {
    RenderContext rc;

    Font* font;

    b32 open;
    b32 wide_open;
    f32 openness_t;

    f32 caret_breathing;

    b32 in_focus;

    u32  caret_pos;
    u32  input_buffer_count;
    char input_buffer[255];
};

inline String input_buffer_as_string(ConsoleState* console) {
    String result = wrap_string(console->input_buffer_count, console->input_buffer);
    return result;
}

#if PULSAR_DEBUG
struct DebugBreakMarker {
    String name;
    b32 enabled;
};

global u32 debug_break_array_count = 0;
global DebugBreakMarker debug_break_array[128];

inline DebugBreakMarker* add_debug_break_(String name) {
    assert(debug_break_array_count < ARRAY_COUNT(debug_break_array));
    DebugBreakMarker* result = debug_break_array + debug_break_array_count++;
    result->name    = name;
    result->enabled = false;
    return result;
}

#define ADD_DEBUG_BREAK(NAME)                                                                           \
    {                                                                                                   \
        local_persist DebugBreakMarker* debug_break_##NAME = add_debug_break_(string_literal(#NAME));   \
        if (debug_break_##NAME->enabled) {                                                              \
            log_print(LogLevel_Info, "Hit breakpoint '%.*s'", string_expand(debug_break_##NAME->name)); \
            __debugbreak();                                                                             \
            debug_break_##NAME->enabled = false;                                                        \
        }                                                                                               \
    }
#else
#define ADD_DEBUG_BREAK(...) /* ... */
#endif

#endif /* PULSAR_CONSOLE_H */
