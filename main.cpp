/*
Copyright 11/11/2025 https://github.com/su8/pdfRipper
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <future>
#include <stdexcept>

void optimizePdf(const std::string &pdfPath);

int main(int argc, char *argv[]) {
  if (argc < 2) { std::cerr << "You must provide folder with *pdf file(s) in it. Exiting." << std::endl; return EXIT_FAILURE; }
  std::vector<std::string> allPdfFiles;
  std::vector<std::future<void>> futures;
  for (const auto& entry : std::filesystem::directory_iterator(argv[1])) {
    if (entry.path().has_extension() && entry.path().extension() == ".pdf") { allPdfFiles.emplace_back(entry.path().string()); } }
  for (const auto &currentPdf : allPdfFiles) { futures.emplace_back(std::async(std::launch::async, optimizePdf, currentPdf)); }
  for (auto& future : futures) { future.get(); }
  return EXIT_SUCCESS;
}

void optimizePdf(const std::string &pdfPath) {
  std::filesystem::path outPath = pdfPath;
  outPath.replace_filename("optimized_" + outPath.filename().string());
  std::vector<std::string> cmd = {
#ifdef _WIN32
#define GS "C:\\gs\\bin\\gswin64c.exe"
#else
#define GS "gs"
#endif /* _WIN32 */
        GS,
        "-sDEVICE=pdfwrite",
        "-dNOPAUSE",
        "-dBATCH",
        "-dQUIET",
        "-dCompatibilityLevel=1.7",
        "-dCompressFonts=true",
        "-dPDFSETTINGS=/screen",
        "-sBandListStorage=memory",
        "-dBufferSpace=99000",
        "-dNumRenderingThreads=8",
        "-sOutputFile=" + outPath.string(),
        pdfPath
  };
  try {
    std::string cmdStr;
    for (const auto &str : cmd) { cmdStr += str + " "; }
    std::cout << "Please wait until we convert the requested *.pdf files." << std::endl;
    std::system(cmdStr.c_str());
    std::cout << "Done" << std::endl;
  } catch (const std::exception &e) { std::cerr << "Error: " << e.what() << std::endl; return; }
}