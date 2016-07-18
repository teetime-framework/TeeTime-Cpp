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
#ifdef __linux__

#include <teetime/stages/Directory2Files.h>
#include <dirent.h>

using namespace teetime; 

void Directory2Files::collectFilesName(const std::string& directory, std::vector<std::string>& filenames, bool recursive)
{ 
  struct dirent* ep = nullptr;
  DIR* dp = opendir(directory.c_str());
  if ( dp )
  {
    while ( (ep = readdir(dp)) )
    {
      std::string name(ep->d_name);
      if ( name == "." || name == ".." )
        continue;

      if ( ep->d_type == DT_REG )
      {
        filenames.push_back(directory + "/" + std::move(name));
      }
      else if ( ep->d_type == DT_DIR && recursive)
      {
        collectFilesName(directory + "/" + name, filenames, true);
      }
    }

    (void) closedir(dp);
  } 
  else
  {
    TEETIME_DEBUG() << "directory not found: '" << directory << "'";
  }
}


#endif