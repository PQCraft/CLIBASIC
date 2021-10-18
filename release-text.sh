if [ -z "$*" ]; then
    FILE=".changelog"
    if [ ! -f '.changelog' ]; then echo "Not a file: .changelog"; exit 1; fi
else
    for FILE in "$@"; do
        if [ ! -f "$FILE" ]; then echo "Not a file: $FILE"; exit 1; fi
    done
    FILE=""
fi
echo "**Changes:**
$(sed 's/^/- /' "$@" "$FILE")
<br>

**(Compressed) Executables:**
- Linux x86 64-bit: \`clibasic-linux-x64.zip\`
- Linux x86 32-bit: \`clibasic-linux-x86.zip\`
- Windows x86 64-bit using VT escape codes: \`clibasic-windows-vt-x64.zip\`
- Windows x86 32-bit using VT escape codes: \`clibasic-windows-vt-x86.zip\`
- Windows x86 64-bit using legacy API calls: \`clibasic-windows-x64.zip\`
- Windows x86 32-bit using legacy API calls: \`clibasic-windows-x86.zip\`
<br>

**Extras:**
- Example programs: \`examples.zip\`
- Documentation: \`docs.zip\`
- Windows Readline library: \`lib.zip\`"
