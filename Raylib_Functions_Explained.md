# Raylib Functions — Complete Explanation
# Used in Algeria History DB GUI

---

## HOW RAYLIB WORKS (big picture)

Raylib is a C library that gives you a window and lets you draw shapes, text,
and images inside it. Every frame your program:

1. Calls BeginDrawing()       ← tells raylib "I'm about to draw"
2. Clears the screen
3. Draws everything (shapes, text, UI)
4. Calls EndDrawing()         ← tells raylib "show it on screen"

This loop runs 60 times per second. The window is just a grid of pixels.
Raylib redraws ALL pixels from scratch every frame — nothing is remembered
from the previous frame.

Coordinate system:
  - (0, 0) is the TOP-LEFT corner
  - X increases going RIGHT
  - Y increases going DOWN

```
(0,0) ──────────────→ X
  │
  │
  ↓
  Y
```

---

## SECTION 1 — WINDOW & PROGRAM LIFECYCLE

───────────────────────────────────────────────────────────────────────────────
### SetConfigFlags(flags)

```c
SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
```

Sets options for the window BEFORE it is created. Must be called before InitWindow.

- FLAG_WINDOW_RESIZABLE  → the user can drag the window edge to resize it
- FLAG_MSAA_4X_HINT      → enables 4x anti-aliasing (smoother edges on shapes)

The | operator combines multiple flags (bitwise OR).

───────────────────────────────────────────────────────────────────────────────
### InitWindow(width, height, title)

```c
InitWindow(1100, 640, "Algeria History DB — NSCS 2025/2026");
```

Creates the actual OS window.
- width  = 1100 pixels wide
- height = 640  pixels tall
- title  = text shown in the window title bar

Must be called before any drawing function. Everything else depends on this.

───────────────────────────────────────────────────────────────────────────────
### SetTargetFPS(fps)

```c
SetTargetFPS(60);
```

Tells raylib to run the main loop at most 60 times per second.
Without this the loop would run as fast as the CPU allows (thousands of times/sec)
burning power and making the program impossible to control.
60 FPS = one frame every ~16.6 milliseconds.

───────────────────────────────────────────────────────────────────────────────
### SetExitKey(key)

```c
SetExitKey(0);
```

By default raylib closes the window when the user presses Escape.
Passing 0 (which means KEY_NULL) disables that default behaviour.
This lets us use Escape ourselves — we use it to close modal dialogs
instead of exiting the whole program.

───────────────────────────────────────────────────────────────────────────────
### WindowShouldClose()

```c
while(!WindowShouldClose()) {
    // main loop
}
```

Returns true when the user clicks the X button on the window, or presses the
exit key (which we disabled above). Used as the condition of the main loop.
When it returns true, the loop exits and the program cleans up and quits.

───────────────────────────────────────────────────────────────────────────────
### CloseWindow()

```c
CloseWindow();
```

Destroys the window and frees all raylib's internal resources.
Always called at the very end of main(), after the loop exits.

───────────────────────────────────────────────────────────────────────────────
### BeginDrawing()

```c
BeginDrawing();
```

Signals the start of a new frame. Must be called before any Draw* function.
Internally raylib prepares the framebuffer (the off-screen pixel canvas)
so you draw into it without the user seeing half-drawn results.

───────────────────────────────────────────────────────────────────────────────
### EndDrawing()

```c
EndDrawing();
```

Signals the end of the frame.
- Copies the framebuffer to the screen (swap buffers / present).
- Waits the right amount of time to hit the target FPS.
- Polls OS events (mouse moves, key presses, window resize, etc.)
Must be paired with every BeginDrawing().

───────────────────────────────────────────────────────────────────────────────
### ClearBackground(color)

```c
ClearBackground(C_SURF);   // C_SURF = white {255,255,255,255}
```

Fills every pixel of the screen with one solid color.
Always the first thing called after BeginDrawing().
Without this, leftover pixels from the previous frame show through.

