#define MAX_KEYBOARD_KEYS 1024
#define MAX_MOUSE_BUTTONS 32

struct mouse_input
{
    bool Buttons[MAX_MOUSE_BUTTONS];
    bool ButtonsProcessed[MAX_MOUSE_BUTTONS];
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
    return MouseInput.Buttons[Button];
}

internal bool
IsMouseButtonProcessed(unsigned int Button)
{
    return MouseInput.ButtonsProcessed[Button];
}

internal vec2
GetMousePosition()
{
    return MouseInput.Position;
}

void
KeyboardCallback(GLFWwindow *Window, int Key, int Scancode, int Action, int Mods)
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
MouseButtonCallback(GLFWwindow *Window, int Button, int Action, int Mods)
{
    if (Button >= 0 && Button < MAX_KEYBOARD_KEYS)
    {
        if (Action == GLFW_PRESS)
            MouseInput.Buttons[Button] = true;
        else if (Action == GLFW_RELEASE)
        {
            MouseInput.Buttons[Button] = false;
            MouseInput.ButtonsProcessed[Button] = false;
        }
    }
}

void
MouseCursorCallback(GLFWwindow *Window, double XPos, double YPos)
{
    MouseInput.Position = vec2{(float)XPos, (float)YPos};
}

internal void
SetInputCallbacks(GLFWwindow *Window)
{
    glfwSetKeyCallback(Window, KeyboardCallback);
    glfwSetMouseButtonCallback(Window, MouseButtonCallback);
    glfwSetCursorPosCallback(Window, MouseCursorCallback);
}