/*

    Cinematic Camera Editor v1 — Stewart
    ---------------------------------

    This is an interpolate camera editor for SA-MP. 
    It allows you to create cinematic sequences using camera points, 
    which are automatically interpolated for smooth camera motion.

    This system is currently developed to allow PC and Mobile users to
    use this system. Rest assured that development will progress.

    Features:
    - NoClip editor for precise camera placement
    - Scene management with multiple camera points
    - Playback of scenes in-game
    - Export scene points to the server console for reference or saving

    Commands & Usage:
    -----------------
    1. /flymode
       - Enables NoClip mode for free camera movement in 3D space.
       - Keys:
         - Move forward/backward: W / S
         - Strafe left/right: A / D
         - Jump / Crouch: Space / Ctrl
       - Toggle off with /noclip again.
       - Credits to Pottus for his/her open-source texture studio. 

    2. /ncspeed <value>
       - Adjusts NoClip movement speed.
       - Example: /ncspeed 2.5 for faster movement.

    3. /addcampoint <sceneid> <time>
       - Adds a camera point (keyframe) at the current camera position.
       - Parameters:
         - sceneid: ID of the scene (0..MAX_CAM_SCENES-1)
         - time: milliseconds to interpolate to the next point
       - Minimum requirement: **2 points per scene** for playback.

    4. /playcam <sceneid>
       - Plays the scene with all points interpolated.
       - Automatically enables spectator mode and resets the camera when done.
       - Displays the number of points in the scene before playback.

    5. /exportscene <sceneid>
       - Prints all camera points in the server console.
       - Each point includes:
         - Camera position (x, y, z)
         - Look-at position (x, y, z)
         - Duration to next point
       - Useful for debugging or saving static scenes.

    Best Practices:
    ----------------
    - Plan camera movement like a storyboard.
    - Add points in logical order for smooth motion.
    - Adjust 'time' per point to control speed.
    - Use /ncspeed to move faster when placing points.
    - You can create multiple scenes (scene IDs 0..MAX_CAM_SCENES-1).
    - Use /exportscene to save coordinates for future reference.

*/



#if defined _INC_interpolate_editor
    #endinput
#endif
#define _INC_interpolate_editor

#include <noclip>
//#include <YSI_Coding\y_hooks>


#define MAX_CAM_POINTS     50
#define MAX_CAM_SCENES     10

enum E_CAM_POINT
{
    Float:cpPosX,
    Float:cpPosY,
    Float:cpPosZ,
    Float:cpLookX,
    Float:cpLookY,
    Float:cpLookZ,
    cpTime
}

// Storage
new CamScene[MAX_CAM_SCENES][MAX_CAM_POINTS][E_CAM_POINT];
new CamPointCount[MAX_CAM_SCENES];


// --- Commands --- //

// Add camera point
CMD:addcampoint(playerid, params[])
{
    new sceneid, time;
    if(sscanf(params, "ii", sceneid, time)) return SendClientMessage(playerid, -1, "Usage: /addcampoint <sceneid> <time>");

    if(sceneid >= MAX_CAM_SCENES || sceneid < 0)
        return SendClientMessage(playerid, -1, "Invalid scene ID.");

    if(CamPointCount[sceneid] >= MAX_CAM_POINTS)
        return SendClientMessage(playerid, -1, "Max points reached.");

    new index = CamPointCount[sceneid];

    new Float:x, Float:y, Float:z, Float:fx, Float:fy, Float:fz;
    GetPlayerCameraPos(playerid, x, y, z);
    GetPlayerCameraFrontVector(playerid, fx, fy, fz);

    CamScene[sceneid][index][cpPosX] = x;
    CamScene[sceneid][index][cpPosY] = y;
    CamScene[sceneid][index][cpPosZ] = z;

    CamScene[sceneid][index][cpLookX] = x + (fx * 10.0);
    CamScene[sceneid][index][cpLookY] = y + (fy * 10.0);
    CamScene[sceneid][index][cpLookZ] = z + (fz * 10.0);

    CamScene[sceneid][index][cpTime] = time;

    CamPointCount[sceneid]++;
    SendClientMessage(playerid, -1, "Camera point saved.");
    return 1;
}

