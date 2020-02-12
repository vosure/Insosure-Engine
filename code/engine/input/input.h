#define MAX_KEYBOARD_KEYS 1024
#define MAX_MOUSE_BUTTONS 32

struct mouse_input
{
    bool MouseButtons[MAX_MOUSE_BUTTONS];
    vec2 Position;
};

struct keyboard_input
{
    bool Keys[MAX_KEYBOARD_KEYS];
    bool KeysProcessed[MAX_KEYBOARD_KEYS];
};

static mouse_input MouseInput;
static keyboard_input KeyboardInput;

internal bool 
IsKeyPressed(keyboard_input Input, unsigned int Keycode)
{
    return Input.Keys[Keycode];
}

internal bool
IsMouseButtonPressed(mouse_input Input, unsigned int Button)
{
    return Input.MouseButtons[Button];
}
internal vec2 
GetMousePosition(mouse_input Input)
{
    return Input.Position;
}

internal void 
keyboard_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
}

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
}

void mouse_cursor_callback(GLFWwindow * window, double xpos, double ypos)
{
}