---

## SECTION 2 — TIME

───────────────────────────────────────────────────────────────────────────────
### GetFrameTime()

```c
float dt = GetFrameTime();
if(gToastTimer > 0) gToastTimer -= dt;
```

Returns the time in seconds that the last frame took to complete.
At 60 FPS this is roughly 0.01666 seconds (1/60).
Used to make animations and timers frame-rate independent.
Here we use it to count down the toast notification timer:
subtract the elapsed time each frame until the timer reaches 0.

───────────────────────────────────────────────────────────────────────────────
### GetTime()

```c
if((int)(GetTime() * 2) % 2 == 0)
    DrawLineEx(...);   // blinking cursor
```

Returns the total time in seconds since the program started.
Multiplying by 2 gives a value that changes 2 times per second.
Taking % 2 alternates between 0 and 1 — this makes the cursor blink
at 1 Hz (on for 0.5s, off for 0.5s).

---

## SECTION 3 — SCREEN SIZE

───────────────────────────────────────────────────────────────────────────────
### GetScreenWidth()

```c
int W = GetScreenWidth();
DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0,0,0,120});
```

Returns the current width of the window in pixels.
Because we set FLAG_WINDOW_RESIZABLE, the user can resize the window,
so we ask for the current size every frame instead of using a constant.

───────────────────────────────────────────────────────────────────────────────
### GetScreenHeight()

```c
int H = GetScreenHeight();
```

Returns the current height of the window in pixels. Same idea as above.
Used to position things at the bottom of the window (status bar, toast).

---

## SECTION 4 — FONTS & TEXT

───────────────────────────────────────────────────────────────────────────────
### LoadFontEx(fileName, fontSize, codepoints, codepointCount)

```c
Font font = LoadFontEx("resources/JetBrainsMono-Regular.ttf", 18, NULL, 0);
```

Loads a TrueType (.ttf) font file from disk and rasterizes it at the given size.
- fileName          = path to the .ttf file
- fontSize          = size in pixels to rasterize (18px here)
- codepoints        = NULL means use default character set (ASCII)
- codepointCount    = 0 means use default

Returns a Font struct. If the file does not exist, font.texture.id will be 0.
We check for that and fall back to the built-in font.

───────────────────────────────────────────────────────────────────────────────
### GetFontDefault()

```c
if(font.texture.id == 0) font = GetFontDefault();
```

Returns raylib's built-in bitmap font that is always available.
It is a small pixelated font — not as clean as JetBrainsMono but always works
without any external file.

───────────────────────────────────────────────────────────────────────────────
### UnloadFont(font)

```c
UnloadFont(font);
```

Frees the GPU texture and CPU memory used by the font.
Called at the end of the program after the loop exits.

───────────────────────────────────────────────────────────────────────────────
### DrawTextEx(font, text, position, fontSize, spacing, color)

```c
DrawTextEx(font, "Algeria History DB", (Vector2){14, 14}, 14, 1, C_TXT1);
```

Draws a string of text using a loaded font.
- font      = the Font struct loaded earlier
- text      = the string to draw (null-terminated C string)
- position  = (Vector2){x, y} — top-left corner of the text
- fontSize  = size to draw the text in pixels
- spacing   = extra pixels between characters (1 = normal)
- color     = RGBA color of the text

Used everywhere — labels, table cells, button text, sidebar items, modal text.

───────────────────────────────────────────────────────────────────────────────
### MeasureTextEx(font, text, fontSize, spacing)

```c
float tw = MeasureTextEx(font, txt, 11, 1).x;
```

Measures how wide (and tall) a string would be if drawn with DrawTextEx.
Returns a Vector2 where:
- .x = width  in pixels
- .y = height in pixels

We only use .x (width) to:
- Center text inside buttons:  x + (buttonWidth - textWidth) / 2
- Right-align tags:            draw background first at measured width + padding
- Find cursor position:        x + measured width of typed text so far
- Wrap text:                   check if next word would exceed maxWidth

