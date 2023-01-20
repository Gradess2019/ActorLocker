# Actor Locker

## Description
Simple plugin that lets you lock actor in the Level Editor so you can't move or select it.

## How to install
1. Download [latest release](https://github.com/Gradess2019/ActorLocker/releases/latest) for your Unreal Engine version
2. Unzip into: **\<ProjectDirectory\>/Plugins** (create Plugins directory if it doesn't exist)
3. If you are using C++: Right Mouse Button on your **.uproject** file -> Generate Visual Studio project files
4. Launch project
5. If it's not enabled: Go to Edit -> Plugins -> "Project" category -> Editor -> Enable "Actor Locker" and restart the editor
7. Done

## How to use
You can manipulate selected actor using **hotkeys**, **World Outliner** or **Context Menu**

### Hotkeys (Default)
- **Alt + Comma** - Lock selection
- **Alt + Period** - Unlock selection
- **Alt + Shift + Comma** - Lock all actors
- **Alt + Shift + Period** - Unlock all actors
- **Alt + Slash** - Temporary toggle lock state of locked actors 

You can change hotkeys in Edit -> Editor Preferences -> Plugins -> Actor Locker

### World Outliner
You will see additional column with **Lock** icon. Just click on it and it will lock itself and all children as well.

![image](https://user-images.githubusercontent.com/38568823/212566043-07a552ef-09df-490f-beed-20489ae4adb3.png)

### Context Menu
Click Right Mouse Button on selected actors and you will see 2 additional actions in the bottom:
- Lock / Unlock selection
- Toggle lock

![image](https://user-images.githubusercontent.com/38568823/212566344-e70dda49-d6ae-4704-8dab-92d5a48b3253.png)