// Export scene
CMD:exportscene(playerid, params[])
{
    new sceneid;
    if(sscanf(params, "i", sceneid)) return SendClientMessage(playerid, -1, "Usage: /exportscene <sceneid>");
    if(sceneid >= MAX_CAM_SCENES || sceneid < 0) return SendClientMessage(playerid, -1, "Scenes have been exceeded to its limit. Reconnect and try again.");

    if(CamPointCount[sceneid] < 1)
    {
        SendClientMessage(playerid, -1, "No points to export.");
        return 1;
    }

    new szFile[64];
    format(szFile, sizeof(szFile), "cameditor_scene %d.ini", sceneid);

    new File:file = fopen(szFile, io_write);
    if(!file)
    {
        SendClientMessage(playerid, -1, "Failed to create INI file.");
        return 1;
    }

    for(new i = 0; i < CamPointCount[sceneid]; i++)
    {
        new string[128];
        format(string, sizeof(string), "Point %d =%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %dsecs\n",
            i,
            CamScene[sceneid][i][cpPosX],
            CamScene[sceneid][i][cpPosY],
            CamScene[sceneid][i][cpPosZ],
            CamScene[sceneid][i][cpLookX],
            CamScene[sceneid][i][cpLookY],
            CamScene[sceneid][i][cpLookZ],
            CamScene[sceneid][i][cpTime]
        );

        fwrite(file, string);
    }

    fwrite(file, "\n");
    fwrite(file, "NOTE: The first point will always be the first cam point. the next points will be where the camera inteprolate go and stop.");

    fclose(file);
    SendClientMessage(playerid, -1, "Scene exported to INI file.");
    return 1;
}


// Play scene
CMD:playcam(playerid, params[])
{
    new sceneid;
    if(sscanf(params, "i", sceneid)) return SendClientMessage(playerid, -1, "Usage: /playcam <sceneid>");
    if(sceneid >= MAX_CAM_SCENES || sceneid < 0) return 1;

    if(CamPointCount[sceneid] < 2)
    {
        SendClientMessage(playerid, -1, "Not enough points to play.");
        return 1;
    }

    TogglePlayerSpectating(playerid, true);
    PlayCamPoint(playerid, sceneid, 0);

    return 1;
}

// --- Helper function to interpolate --- //
forward PlayCamPoint(playerid, sceneid, index);

public PlayCamPoint(playerid, sceneid, index)
{
    if(index >= CamPointCount[sceneid] - 1)
    {
        TogglePlayerSpectating(playerid, false);
        SetCameraBehindPlayer(playerid);
        SendClientMessage(playerid, -1, "Scene playback ended.");
        return 1;
    }

    new next = index + 1;

    InterpolateCameraPos(playerid,
        CamScene[sceneid][index][cpPosX],
        CamScene[sceneid][index][cpPosY],
        CamScene[sceneid][index][cpPosZ],

        CamScene[sceneid][next][cpPosX],
        CamScene[sceneid][next][cpPosY],
        CamScene[sceneid][next][cpPosZ],

        CamScene[sceneid][next][cpTime],
        CAMERA_MOVE
    );

    InterpolateCameraLookAt(playerid,
        CamScene[sceneid][index][cpLookX],
        CamScene[sceneid][index][cpLookY],
        CamScene[sceneid][index][cpLookZ],

        CamScene[sceneid][next][cpLookX],
        CamScene[sceneid][next][cpLookY],
        CamScene[sceneid][next][cpLookZ],

        CamScene[sceneid][next][cpTime],
        CAMERA_MOVE
    );

    SetTimerEx("PlayCamPoint",
        CamScene[sceneid][next][cpTime],
        false,
        "ddd",
        playerid,
        sceneid,
        next
    );

    return 1;
}
