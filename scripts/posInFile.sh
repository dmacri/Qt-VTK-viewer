#!/usr/bin/env bash
# posInFile.sh - show the line containing a byte offset and mark the exact byte with '^'
# always append an EOL marker '␤' to the displayed line
# optional --show-whitespace: show spaces as '␠' and tabs as '→'
# terminal width is detected automatically with tput cols

set -euo pipefail

file=${1:-}
offset=${2:-}
show_whitespace=false

# optional third argument may be --show-whitespace
if [[ "${3:-}" == "--show-whitespace" || "${2:-}" == "--show-whitespace" && -n "${3:-}" ]]; then
    # case when user passed only two args and second is flag
    if [[ -z "${offset}" || "${offset}" == "--show-whitespace" ]]; then
        # shift arguments (very simple handling)
        show_whitespace=true
        offset=${3:-}
    else
        show_whitespace=true
    fi
fi

# usage check
if [[ -z "$file" || -z "$offset" ]]; then
    echo "Usage: $0 <file> <byte_offset> [--show-whitespace]"
    exit 1
fi

# validate offset numeric
if ! [[ "$offset" =~ ^[0-9]+$ ]]; then
    echo "Error: byte offset must be a non-negative integer!"
    exit 1
fi

if [[ ! -f "$file" ]]; then
    echo "Error: file '$file' does not exist!"
    exit 1
fi

echo "File '$file':$offset"

# Read the prefix up to offset (first 'offset' bytes).
# We use dd to be precise with bytes.
prefix=$(dd if="$file" bs=1 count="$offset" 2>/dev/null || true)

# Count how many newline bytes are in the prefix -> number of full lines before the target byte
newline_count=$(printf '%s' "$prefix" | tr -cd '\n' | wc -c)
# target line number is newline_count + 1 (sed is 1-based)
line_num=$(( newline_count + 1 ))

# Determine the byte-length of the final (partial) line in the prefix: that's column_in_line
# We take the last record of prefix split by newline and measure bytes
# Using awk to get last record reliably
last_line_in_prefix=$(printf '%s' "$prefix" | awk 'BEGIN{RS="\n"} {last=$0} END{printf "%s", last}')
column_bytes=$(printf '%s' "$last_line_in_prefix" | wc -c)
echo "Line number: $line_num"
echo "Column in line (byte offset from line start): $column_bytes"

# Extract the full line content from file (sed returns line without newline)
line_content=$(sed -n "${line_num}p" "$file" || true)

# Always append visible EOL marker
eol_marker="␤"
orig_visible_line="${line_content}${eol_marker}"

# Determine whether the offset byte itself is a newline (i.e., the byte at skip=offset)
# We read the single byte at position 'offset' (dd skip counts bytes from 0)
byte_at_offset=$(dd if="$file" bs=1 count=1 skip="$offset" 2>/dev/null | od -An -t u1 | tr -s ' ' | sed -e 's/^ *//' -e 's/ *$//')

# If dd returned nothing (offset beyond EOF), byte_at_offset will be empty
# For our pointer computation we consider newline (10) specially; otherwise pointer is at column_bytes
# pointer_index_in_orig = column_bytes (this is correct in both cases: points to EOL marker if offset==newline)
pointer_index="$column_bytes"

# Prepare line for display and compute pointer position after applying whitespace transformation (if any).
# We must apply same transformations to the prefix (up to pointer_index) to compute accurate pointer position.

# Get prefix of the visible original line up to pointer_index (in bytes)
# Use printf and head -c to slice by bytes
prefix_up_to_pointer=$(printf '%s' "$orig_visible_line" | head -c "$pointer_index")

# Apply whitespace visualization if requested
if [[ "$show_whitespace" == true ]]; then
    # Replace space -> ␠ ; tab -> → ; keep eol marker (it will be unaffected)
    vis_full=$(printf '%s' "$orig_visible_line" | sed -e 's/ /␠/g' -e $'s/\t/→/g')
    vis_prefix=$(printf '%s' "$prefix_up_to_pointer" | sed -e 's/ /␠/g' -e $'s/\t/→/g')
else
    vis_full="$orig_visible_line"
    vis_prefix="$prefix_up_to_pointer"
fi

# pointer_pos is the display length (in bytes/columns for ASCII) of the transformed prefix
# Note: wc -c counts bytes; for ASCII this equals visual columns. For multibyte UTF-8 there may be mismatch.
pointer_pos=$(printf '%s' "$vis_prefix" | wc -c)

# Now we have vis_full (the full visible line after replacements) and pointer_pos.
# Next: trim vis_full if needed to fit terminal width and place pointer centered.

# determine terminal width automatically
max_width=$(tput cols 2>/dev/null || true)
if [[ -z "$max_width" || "$max_width" -lt 20 ]]; then
    max_width=80
fi

# current displayed line length (bytes)
line_len=$(printf '%s' "$vis_full" | wc -c)

if (( line_len > max_width )); then
    # We want to center the pointer when possible.
    # Compute start byte offset for the slice: center pointer roughly in middle
    half=$(( max_width / 2 ))
    # start pointer index in bytes (try to place pointer at half)
    start_byte=$(( pointer_pos - half ))
    (( start_byte < 0 )) && start_byte=0
    # Ensure we don't go past end
    if (( start_byte + max_width > line_len )); then
        start_byte=$(( line_len - max_width ))
        (( start_byte < 0 )) && start_byte=0
    fi
    # Extract substring (by bytes)
    display_line=$(printf '%s' "$vis_full" | dd bs=1 skip="$start_byte" count="$max_width" 2>/dev/null)
    # Adjust pointer position relative to display start
    pointer_pos=$(( pointer_pos - start_byte ))
else
    display_line="$vis_full"
    # pointer_pos already correct
fi

# Finally print results
echo "The line:"
printf '%s\n' "$display_line"
# print pointer (use printf with width)
# If pointer_pos equals 0, print '^' at column 0 (no leading spaces)
if (( pointer_pos > 0 )); then
    # print pointer_pos spaces then ^
    # printf "%*s^\n" will pad pointer_pos with spaces, but counts width characters; using pointer_pos is OK for ASCII
    printf "%*s^\n" "$pointer_pos" ""
else
    # pointer at very start
    printf "^\n"
fi

# Optionally, show the actual byte value under the caret for debugging (uncomment if needed)
# byte_val_hex=$(dd if="$file" bs=1 count=1 skip="$offset" 2>/dev/null | od -An -t x1 | tr -s ' ' | sed -e 's/^ *//' -e 's/ *$//')
# echo "Byte at offset (hex): ${byte_val_hex:-(none)}"
