# PowerShell script to automatically download ONNX Runtime

$ONNX_VERSION = "1.20.0"
$ONNX_URL = "https://github.com/microsoft/onnxruntime/releases/download/v$ONNX_VERSION/onnxruntime-win-x64-$ONNX_VERSION.zip"
$DOWNLOAD_DIR = "$PSScriptRoot\onnxruntime_temp"
$EXTRACT_DIR = "$PSScriptRoot\onnxruntime"

Write-Host "Downloading ONNX Runtime v$ONNX_VERSION..." -ForegroundColor Green

# Create temporary directory
if (Test-Path $DOWNLOAD_DIR) {
    Remove-Item -Recurse -Force $DOWNLOAD_DIR
}
New-Item -ItemType Directory -Path $DOWNLOAD_DIR | Out-Null

# Download archive
$ZIP_FILE = "$DOWNLOAD_DIR\onnxruntime.zip"
Write-Host "Downloading from: $ONNX_URL" -ForegroundColor Yellow
try {
    Invoke-WebRequest -Uri $ONNX_URL -OutFile $ZIP_FILE -UseBasicParsing
    Write-Host "Download completed!" -ForegroundColor Green
} catch {
    Write-Host "Error downloading: $_" -ForegroundColor Red
    exit 1
}

# Extract archive
Write-Host "Extracting archive..." -ForegroundColor Yellow
if (Test-Path $EXTRACT_DIR) {
    Remove-Item -Recurse -Force $EXTRACT_DIR
}
Expand-Archive -Path $ZIP_FILE -DestinationPath $EXTRACT_DIR -Force

# Move files to correct structure
$TEMP_INCLUDE = Get-ChildItem -Path $EXTRACT_DIR -Recurse -Filter "onnxruntime_cxx_api.h" | Select-Object -First 1
if ($TEMP_INCLUDE) {
    $TARGET_INCLUDE = "$EXTRACT_DIR\include\onnxruntime\core\session"
    New-Item -ItemType Directory -Path $TARGET_INCLUDE -Force | Out-Null
    Copy-Item -Path $TEMP_INCLUDE.FullName -Destination "$TARGET_INCLUDE\onnxruntime_cxx_api.h" -Force
}

$TEMP_LIB = Get-ChildItem -Path $EXTRACT_DIR -Recurse -Filter "onnxruntime.lib" | Select-Object -First 1
if ($TEMP_LIB) {
    $TARGET_LIB = "$EXTRACT_DIR\lib"
    New-Item -ItemType Directory -Path $TARGET_LIB -Force | Out-Null
    Copy-Item -Path $TEMP_LIB.FullName -Destination "$TARGET_LIB\onnxruntime.lib" -Force
}

$TEMP_DLL = Get-ChildItem -Path $EXTRACT_DIR -Recurse -Filter "onnxruntime.dll" | Select-Object -First 1
if ($TEMP_DLL) {
    Copy-Item -Path $TEMP_DLL.FullName -Destination "$EXTRACT_DIR\lib\onnxruntime.dll" -Force
}

# Remove temporary directory
Remove-Item -Recurse -Force $DOWNLOAD_DIR

Write-Host "`nONNX Runtime successfully installed to: $EXTRACT_DIR" -ForegroundColor Green
Write-Host "`nNext steps:" -ForegroundColor Yellow
Write-Host "1. Open project properties in Visual Studio" -ForegroundColor White
Write-Host "2. Configuration Properties -> VC++ Directories" -ForegroundColor White
Write-Host "3. Include Directories: add `$`(ProjectDir)onnxruntime\include" -ForegroundColor White
Write-Host "4. Library Directories: add `$`(ProjectDir)onnxruntime\lib" -ForegroundColor White
Write-Host "5. Configuration Properties -> Linker -> Input" -ForegroundColor White
Write-Host "6. Additional Dependencies: add onnxruntime.lib" -ForegroundColor White
Write-Host "7. Copy onnxruntime.dll to .exe folder (x64/Debug or x64/Release)" -ForegroundColor White

