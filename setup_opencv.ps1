# PowerShell script to configure OpenCV 3.4.4 for Visual Studio project

$OPENCV_PATH = "E:\OpenCV\opencv-3.4.4"
$PROJECT_DIR = $PSScriptRoot

Write-Host "Configuring OpenCV 3.4.4 for ONNX_CPP project..." -ForegroundColor Green
Write-Host "OpenCV path: $OPENCV_PATH" -ForegroundColor Yellow
Write-Host "Project directory: $PROJECT_DIR" -ForegroundColor Yellow

# Check if OpenCV exists
if (-not (Test-Path $OPENCV_PATH)) {
    Write-Host "ERROR: OpenCV not found at $OPENCV_PATH" -ForegroundColor Red
    exit 1
}

# Check required modules
$REQUIRED_MODULES = @(
    "core",
    "imgproc", 
    "imgcodecs"
)

$MISSING_MODULES = @()
foreach ($module in $REQUIRED_MODULES) {
    $debug_lib = "$OPENCV_PATH\build64\lib\Debug\opencv_${module}344d.lib"
    $release_lib = "$OPENCV_PATH\build64\lib\Release\opencv_${module}344.lib"
    
    if (-not ((Test-Path $debug_lib) -or (Test-Path $release_lib))) {
        $MISSING_MODULES += $module
    }
}

if ($MISSING_MODULES.Count -gt 0) {
    Write-Host "WARNING: Missing OpenCV modules: $($MISSING_MODULES -join ', ')" -ForegroundColor Yellow
    Write-Host "You may need to rebuild OpenCV with these modules enabled." -ForegroundColor Yellow
} else {
    Write-Host "All required OpenCV modules found!" -ForegroundColor Green
}

# Check include directory
$INCLUDE_DIR = "$OPENCV_PATH\include"
if (-not (Test-Path "$INCLUDE_DIR\opencv2\opencv.hpp")) {
    Write-Host "ERROR: OpenCV headers not found at $INCLUDE_DIR" -ForegroundColor Red
    exit 1
}

# Check library directories
$DEBUG_LIB_DIR = "$OPENCV_PATH\build64\lib\Debug"
$RELEASE_LIB_DIR = "$OPENCV_PATH\build64\lib\Release"

if (-not (Test-Path $DEBUG_LIB_DIR)) {
    Write-Host "WARNING: Debug libraries directory not found: $DEBUG_LIB_DIR" -ForegroundColor Yellow
}

if (-not (Test-Path $RELEASE_LIB_DIR)) {
    Write-Host "WARNING: Release libraries directory not found: $RELEASE_LIB_DIR" -ForegroundColor Yellow
}

# Check DLL directories
$DEBUG_BIN_DIR = "$OPENCV_PATH\build64\bin\Debug"
$RELEASE_BIN_DIR = "$OPENCV_PATH\build64\bin\Release"

Write-Host "`nOpenCV Configuration Summary:" -ForegroundColor Cyan
Write-Host "  Include Directory: $INCLUDE_DIR" -ForegroundColor White
Write-Host "  Debug Libraries: $DEBUG_LIB_DIR" -ForegroundColor White
Write-Host "  Release Libraries: $RELEASE_LIB_DIR" -ForegroundColor White
Write-Host "  Debug DLLs: $DEBUG_BIN_DIR" -ForegroundColor White
Write-Host "  Release DLLs: $RELEASE_BIN_DIR" -ForegroundColor White

Write-Host "`nRequired OpenCV modules for CLIP:" -ForegroundColor Cyan
Write-Host "  - opencv_core (core functionality)" -ForegroundColor White
Write-Host "  - opencv_imgproc (image processing: resize, normalize)" -ForegroundColor White
Write-Host "  - opencv_imgcodecs (image loading: imread)" -ForegroundColor White

Write-Host "`nNext steps:" -ForegroundColor Yellow
Write-Host "1. Open Visual Studio project properties" -ForegroundColor White
Write-Host "2. Configuration Properties -> VC++ Directories" -ForegroundColor White
Write-Host "3. Include Directories: add $INCLUDE_DIR" -ForegroundColor White
Write-Host "4. Library Directories (Debug): add $DEBUG_LIB_DIR" -ForegroundColor White
Write-Host "5. Library Directories (Release): add $RELEASE_LIB_DIR" -ForegroundColor White
Write-Host "6. Configuration Properties -> Linker -> Input" -ForegroundColor White
Write-Host "7. Additional Dependencies (Debug): add opencv_core344d.lib;opencv_imgproc344d.lib;opencv_imgcodecs344d.lib" -ForegroundColor White
Write-Host "8. Additional Dependencies (Release): add opencv_core344.lib;opencv_imgproc344.lib;opencv_imgcodecs344.lib" -ForegroundColor White
Write-Host "9. Copy OpenCV DLLs to output directory (x64/Debug or x64/Release)" -ForegroundColor White
Write-Host "   - opencv_core344d.dll (Debug) or opencv_core344.dll (Release)" -ForegroundColor White
Write-Host "   - opencv_imgproc344d.dll (Debug) or opencv_imgproc344.dll (Release)" -ForegroundColor White
Write-Host "   - opencv_imgcodecs344d.dll (Debug) or opencv_imgcodecs344.dll (Release)" -ForegroundColor White

Write-Host "`nConfiguration complete!" -ForegroundColor Green

