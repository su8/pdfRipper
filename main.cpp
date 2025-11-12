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
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <future>
#include <stdexcept>

static void optimizePdf(const std::string &pdfPath);

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  if (argc < 2) { std::cerr << "You must provide folder with *pdf file(s) in it. Exiting." << std::endl; return EXIT_FAILURE; }
  std::vector<std::string> allPdfFiles;
  std::vector<std::future<void>> futures;
  for (const auto &entry : fs::directory_iterator(argv[1])) {
    if (entry.path().has_extension() && entry.path().extension() == ".pdf") { allPdfFiles.emplace_back(entry.path().string()); } }
  for (const auto &currentPdf : allPdfFiles) { futures.emplace_back(std::async(std::launch::async, optimizePdf, currentPdf)); }
  for (auto &future : futures) { future.get(); }
  return EXIT_SUCCESS;
}

#ifdef _WIN32
#define GS "C:\\gs\\bin\\gswin64c.exe"
#else
#define GS "gs"
#endif /* _WIN32 */

static void optimizePdf(const std::string &pdfPath) {
  fs::path outPath = pdfPath;
  if (outPath.stem().filename().string().starts_with("optimized_")) { std::cout << pdfPath << " already exists. Nothing to be done." << std::endl; return; }
  outPath.replace_filename("optimized_" + outPath.filename().string());
  if (fs::exists(outPath) && fs::is_regular_file(outPath)) { return; }
  char params[4096] = GS " -sDEVICE=pdfwrite -dNOPAUSE -dBATCH -dQUIET -dCompatibilityLevel=1.7 -dCompressFonts=true -dSubsetFonts=true -dPDFSETTINGS=/screen -sBandListStorage=memory -dBufferSpace=99000 -dNumRenderingThreads=8 -sOutputFile=";
  snprintf(params, sizeof(params), "%s%s %s", params, outPath.filename().string().c_str(), pdfPath.c_str());
  try {
    std::cout << "Please wait until we convert the requested " << pdfPath << " file." << std::endl;
    std::system(params);
    std::cout << "Done." << std::endl;
  } catch (const std::exception &e) { std::cerr << "Error: " << e.what() << std::endl; return; }
}