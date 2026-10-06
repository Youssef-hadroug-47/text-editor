# Text Editor

A simple text editor for opening and editing files.

## Features

- Open existing files
- Edit text content
- Save changes to files

## Keyboard Shortcuts

- **Ctrl+S** - Save the current file
- **Ctrl+Q** - Quit the editor
  - If the file has been modified, press **Ctrl+Q** again to confirm quit without saving
- **Alt+$** - Go to end of line
- **Alt+0** - Go to beginning of the line

## Known Limitations

### Character Encoding

**UTF-8 is not supported.** Files containing UTF-8 encoded characters may display incorrectly or appear broken. This includes:

- Accented characters (é, ñ, ü, etc.)
- Non-Latin scripts (Chinese, Arabic, Cyrillic, etc.)
- Special symbols and emoji

For best results, use files with ASCII-only characters.

## Usage

1. Launch the text editor
2. Open a file to begin editing
3. Make your changes
4. Press **Ctrl+S** to save
5. Press **Ctrl+Q** to quit (press again if file has unsaved changes)

## Notes

- Unsaved changes will trigger a confirmation prompt when attempting to quit
- The editor works best with plain text files using standard ASCII encoding