---

## SECTION 5 — DRAWING SHAPES

───────────────────────────────────────────────────────────────────────────────
### DrawRectangle(x, y, width, height, color)

```c
DrawRectangle((int)x, (int)y, (int)w, (int)h, c);
```

Draws a solid filled rectangle.
- x, y         = top-left corner position (integers)
- width, height = size in pixels (integers)
- color        = RGBA fill color

This is the most-used function in the GUI. Used for:
- Background panels (sidebar, topbar, statusbar)
- Table row backgrounds (normal, hovered, selected)
- The dark overlay behind modals

Takes integers. Our wrapper `drect()` casts floats to int for convenience.

───────────────────────────────────────────────────────────────────────────────
### DrawRectangleRounded(rec, roundness, segments, color)

```c
DrawRectangleRounded((Rectangle){x, y, w, h}, 0.35f, 4, bg);
```

Draws a solid filled rectangle with rounded corners.
- rec        = (Rectangle){x, y, width, height} — position and size
- roundness  = 0.0 to 1.0 — how round the corners are
                 0.0 = sharp square corners
                 0.5 = semi-circle ends
                 1.0 = full circle/ellipse
- segments   = how smooth the curves are (4 = fast, 8+ = smoother)
- color      = fill color

Used for: buttons, input fields, tags/badges, modal windows, toast notification.

───────────────────────────────────────────────────────────────────────────────
### DrawRectangleLinesEx(rec, lineThick, color)

```c
DrawRectangleLinesEx((Rectangle){sx, 10, 162, 26}, 1, C_BORDER);
```

Draws only the OUTLINE of a rectangle (no fill).
- rec       = (Rectangle){x, y, width, height}
- lineThick = thickness of the border line in pixels
- color     = color of the border

Used for the search box border. Distinguishes from DrawRectangleRoundedLinesEx
because the search box has sharp corners.

───────────────────────────────────────────────────────────────────────────────
### DrawRectangleRoundedLinesEx(rec, roundness, segments, lineThick, color)

```c
DrawRectangleRoundedLinesEx((Rectangle){x, y, w, h}, 0.35f, 4, 1, C_BORDER);
```

Draws only the OUTLINE of a rounded rectangle.
Same parameters as DrawRectangleRounded but draws the border only, no fill.
Used together with DrawRectangleRounded to create filled+bordered elements
like buttons and input fields:

```c
DrawRectangleRounded(rec, 0.35f, 4, bg);         // fill
DrawRectangleRoundedLinesEx(rec, 0.35f, 4, 1, br); // border on top
```

───────────────────────────────────────────────────────────────────────────────
### DrawLineEx(startPos, endPos, thick, color)

```c
DrawLineEx((Vector2){SIDEBAR_W, 0}, (Vector2){SIDEBAR_W, WIN_H}, 1, C_BORDER);
```

Draws a line between two points with a specified thickness.
- startPos = (Vector2){x1, y1} — start point
- endPos   = (Vector2){x2, y2} — end point
- thick    = line thickness in pixels (1 = single pixel, 1.5f = slightly thicker)
- color    = line color

Used for:
- Vertical divider between sidebar and main content
- Vertical divider between main content and detail panel
- Horizontal separator lines (below topbar, above statusbar, between sections)
- The blinking cursor inside input fields (a very thin vertical line)
- Arrows between nodes in stack/queue view

───────────────────────────────────────────────────────────────────────────────
### DrawCircle(centerX, centerY, radius, color)

```c
DrawCircle(17, (int)(y + 14), 3, C_TXT3);
```

Draws a filled solid circle.
- centerX = x coordinate of the center
- centerY = y coordinate of the center
- radius  = radius in pixels
- color   = fill color

Used in the sidebar as the small bullet dot (radius 3) before each menu item label.

───────────────────────────────────────────────────────────────────────────────
### DrawTriangle(v1, v2, v3, color)

