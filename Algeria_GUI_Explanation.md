# Algeria History DB — Full GUI Explanation

---

## 1. What is this program?

This is a **desktop GUI application** written in C using the **Raylib** graphics library.
It manages a database of Algerian historical **personalities** (people) and **events**,
stored in a plain text file called `Algeria_history.txt`.

The window is split into 3 zones:

```
┌──────────────┬────────────────────────────────┬───────────────┐
│              │  Top bar (title + search + add) │               │
│   SIDEBAR    ├────────────────────────────────┤  DETAIL PANEL │
│  (190 px)    │                                │   (260 px)    │
│              │      MAIN TABLE / VIEW         │               │
│              │                                │               │
│              ├────────────────────────────────┤               │
│              │  Status bar                    │               │
└──────────────┴────────────────────────────────┴───────────────┘
```

---

## 2. Data Structure — TList

Every record (personality or event) is stored as a node of this struct:

```c
typedef struct TList {
    char name[40];          // Name of personality or event
    char difinition[400];   // Description (personalities only)
    struct {
        char birth[10];     // Birth year  e.g. "1808"
        char death[10];     // Death year  e.g. "1883"
    } date;                 // Used for personalities
    char dateB[20];         // Date string for events  e.g. "01/11/1954"
    char dateE[10];         // (reserved, not used currently)
    int  type;              // 0 = personality,  1 = event
    struct TList *next;     // Pointer to next node  →
    struct TList *prv;      // Pointer to previous node  ←
} TList;
```

This forms a **bidirectional (doubly) linked list**:
- Each node knows its **next** neighbour and its **previous** neighbour.
- The list has no fixed array size — it grows dynamically with `malloc`/`calloc`.

---

## 3. Global Lists in Memory

The program keeps 4 linked lists live in RAM at all times:

| Variable        | What it holds                                      |
|-----------------|----------------------------------------------------|
| `gPersonNames`  | Personalities with name + definition only          |
| `gPersonDates`  | Personalities with name + birth/death years only   |
| `gMerged`       | Merged result: name + definition + dates (full)    |
| `gEvents`       | All historical events with name + date string      |

`gMerged` is what you see in the Personalities tab.
`gEvents` is what you see in the Events tab.

---

## 4. File Format — Algeria_history.txt

All data lives in one file. Two line formats coexist:

**Personality line:**
```
Name=definition{YYYY-YYYY}
```
Example:
```
Emir Abdelkader=Algerian military and political leader...{1808-1883}
```

**Event line:**
```
Name:{date}
```
Example:
```
Battle of Algiers:{1956-1957}
Start of Revolution:{01/11/1954}
```

The parser tells them apart: event lines contain `:{`, personality lines contain `=`.

---

## 5. All Functions — Explained One by One

### 5.1 Data Functions

---

#### `addEvents(TList **b, char *namEvente, char *date)`
**Your original function.**

- Creates a new node with `malloc`.
- Copies the event name into `newnode->name` and the date into `newnode->dateB`.
- Sets `type = 1` (event).
- If the list is empty (`*b == NULL`), the new node becomes the head.
- Otherwise, walks to the end of the list and attaches it there.
- Opens `Algeria_history.txt` in **append mode** (`"a"`) and writes the line:
  ```
  EventName:{date}
  ```
- Called by the GUI when you click **Save** in the Add Event modal.

---

#### `addPersonality(TList **s, TList **a, name, def, DoB, DoD)`

- Creates **two** separate nodes for the same person:
  - One node in `s` (the names list) storing name + definition.
  - One node in `a` (the dates list) storing name + birth year + death year.
- Both lists are extended by walking to their end and attaching the new node.
- Called during seed (startup) and when user clicks Save in Add Personality modal.

---

#### `mergePersonalities(TList *names, TList *dates)`

- Combines the names list and the dates list into one complete list (`gMerged`).
- For each node in `names`, searches `dates` for a node with the same name.
- When found, creates a new merged node with name + definition + birth + death.
- Sets `prv` pointers so the result is a proper **doubly linked list**.
- Returns the head of the merged list.

---

#### `deleteFromList(TList **list, const char *name)`

- Walks the list looking for a node whose name matches.
- When found:
  - If it is the first node: makes the second node the new head.
  - Otherwise: makes the previous node skip over it (`prev->next = ptr->next`).
  - Updates the `prv` pointer of the node after the deleted one.
  - Calls `free()` to release the memory.

---

#### `updatePersonality(TList *list, name, def, b, d)`

- Walks the list until it finds the node with the given name.
- Overwrites its `difinition`, `date.birth`, and `date.death` fields.
- Used when you edit a personality and click Save.

