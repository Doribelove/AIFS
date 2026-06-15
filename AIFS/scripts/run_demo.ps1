$ErrorActionPreference = "Stop"

cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

$demo = Get-ChildItem -LiteralPath build -Recurse -Filter "aifs_demo.exe" | Select-Object -First 1
if ($null -eq $demo) {
    throw "aifs_demo.exe was not found under build/"
}
& $demo.FullName