```c
DrawTriangle(
    (Vector2){ax,    ay + gap + 10},   // bottom tip (arrow point)
    (Vector2){ax - 5, ay + gap + 2},   // top left
    (Vector2){ax + 5, ay + gap + 2},   // top right
    C_TXT3
);
```

Draws a filled solid triangle defined by 3 points.
Used in the stack/queue structural view to draw the arrowhead at the bottom
of the connecting line between two nodes.
The three vertices form a downward-pointing triangle (▼).

Note: raylib requires vertices in counter-clockwise order.

---

## SECTION 6 — CLIPPING

───────────────────────────────────────────────────────────────────────────────
### BeginScissorMode(x, y, width, height)

```c
BeginScissorMode(tableX, tableY + 28, tableW, tableH - 28);
```

Activates a "scissor rectangle" — a clipping region.
After this call, NOTHING drawn outside the given rectangle is visible.
Pixels outside the region are simply discarded.

Used in the table to prevent rows from drawing over the header or outside
the table area when the user scrolls. Without this, rows that are half-visible
at the top or bottom edge would bleed through into the topbar or statusbar.

Parameters:
- x, y         = top-left corner of the clipping region
- width, height = size of the clipping region

───────────────────────────────────────────────────────────────────────────────
### EndScissorMode()

```c
EndScissorMode();
```

Turns off the scissor clipping. Everything drawn after this is visible anywhere
on screen again. Must be paired with every BeginScissorMode().

---

## SECTION 7 — MOUSE INPUT

───────────────────────────────────────────────────────────────────────────────
### GetMousePosition()

```c
Vector2 mp = GetMousePosition();
```

Returns the current (x, y) position of the mouse cursor as a Vector2.
Called once per draw function and stored in `mp` for reuse.
Used to check hover states and click detection.

───────────────────────────────────────────────────────────────────────────────
### IsMouseButtonPressed(button)

```c
if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    // handle click
}
```

Returns true on the EXACT FRAME the mouse button was pressed down.
- Returns true only once per click (the frame the button goes down)
- Returns false every other frame, even while held down

MOUSE_LEFT_BUTTON = 0 (the constant for left click).
Used for every clickable element: buttons, table rows, sidebar items, form fields.

───────────────────────────────────────────────────────────────────────────────
### GetMouseWheelMove()

```c
float wh = GetMouseWheelMove();
if(wh != 0 && CheckCollisionPointRec(mp, tableRect))
    gScrollY -= (int)wh * 2;
```

Returns how much the mouse wheel moved this frame:
- Positive = scrolled UP (wheel forward)
- Negative = scrolled DOWN (wheel backward)
- 0        = no scroll this frame

We multiply by 2 to scroll 2 rows per notch. We check first that the mouse
is over the table (using CheckCollisionPointRec) so scrolling elsewhere
does not affect the table.

───────────────────────────────────────────────────────────────────────────────
### CheckCollisionPointRec(point, rectangle)

```c
bool hov = CheckCollisionPointRec(mp, (Rectangle){0, y, SIDEBAR_W, 28});
bool hSave = CheckCollisionPointRec(mp, (Rectangle){mx+20, fy, mw/2-25, 32});
```

Returns true if a point (x,y) is inside a rectangle.
- point     = (Vector2){x, y} — usually the mouse position
- rectangle = (Rectangle){x, y, width, height}

This is HOW BUTTONS WORK in this GUI. Raylib has no built-in button widget.
Instead we:
1. Draw a rectangle where the button should be
2. Call CheckCollisionPointRec every frame to check if the mouse is over it
3. If yes, draw it highlighted (hover state)
4. If IsMouseButtonPressed is also true, handle the click

Used for every interactive element: buttons, table rows, sidebar items,
form fields (to change focus), search box.

---

## SECTION 8 — KEYBOARD INPUT

───────────────────────────────────────────────────────────────────────────────
### GetCharPressed()