---

#### `sortByName(TList *head)`

- **Bubble sort** on the linked list by name (alphabetical A→Z).
- Two nested loops compare every pair of adjacent nodes.
- When `i->name > j->name`, swaps **all fields** between the two nodes
  (name, definition, birth, death, dateB, type) — it swaps the data,
  not the pointers, which is simpler for linked lists.

---

#### `sortByAge(TList *head)`

- **Bubble sort** by lifespan (shortest life first).
- Calculates age as `death year − birth year` for both nodes.
- If node `i` lived longer than node `j`, swaps their data.
- Only meaningful for personalities (events have no birth/death).

---

#### `isPalindrome(const char *w)`

- Checks if a single word reads the same forwards and backwards.
- Uses two pointers: `i` starts at the left, `j` starts at the right.
- Compares characters (case-insensitive with `tolower`) and moves inward.
- Returns `1` if palindrome, `0` if not.
- Example: `"aba"` → palindrome. `"Ahmed"` → not.

---

#### `isKilled(const char *def)`

- Checks if a personality's definition contains any of these keywords:
  `"killed"`, `"assassinated"`, `"murdered"`, `"executed"`, `"shot dead"`.
- Converts the definition to lowercase first for case-insensitive matching.
- Returns `true` if any keyword is found → displayed as **Martyr** tag in the table.

---

#### `freeList(TList *h)`

- Walks the entire list and calls `free()` on every node.
- Called on program exit to release all memory cleanly.

---

#### `listCount(TList *h)`

- Walks the list and counts nodes.
- Returns the total number of records.
- Used to update the status bar count.

---

#### `listGet(TList *h, int idx)`

- Returns the node at position `idx` (0-based).
- Used to get the currently selected record for the detail panel.

---

### 5.2 File I/O Functions

---

#### `loadPersonalities(FILE *f)`

- Reads the file line by line.
- Skips any line containing `:{` (those are events).
- Skips any line without `=`.
- Parses each personality line using `sscanf`:
  - Everything before `=` → name.
  - Everything between `=` and `{` → definition.
  - Inside `{...}` → birth year and death year.
- Creates nodes in both `gPersonNames` and `gPersonDates`.

---

#### `loadEvents(FILE *f)`

- Reads the file line by line.
- Only processes lines containing `:{`.
- Parses: everything before `:` → name, everything inside `{...}` → date.
- Creates nodes in `gEvents`.

---

#### `loadFromFile(void)`

- Opens `Algeria_history.txt`.
- Calls `loadPersonalities()` then rewinds the file and calls `loadEvents()`.
- After loading, calls `mergePersonalities()` to build `gMerged`.
- Frees old lists first to avoid memory leaks on reload.

---

#### `savePersonalities(void)`

- Preserves event lines: reads them from the file into a buffer first.
- Rewrites the whole file: all merged personality nodes first, then the event lines.
- Format: `Name=definition{birth-death}` for each personality.
- Called after Add, Edit, or Delete of a personality.

---

#### `seedData(void)`

- Called only when `Algeria_history.txt` does not exist yet (first run).
- Adds 7 default personalities using `addPersonality()`.
- Adds 5 default events using your `addEvents()` function.
- `addEvents()` creates the file automatically since it opens it in append mode.

---

### 5.3 UI Helper Functions

---

#### `activeList()`

- Returns `gEvents` if the Events tab is active, otherwise `gMerged`.
- Every table/detail/delete/edit operation calls this to know which list to use.

---

#### `strCI(haystack, needle)`

- **Case-insensitive string search.**
- Copies both strings, lowercases them, then uses `strstr()`.
- Used for the search box — lets you type "ahmed" and find "Ahmed Ben Bella".

---

#### `showToast(msg)`

- Sets `gToast` to the message and `gToastTimer` to 3 seconds.
- The main loop draws a small dark pill at the bottom of the screen while the timer is above 0.
- Used to confirm actions: "Event added.", "Record deleted.", etc.

---

#### `drect(x, y, w, h, color)`

- Thin wrapper around Raylib's `DrawRectangle`.
- Used everywhere to fill rectangular areas with a solid color.

---

#### `dtag(font, text, x, y, bg, fg)`

- Draws a small colored pill/badge (like "Military", "Martyr", "Event").
- Measures the text width, draws a rounded rectangle behind it, then draws the text.

---

#### `dbtn(font, text, x, y, w, h, bg, fg, border, hovered)`

- Draws a clickable button with rounded corners and a border.
- When `hovered` is true, darkens the background color slightly.
- Draws the text centered inside the button.
- Does **not** handle clicks itself — the caller checks `IsMouseButtonPressed`.

