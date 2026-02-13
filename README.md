# Cinematic Camera Editor — Stewart

An **interpolate camera editor** for [SA-MP-Open.MP](https://github.com/openmultiplayer/open.mp) that allows you to easily create cinematic sequences using camera points.  
Smoothly move the camera through multiple points to create professional cinematic effects for intros, property showcases, or trailers.

---

## Features

- Free NoClip mode for precise camera positioning  
- Add multiple camera points (keyframes) per scene  
- Play scenes with smooth interpolation  
- Export scene coordinates to the server console  
- Supports multiple scenes per server  

---

## Git Clone

Clone the repository to your includes folder:

```bash
git clone https://github.com/stewartab/Interpolate-Camera-Editor.git
```

Include it in your gamemode:

```c++
#include <camera_editor>
```

---

## Commands & Usage
1. `/noclip`
- Toggle NoClip mode for free camera movement
- Keys:
    - Move forward/backward: **W / S**
    - Strafe left/right: **A / D**
    - Jump / Crouch: **Space / Ctrl**

2. `/ncspeed <value>`
- Adjust NoClip speed
- Example: /ncspeed 2.5

3. `/addcampoint <sceneid> <time>`
- Add current camera position as a point in a scene
- Parameters:
    - sceneid: Scene ID **(0..MAX_CAM_SCENES-1)**
    - time: Milliseconds to interpolate to the next point
- Minimum: 2 points per scene for playback

4. `/playcam <sceneid>`
- Plays the scene using all added points
- Automatically enables spectator mode
- Displays the number of points in the scene

5. `/exportscene <sceneid>`
- Prints all camera points to the server console
- Each point includes:
    - Camera position (x, y, z)
    - Look-at position (x, y, z)
    - Duration to next point

---

## Credits
Cinematic Camera Editor coded by [Stewart](https://github.com/stewartab)

---

## License
> This project is released under the MIT License — free to use, modify, and distribute.