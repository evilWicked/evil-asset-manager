#pragma once

//we are using evil signals in this one so will need its implementation
#include "../signals/evil-wrapper.cpp"

//we are using the renderer in this one so will need its implementation
#include "../test-renderer/evil-wrapper.cpp"

//we are using utils in this one so will need its implementation
#include "../util/evil-wrapper.cpp"

#include "imp/asset.cpp"
//#include "imp/asset-class.cpp"
#include "imp/asset-loader.cpp"
#include "imp/asset-manager.cpp"
#include "imp/string-asset.cpp"

//DANGER DANGER WILL ROBINSON https://www.youtube.com/watch?v=RG0ochx16Dg 
//This file is responsible for bringing in all source used by the project.
//         changes to it will make life miserable