---

#### `dwrapped(font, text, x, y, maxW, fontSize, color)`

- Draws multi-line text that wraps at word boundaries within `maxW` pixels.
- Splits text into words with `strtok`, builds lines word by word.
- When adding the next word would exceed `maxW`, draws the current line and starts a new one.
- Used for descriptions in the detail panel and modal dialogs.

---

#### `dinput(font, label, value, x, y, w, focused, multi)`

- Draws a labeled input field box.
- Draws the label text above, then a rounded rectangle.
- Draws the current value inside.
- When `focused` is true: draws a blinking cursor at the end of the text (blinks at 2 Hz).
- When `multi` is true: uses `dwrapped` for multiline display (description field).

---

#### `rowTag(font, node, x, y)`

- Decides which color tag to draw for a table row:
  - Event → purple "Event" tag.
  - Personality whose definition contains killed/executed/etc → amber "Martyr" tag.
  - Other personality → green "Military" tag.

---

### 5.4 Main Draw Functions

---

#### `drawModal(Font font)`

- Called every frame when `gModal != MODAL_NONE`.
- Draws a dark semi-transparent overlay over the whole window.
- Then draws a white rounded rectangle (the dialog box) in the center.
- Handles 8 modal types:

| Modal            | Purpose                                      |
|------------------|----------------------------------------------|
| `MODAL_ADD_P`    | Form to add a new personality                |
| `MODAL_EDIT_P`   | Form to edit selected personality            |
| `MODAL_ADD_E`    | Form to add a new event (uses `addEvents`)   |
| `MODAL_EDIT_E`   | Form to edit selected event                  |
| `MODAL_CONFIRM_DEL` | Confirmation before deleting a record     |
| `MODAL_SEARCH_DATE` | Search all records by year               |
| `MODAL_COUNT`    | Count how many records contain a keyword     |
| `MODAL_PALINDROMES` | Find palindromic words in definitions    |

- At the bottom of `drawModal`, the **text input routing** block runs:
  - Reads characters with `GetCharPressed()` and appends to the correct field buffer.
  - `Backspace` removes the last character.
  - `Tab` moves focus to the next field.
  - Mouse clicks on field rectangles change focus.
  - For event forms: `fFocus 0` → writes to `fName`, `fFocus 1` → writes to `fBirth`.

---

#### `drawSidebar(Font font)`

- Draws the left panel (190px wide) with a dark background.
- Header: "Algeria History DB" title + "NSCS · 2025/2026" subtitle.
- Renders 4 sections with their items:

| Section       | Items                                               |
|---------------|-----------------------------------------------------|
| Linked Lists  | Personalities, Events                               |
| Structures    | Stack view, Queue view                              |
| Operations    | Sort by name, Sort by age, Palindromes, Merge lists |
| Recursion     | Search by date, Count occurrences                   |

- Highlights the active item with a green left border and white background.
- On mouse click, sets `gSBActive`, changes `gTab` and `gView`, or opens a modal.

---

#### `drawTable(Font font, tx, ty, tw, th)`

- Draws the main data table inside the content area.
- First filters the active list by the search box text using `strCI()`.
- Draws a sticky header row with column names.
- For **Personalities**: columns are #, Name, Birth, Death, Type.
- For **Events**: columns are #, Name, Date, Type (no Death column).
- Draws each visible row; highlights selected row in green, hovered row in beige.
- Handles mouse scroll (`GetMouseWheelMove`) to scroll through records.
- Clicking a row sets `gSelected` to that row's index.

---

#### `drawStructView(Font font, x, y, w, h)`

- Draws the Stack or Queue structural visualization.
- Only uses `gMerged` (personalities).
- Draws each personality as a rounded box with name and birth-death-age info.
- Between boxes draws a vertical arrow (`DrawLineEx` + `DrawTriangle`).
- **Stack view**: renders from last node to first (top = most recently added).
  - Shows "▲ TOP" at the top, "▼ BOTTOM" at the bottom.
- **Queue view**: renders from first to last (head = first inserted).
  - Shows "HEAD →" on the left, "→ TAIL" on the right.

---

#### `drawDetail(Font font, x, w)`

- Draws the right panel (260px wide).
- Gets the currently selected record with `listGet(activeList(), gSelected)`.
- **Header**: shows name and date range / age.
- **Body** (for personalities):
  - DESCRIPTION — wrapped definition text.
  - BIRTH / DEATH years.
  - DATA STRUCTURE — shows `next ->` and `prev ->` pointer targets by name (or NULL).
  - FLAGS — Palindrome (yes/no) and Killed (yes/no).