```c
int key = GetCharPressed();
while(key > 0) {
    int l = (int)strlen(tgt);
    if(l < mxl && key >= 32) { tgt[l] = (char)key; tgt[l+1] = 0; }
    key = GetCharPressed();
}
```

Returns the Unicode codepoint of the next character typed this frame.
Returns 0 when there are no more characters waiting.
Must be called in a loop because the user could type multiple characters
between frames (or hold a key and get repeats).

key >= 32 filters out non-printable control characters (space=32 is the first printable).
The typed character is appended to the current input buffer (fName, fBirth, etc.)

───────────────────────────────────────────────────────────────────────────────
### IsKeyPressed(key)

```c
if(IsKeyPressed(KEY_ESCAPE) && gModal != MODAL_NONE) gModal = MODAL_NONE;
if(IsKeyPressed(KEY_BACKSPACE)) { int l = strlen(tgt); if(l>0) tgt[l-1] = 0; }
if(IsKeyPressed(KEY_TAB) && !isSearch) { fFocus = (fFocus+1) % (maxF+1); }
```

Returns true on the EXACT FRAME a key was pressed down.
Like IsMouseButtonPressed — fires once per keypress, not continuously while held.

Key constants used:
- KEY_ESCAPE    = Esc key    → closes the open modal
- KEY_BACKSPACE = Backspace  → deletes the last character in the input field
- KEY_TAB       = Tab key    → moves focus to the next input field

---

## SECTION 9 — COLORS & DATA TYPES

───────────────────────────────────────────────────────────────────────────────
### Color

```c
#define C_ACCENT  CLITERAL(Color){ 29, 158, 117, 255}
(Color){0, 0, 0, 120}
```

A struct with 4 bytes: { r, g, b, a }
- r = red   (0-255)
- g = green (0-255)
- b = blue  (0-255)
- a = alpha (0=fully transparent, 255=fully opaque)

Example colors used in this GUI:
- C_SURF     = {255,255,255,255} white (main background)
- C_BG       = {245,244,240,255} off-white (sidebar, table header)
- C_ACCENT   = { 29,158,117,255} green (active item, add button)
- C_BORDER   = {220,218,210,255} light grey (dividers, input borders)
- C_TXT1     = { 26, 25, 22,255} near-black (primary text)
- C_TXT2     = { 90, 88, 82,255} dark grey (secondary text)
- C_TXT3     = {140,138,130,255} mid grey (labels, placeholders)
- C_SEL      = {225,245,238,255} light green (selected row)
- C_RED_BG   = {252,235,235,255} light red (delete button background)
- {0,0,0,120}                    semi-transparent black (modal overlay)

CLITERAL(Color) is a raylib macro that works in both C and C++ to initialize
a struct literal. In C you can also just write (Color){r,g,b,a}.

───────────────────────────────────────────────────────────────────────────────
### Vector2

```c
(Vector2){14, 14}
(Vector2){cx, ty}
```

A struct with two floats: { float x; float y; }
Used for 2D positions and sizes.
DrawTextEx takes a Vector2 for position.
DrawLineEx takes two Vector2 values (start and end).

───────────────────────────────────────────────────────────────────────────────
### Rectangle

```c
(Rectangle){x, y, width, height}
(Rectangle){mx, my, mw, mh}
```

A struct with four floats: { float x; float y; float width; float height; }
x, y = top-left corner position.
width, height = size.
Used by DrawRectangleRounded, DrawRectangleRoundedLinesEx, CheckCollisionPointRec,
BeginScissorMode.

───────────────────────────────────────────────────────────────────────────────
### Font

```c
Font font = LoadFontEx("resources/JetBrainsMono-Regular.ttf", 18, NULL, 0);
```

A struct that holds a loaded font (texture on the GPU + glyph metrics).
Passed to DrawTextEx and MeasureTextEx.
Must be loaded once and unloaded at the end.

---

## SECTION 10 — FLAGS & CONSTANTS

