/**
* Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#ifdef _WIN32

#include <teetime/stages/Directory2Files.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

using namespace teetime;

void Directory2Files::collectFilesName(const std::string& directory, std::vector<std::string>& filenames, bool recursive)
{
  WIN32_FIND_DATA ffd;

  TCHAR szDir[MAX_PATH];
  StringCchCopy(szDir, MAX_PATH, directory.c_str());
  StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

  HANDLE hFind = FindFirstFileA(szDir, &ffd);

  if (INVALID_HANDLE_VALUE == hFind)
  {
    TEETIME_DEBUG() << "Directory not found: " << directory;
    return;
  }

  do
  {
    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      if (recursive && strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
      {
        collectFilesName(directory + "/" + std::string(ffd.cFileName), filenames, recursive);
      }
    }
    else
    {
      filenames.push_back(directory + "/" + std::string(ffd.cFileName));
    }
  } while (FindNextFileA(hFind, &ffd) != 0);

  FindClose(hFind);
}



#endif