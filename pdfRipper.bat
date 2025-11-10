:TOP
@CLS
@ECHO OFF
TITLE pdfRipper
:BEGIN
if not exist C:\MingW\bin\pdfRipper.exe (
COPY pdfRipper.exe C:\MingW\bin\pdfRipper.exe
)
CLS
:loop
echo Provide the folder that has *.pdf files in it
SET /P NAME=
C:\MingW\bin\pdfRipper.exe %name%
PAUSE
GOTO loop