───────────────────────────────────────────────────────────────────────────────
### FLAG_WINDOW_RESIZABLE

Used in SetConfigFlags. Tells the OS to allow the window to be resized by dragging.

───────────────────────────────────────────────────────────────────────────────
### FLAG_MSAA_4X_HINT

Used in SetConfigFlags. Requests 4x Multi-Sample Anti-Aliasing from the GPU.
Makes diagonal lines and curved shapes appear smoother.
"Hint" means the GPU may ignore it if not supported.

───────────────────────────────────────────────────────────────────────────────
### MOUSE_LEFT_BUTTON

The constant 0. Passed to IsMouseButtonPressed to check the left click.

───────────────────────────────────────────────────────────────────────────────
### KEY_ESCAPE, KEY_BACKSPACE, KEY_TAB

Integer constants for specific keyboard keys.
Passed to IsKeyPressed.

---

## SECTION 11 — HOW EVERYTHING CONNECTS (frame timeline)

Every frame (60 times/sec) this happens in order:

```
GetFrameTime()          → how long did last frame take?
    ↓
gToastTimer -= dt       → count down toast
    ↓
IsKeyPressed(ESCAPE)    → did user press Escape? close modal
    ↓
GetCharPressed()        → any text typed? add to search/form buffer
IsKeyPressed(BACKSPACE) → delete last char
    ↓
BeginDrawing()          → start drawing to framebuffer
ClearBackground()       → white fill
    ↓
GetScreenWidth/Height() → current window size
GetMousePosition()      → where is the mouse right now?
    ↓
── drawSidebar ──────────────────────────────────────────
DrawRectangle           → sidebar background
DrawTextEx              → title, section labels, item labels
DrawCircle              → bullet dots
DrawLineEx              → separator lines
CheckCollisionPointRec  → is mouse over an item? (hover)
IsMouseButtonPressed    → was item clicked? change tab/view
    ↓
── topbar ───────────────────────────────────────────────
DrawRectangle           → topbar background
DrawTextEx              → current view title
DrawRectangleLinesEx    → search box border
DrawTextEx              → search box text / placeholder
DrawRectangleRounded    → add button fill
DrawRectangleRoundedLinesEx → add button border
DrawTextEx              → add button label
CheckCollisionPointRec  → button hover detection
IsMouseButtonPressed    → button click → open modal
    ↓
── drawTable (if VIEW_LIST) ─────────────────────────────
DrawRectangle           → header background
DrawTextEx              → column headers
BeginScissorMode        → clip rows to table area
  loop rows:
    DrawRectangle       → row background (normal/hover/selected)
    DrawLineEx          → row bottom border
    DrawTextEx          → cell text (index, name, dates)
    DrawRectangleRounded→ type tag background
    DrawTextEx          → type tag text
    CheckCollisionPointRec → hover?
    IsMouseButtonPressed   → click → gSelected = this row
EndScissorMode
GetMouseWheelMove       → scroll table
    ↓
── drawStructView (if VIEW_STACK or VIEW_QUEUE) ──────────
loop nodes:
    DrawRectangleRounded     → node box
    DrawRectangleRoundedLinesEx → node box border
    DrawTextEx               → node label
    DrawLineEx               → connecting arrow line
    DrawTriangle             → arrowhead
DrawTextEx                   → HEAD/TAIL or TOP/BOTTOM labels
    ↓
── drawDetail ───────────────────────────────────────────
DrawRectangle           → panel background
DrawLineEx              → left border
DrawTextEx              → name, dates, description, flags
DrawRectangleRounded + DrawRectangleRoundedLinesEx → Edit button
DrawRectangleRounded + DrawRectangleRoundedLinesEx → Delete button
CheckCollisionPointRec  → button hover
IsMouseButtonPressed    → button click → open modal
    ↓
── drawModal (if gModal != MODAL_NONE) ──────────────────
DrawRectangle           → dark overlay (semi-transparent)
DrawRectangleRounded    → modal white box
DrawTextEx              → modal title
DrawLineEx              → title underline
DrawRectangleRounded    → input field backgrounds
DrawRectangleRoundedLinesEx → input borders (green if focused)
DrawTextEx              → input labels and values
DrawLineEx              → blinking cursor (GetTime trick)
DrawRectangleRounded + borders → Save/Cancel/Delete buttons
CheckCollisionPointRec  → button hover
IsMouseButtonPressed    → button click → save/cancel/delete
GetCharPressed          → type into focused field
IsKeyPressed(BACKSPACE) → delete char
IsKeyPressed(TAB)       → next field
    ↓
── toast ────────────────────────────────────────────────
if gToastTimer > 0:
    MeasureTextEx       → measure toast message width
    DrawRectangleRounded→ dark pill background
    DrawTextEx          → message text
    ↓
EndDrawing()            → present frame to screen, wait for 60fps
```

