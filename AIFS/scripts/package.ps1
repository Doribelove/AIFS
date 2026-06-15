$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$parent = Split-Path -Parent $root
$zip = Join-Path $parent "AIFS_github_project.zip"

if (Test-Path -LiteralPath $zip) {
    Remove-Item -LiteralPath $zip -Force
}

Compress-Archive -Path (Join-Path $root "*") -DestinationPath $zip -Force
Write-Output "Created $zip"

