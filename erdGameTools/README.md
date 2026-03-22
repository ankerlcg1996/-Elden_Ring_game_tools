# erdGameTools

Standalone Elden Ring native menu mod that injects `erdGameTools` into the Site of Grace menu.

Current behavior:

- Adds a native first-level menu entry `erdGameTools`.
- Adds second-level menus `游戏修改` and `快捷入口`.
- `游戏修改` drives the built-in gameplay toggles from this project.
- `快捷入口` reads external entries from `Resources/NpcMenus.zh.txt`.

Build:

```powershell
cmake -S erdGameTools -B erdGameTools/build -G "Visual Studio 17 2022" -A x64
cmake --build erdGameTools/build --config Release
```