---

## QUICK REFERENCE TABLE

| Function                      | Category    | What it does in one line                              |
|-------------------------------|-------------|-------------------------------------------------------|
| SetConfigFlags()              | Window      | Set window options before creation                    |
| InitWindow()                  | Window      | Create the OS window                                  |
| SetTargetFPS()                | Window      | Cap frame rate at N frames/sec                        |
| SetExitKey()                  | Window      | Change (or disable) the auto-close key                |
| WindowShouldClose()           | Window      | True when user clicks X — main loop condition         |
| CloseWindow()                 | Window      | Destroy window and free raylib resources              |
| BeginDrawing()                | Frame       | Start a new frame (must precede all drawing)          |
| EndDrawing()                  | Frame       | Present frame to screen and wait for FPS target       |
| ClearBackground()             | Frame       | Fill entire screen with one color                     |
| GetFrameTime()                | Time        | Seconds elapsed since last frame (~0.0166 at 60fps)   |
| GetTime()                     | Time        | Seconds since program started (used for blinking)     |
| GetScreenWidth()              | Screen      | Current window width in pixels                        |
| GetScreenHeight()             | Screen      | Current window height in pixels                       |
| LoadFontEx()                  | Font        | Load a .ttf font file from disk                       |
| GetFontDefault()              | Font        | Get raylib's built-in fallback font                   |
| UnloadFont()                  | Font        | Free font memory                                      |
| DrawTextEx()                  | Draw/Text   | Draw a string at a position with a font and color     |
| MeasureTextEx()               | Draw/Text   | Measure pixel width/height of a string                |
| DrawRectangle()               | Draw/Shape  | Draw a solid filled rectangle                         |
| DrawRectangleRounded()        | Draw/Shape  | Draw a solid filled rectangle with rounded corners    |
| DrawRectangleLinesEx()        | Draw/Shape  | Draw only the border of a rectangle                   |
| DrawRectangleRoundedLinesEx() | Draw/Shape  | Draw only the border of a rounded rectangle           |
| DrawLineEx()                  | Draw/Shape  | Draw a line between two points with thickness         |
| DrawCircle()                  | Draw/Shape  | Draw a solid filled circle                            |
| DrawTriangle()                | Draw/Shape  | Draw a solid filled triangle (3 points)               |
| BeginScissorMode()            | Clipping    | Restrict drawing to a rectangle region                |
| EndScissorMode()              | Clipping    | Remove the drawing restriction                        |
| GetMousePosition()            | Mouse       | Get current mouse cursor (x,y) as Vector2             |
| IsMouseButtonPressed()        | Mouse       | True on the exact frame left/right button is clicked  |
| GetMouseWheelMove()           | Mouse       | How much the scroll wheel moved this frame            |
| CheckCollisionPointRec()      | Collision   | True if a point is inside a rectangle                 |
| GetCharPressed()              | Keyboard    | Next typed character (call in loop until returns 0)   |
| IsKeyPressed()                | Keyboard    | True on the exact frame a key is pressed              |
