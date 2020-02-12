#define MAX_KEYBOARD_KEYS 1024
#define MAX_MOUSE_BUTTONS 32

struct mouse_input
{
    bool MouseButtons[MAX_MOUSE_BUTTONS];
    bool MouseButtonsProcessed[MAX_MOUSE_BUTTONS];
    vec2 Position;
};

struct keyboard_input
{
    bool Keys[MAX_KEYBOARD_KEYS];
    bool KeysProcessed[MAX_KEYBOARD_KEYS];
};

static keyboard_input KeyboardInput;
static mouse_input MouseInput;

internal bool
IsKeyPressed(unsigned int Keycode)
{
    return KeyboardInput.Keys[Keycode];
}

internal bool
IsKeyProcessed(unsigned int Keycode)
{
    return KeyboardInput.KeysProcessed[Keycode];
}

internal bool
IsMouseButtonPressed(unsigned int Button)
{
    return MouseInput.MouseButtons[Button];
}
internal vec2
GetMousePosition()
{
    return MouseInput.Position;
}

void
keyboard_callback(GLFWwindow *Window, int Key, int Scancode, int Action, int Mods)
{
    if (Key >= 0 && Key < MAX_KEYBOARD_KEYS)
    {
        if (Action == GLFW_PRESS)
            KeyboardInput.Keys[Key] = true;      
        else if (Action == GLFW_RELEASE)
        {
            KeyboardInput.Keys[Key] = false;
            KeyboardInput.KeysProcessed[Key] = false;
        }
    }
}

void 
mouse_button_callback(GLFWwindow *Window, int Button, int Action, int Mods)
{
    if (Button >= 0 && Button < MAX_KEYBOARD_KEYS)
    {
        if (Action == GLFW_PRESS)
            MouseInput.MouseButtons[Button] = true;
        else if (Action == GLFW_RELEASE)
        {
            MouseInput.MouseButtons[Button] = false;
            MouseInput.MouseButtonsProcessed[Button] = false;
        }
    }
}

void 
mouse_cursor_callback(GLFWwindow *Window, double XPos, double YPos)
{
    MouseInput.Position = vec2{(float)XPos, (float)YPos};
}

internal void 
SetInputCallbacks(GLFWwindow *Window)
{
    glfwSetKeyCallback(Window, keyboard_callback);
    glfwSetMouseButtonCallback(Window, mouse_button_callback);
    glfwSetCursorPosCallback(Window, mouse_cursor_callback);
}