- **Body** (for events):
  - DATE — the event date string.
  - DATA STRUCTURE — next/prev pointers.
- **Footer**: Edit button (opens edit modal) and Delete button (opens confirm modal).

---

#### `main(void)`

- Initializes the Raylib window (1100×640, resizable, 4× antialiasing).
- Tries to open `Algeria_history.txt`:
  - If it exists → calls `loadFromFile()`.
  - If it does not → calls `seedData()` which creates the file with defaults.
- Loads a font from `resources/JetBrainsMono-Regular.ttf` if present,
  otherwise falls back to Raylib's built-in font.
- **Main loop** runs at 60 FPS:
  1. Decrements the toast timer.
  2. Handles `Escape` key (closes any open modal).
  3. Handles search box keyboard input.
  4. Calls `drawSidebar`, then draws the top bar, table/structure view,
     status bar, detail panel, modal (if open), and toast (if active).
- On exit: frees all 4 linked lists and the font, then closes the window.

---

## 6. Enums (State Flags)

```c
typedef enum { VIEW_LIST, VIEW_STACK, VIEW_QUEUE } ViewMode;
```
Controls what the main content area shows.

```c
typedef enum { TAB_PERSONALITIES=0, TAB_EVENTS=1 } TabMode;
```
Controls which linked list (`gMerged` or `gEvents`) is the active one.

```c
typedef enum {
    MODAL_NONE, MODAL_ADD_P, MODAL_EDIT_P,
    MODAL_ADD_E, MODAL_EDIT_E,
    MODAL_CONFIRM_DEL,
    MODAL_SEARCH_DATE, MODAL_COUNT, MODAL_PALINDROMES
} ModalMode;
```
Controls which dialog box (if any) is currently open.

---

## 7. Global State Variables

| Variable       | Type      | Purpose                                           |
|----------------|-----------|---------------------------------------------------|
| `gView`        | ViewMode  | LIST / STACK / QUEUE                              |
| `gTab`         | TabMode   | Which tab is active (personalities or events)     |
| `gModal`       | ModalMode | Which modal is open (NONE = no modal)             |
| `gSelected`    | int       | Index of the selected row in the current list     |
| `gScrollY`     | int       | How many rows scrolled down in the table          |
| `gSearch`      | char[80]  | Current text in the search box                    |
| `gSearchFocus` | bool      | Whether the search box is receiving keyboard input|
| `gSBActive`    | int       | Index of the active sidebar item                  |
| `fName`        | char[40]  | Form field: name being typed                      |
| `fDef`         | char[400] | Form field: description being typed               |
| `fBirth`       | char[20]  | Form field: birth year or event date being typed  |
| `fDeath`       | char[10]  | Form field: death year being typed                |
| `fFocus`       | int       | Which form field has keyboard focus (0,1,2,3)     |
| `fEditIdx`     | int       | Index of the record being edited                  |
| `gToast`       | char[300] | Message shown in the toast notification           |
| `gToastTimer`  | float     | Seconds remaining before toast disappears         |
| `gOpResult`    | char[600] | Result text shown inside Search/Count modals      |

---

## 8. How a User Action Flows (Example: Add Event)

```
User clicks "+ Add" button  (Events tab is active)
        ↓
GUI sets gModal = MODAL_ADD_E
clears fName, fBirth
        ↓
Next frames: drawModal() draws the Add Event dialog
        ↓
User types in Name field  (fFocus = 0)
  → GetCharPressed() appends to fName[]
        ↓
User presses Tab  (fFocus becomes 1)
  → GetCharPressed() now appends to fBirth[]
        ↓
User types the date
        ↓
User clicks "Save"
  → hSave is true, fName[0] is not empty
  → calls addEvents(&gEvents, fName, fBirth)
        ↓
addEvents():
  allocates new node
  copies fName → node->name
  copies fBirth → node->dateB
  appends node to gEvents list
  opens Algeria_history.txt in append mode
  writes "EventName:{date}\n"
        ↓
GUI sets gModal = MODAL_NONE
calls showToast("Event added.")
        ↓
Table redraws, new event appears in list
Toast fades after 3 seconds
```

---

## 9. Build & Run

```bash
# Compile
gcc algeria_history_gui.c -o algeria_history_gui \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Run
./algeria_history_gui
```

The program looks for `Algeria_history.txt` in the **same folder** you run it from.
If the file does not exist, it creates it automatically with the default seed data.

Optional: put `JetBrainsMono-Regular.ttf` inside a `resources/` subfolder next to
the executable for a better font. Without it, Raylib's built-in bitmap font is used.
